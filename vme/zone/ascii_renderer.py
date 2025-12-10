#!/usr/bin/env python3
"""
ASCII Map Renderer for DikuMUD Zones
Handles rendering of zone paths to ASCII art
"""

from typing import List, Tuple, Set, Dict
from constants import default_elongate

class ASCIIMapRenderer:
    def __init__(self, rooms):
        self.rooms = rooms

    def render_ascii_map(self, all_paths: List[Tuple[int, int, List[str], int]]) -> str:
        """Render ASCII map from bidirectional paths"""
        # Create a large grid
        width = 100
        height = 50
        grid = [[' ' for _ in range(width)] for _ in range(height)]
        
        rendered_paths = set()
        room_positions = {}
        
        # Direction vectors for rendering (step = elongate + 1, assuming elongate=1)
        direction_vectors = {
            0: (0, -2),   # north
            1: (2, 0),    # east
            2: (0, 2),    # south
            3: (-2, 0),   # west
            4: (0, -2),   # up (same as north for 2D)
            5: (0, 2),    # down (same as south)
            6: (-2, -2),  # northwest
            7: (2, -2),   # northeast
            8: (-2, 2),   # southwest
            9: (2, 2),    # southeast
            10: (2, 0),   # enter (same as east)
            11: (-2, 0)   # exit (same as west)
        }
        
        collision_occurred = False
        collision_pos = None
        
        for dir1, dir2, path, length in all_paths:
            if len(path) < 2:
                continue
                
            # Determine direction vector (use dir1)
            dx, dy = direction_vectors.get(dir1, (1, 0))
            
            # Choose character based on direction
            if dx != 0 and dy == 0:
                char = '-'
            elif dx == 0 and dy != 0:
                char = '|'
            else:
                char = '/'
            
            # Try to render the path
            collision, pos = self._render_path(grid, path, rendered_paths, room_positions, dx, dy, char)
            if collision:
                collision_occurred = True
                if pos:
                    collision_pos = pos
                break
        
        # Mark collision if occurred
        if collision_occurred:
            self._mark_collision_at_point(grid, path, collision_pos)
        
        return self._format_grid_output(grid, collision_occurred)

    def _mark_collision_at_point(self, grid: List[List[str]], path: List[str], collision_pos: Tuple[int,int] = None):
        """Mark collision location where path attempted to render"""
        if collision_pos:
            x, y = collision_pos
            if 0 <= x < len(grid[0]) and 0 <= y < len(grid):
                grid[y][x] = '!'
                if x + 1 < len(grid[0]):
                    grid[y][x + 1] = '!'
            return
        # Fallback to old way
        for y in range(len(grid)):
            for x in range(len(grid[0]) - 1):
                if grid[y][x] != ' ':
                    grid[y][x] = '!'
                    if x + 1 < len(grid[0]):
                        grid[y][x + 1] = '!'
                    return

    def _format_grid_output(self, grid: List[List[str]], collision_occurred: bool = False) -> str:
        """Format grid for display"""
        # Trim empty rows/columns
        min_row = len(grid)
        max_row = -1
        min_col = len(grid[0]) if grid else 0
        max_col = -1
        
        for y, row in enumerate(grid):
            for x, char in enumerate(row):
                if char != ' ':
                    min_row = min(min_row, y)
                    max_row = max(max_row, y)
                    min_col = min(min_col, x)
                    max_col = max(max_col, x)
                    
        if min_row > max_row:
            return "Empty map"
            
        # Extract and format the relevant portion
        result = []
        
        for y in range(min_row, max_row + 1):
            row_str = ""
            for x in range(min_col, max_col + 1):
                row_str += grid[y][x]
            result.append(f"     {row_str}")
        
        if collision_occurred:
            result.append("")
            result.append("!! - Collision detected, rendering stopped")
        
        return '\n'.join(result)

    def _render_path(self, grid, path, rendered_paths, room_positions, dx, dy, char):
        if len(path) < 2:
            return False
        actual_dx = dx
        actual_dy = dy
        # Try to connect to existing if possible
        if len(rendered_paths) > 0:
            existing_rooms = room_positions
            connection_candidates = [room_name for room_name in path if room_name in existing_rooms]
            connection_room = None
            connection_index = -1
            if connection_candidates:
                # Choose the connection room closest to the center of the grid
                center_x = len(grid[0]) // 2
                connection_room = min(connection_candidates, key=lambda r: abs(existing_rooms[r][0] - center_x))
                connection_index = path.index(connection_room)
                print(f"Debug: Path {path[0]} -> {path[-1]}, connection candidates: {connection_candidates}, chosen: {connection_room} at x={existing_rooms[connection_room][0]}, center_x={center_x}")

            if connection_room:
                conn_x, conn_y = existing_rooms[connection_room]
                # Check if can place the other rooms
                for i, room_name in enumerate(path):
                    if i == connection_index:
                        continue
                    offset = i - connection_index
                    px = conn_x + offset * actual_dx
                    py = conn_y + offset * actual_dy
                    if not (0 <= px < len(grid[0]) and 0 <= py < len(grid) and grid[py][px] == ' '):
                        return True, (px, py) if 0 <= px < len(grid[0]) and 0 <= py < len(grid) else None
                # Place
                for i, room_name in enumerate(path):
                    if i == connection_index:
                        continue
                    offset = i - connection_index
                    px = conn_x + offset * actual_dx
                    py = conn_y + offset * actual_dy
                    grid[py][px] = 'O'
                    room_positions[room_name] = (px, py)
                # Draw connections
                for i in range(len(path)-1):
                    p1i = i
                    p2i = i+1
                    p1x = conn_x + (p1i - connection_index) * actual_dx
                    p1y = conn_y + (p1i - connection_index) * actual_dy
                    p2x = conn_x + (p2i - connection_index) * actual_dx
                    p2y = conn_y + (p2i - connection_index) * actual_dy
                    elongation = default_elongate
                    if path[p1i] in self.rooms:
                        room = self.rooms[path[p1i]]
                        for d, exit_info in room.exits.items():
                            if exit_info[0] == path[p2i]:
                                elongation = exit_info[1]
                                break
                    # Draw line
                    if actual_dx != 0 and actual_dy == 0:  # horizontal
                        y = p1y
                        start_x = min(p1x, p2x)
                        end_x = max(p1x, p2x)
                        for gx in range(start_x + 1, end_x):
                            grid[y][gx] = char

                    elif actual_dx == 0 and actual_dy != 0:  # vertical
                        x = p1x
                        start_y = min(p1y, p2y)
                        end_y = max(p1y, p2y)
                        for gy in range(start_y + 1, end_y):
                            grid[gy][x] = char

                    else:  # diagonal
                        mx = (p1x + p2x) // 2
                        my = (p1y + p2y) // 2
                        grid[my][mx] = char
                rendered_paths.add(frozenset(path))
                return False, None
            # else:
                #     # No connection, for diagonal, can't render
                #     if dx != 0 and dy != 0:
                #         return True
            if not connection_room and dx != 0 and dy != 0:
                return True, None
        # Standalone placement
        center = len(grid) // 2
        if dx > 0 and dy == 0:  # east
            start_x = 2
            start_y = center
        elif dx < 0 and dy == 0:  # west
            start_x = len(grid[0]) - 2 - (len(path)-1) * abs(dx)
            start_y = center
        elif dx == 0 and dy > 0:  # south
            start_x = center
            start_y = 2
        elif dx == 0 and dy < 0:  # north
            start_x = center
            start_y = len(grid) - 2 - (len(path)-1) * abs(dy)
        elif dx > 0 and dy < 0:  # ne
            start_x = 2
            start_y = len(grid) - 2 - (len(path)-1) * abs(dy)
        elif dx < 0 and dy < 0:  # nw
            start_x = len(grid[0]) - 2 - (len(path)-1) * abs(dx)
            start_y = len(grid) - 2 - (len(path)-1) * abs(dy)
        elif dx > 0 and dy > 0:  # se
            start_x = 2
            start_y = 2
        elif dx < 0 and dy > 0:  # sw
            start_x = len(grid[0]) - 2 - (len(path)-1) * abs(dx)
            start_y = 2
        else:
            return True, None
        # Check bounds
        end_x = start_x + (len(path)-1) * dx
        end_y = start_y + (len(path)-1) * dy
        if end_x < 0 or end_x >= len(grid[0]) or end_y < 0 or end_y >= len(grid):
            return True, None
        # Check collisions
        for i in range(len(path)):
            x = start_x + i * dx
            y = start_y + i * dy
            if grid[y][x] != ' ':
                return True, (x, y)
        # Place
        for i, room in enumerate(path):
            x = start_x + i * dx
            y = start_y + i * dy
            grid[y][x] = 'O'
            room_positions[room] = (x, y)
        # Draw connections
        for i in range(len(path)-1):
            x1 = start_x + i * dx
            y1 = start_y + i * dy
            x2 = start_x + (i+1) * dx
            y2 = start_y + (i+1) * dy
            elongation = default_elongate
            if path[i] in self.rooms:
                room = self.rooms[path[i]]
                for d, exit_info in room.exits.items():
                    if exit_info[0] == path[i+1]:
                        elongation = exit_info[1]
                        break
            if dx != 0 and dy == 0:
                y = y1
                for gx in range(min(x1, x2) + 1, max(x1, x2)):
                    grid[y][gx] = char

            elif dx == 0 and dy != 0:
                x = x1
                for gy in range(min(y1, y2) + 1, max(y1, y2)):
                    grid[gy][x] = char

            else:
                mx = (x1 + x2) // 2
                my = (y1 + y2) // 2
                grid[my][mx] = char
        rendered_paths.add(frozenset(path))
        return False, None
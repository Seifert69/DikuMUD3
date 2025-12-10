#!/usr/bin/env python3
"""
ASCII Map Renderer for DikuMUD Zones
Handles rendering of zone paths to ASCII art
"""

from typing import List, Tuple, Set, Dict

class ASCIIMapRenderer:
    def __init__(self, rooms):
        self.rooms = rooms

    def get_direction_vector(self, dir_index: int) -> Tuple[int, int]:
        """Get base direction vector for given direction"""
        vectors = {
            0: (0, -1),   # north
            1: (1, 0),    # east
            2: (0, 1),    # south
            3: (-1, 0),   # west
            4: (0, -1),   # up (same as north for 2D)
            5: (0, 1),    # down (same as south)
            6: (-1, -1),  # northwest
            7: (1, -1),   # northeast
            8: (-1, 1),   # southwest
            9: (1, 1),    # southeast
            10: (1, 0),   # enter (same as east)
            11: (-1, 0)   # exit (same as west)
        }
        return vectors.get(dir_index, (1, 0))

    def render_ascii_map(self, all_paths: List[Tuple[int, int, List[str], int]]) -> str:
        """Render ASCII map from bidirectional paths"""
        # Create a large grid
        width = 100
        height = 50
        grid = [[' ' for _ in range(width)] for _ in range(height)]
        
        rendered_paths = set()
        room_positions = {}
        

        
        collision_occurred = False
        collision_pos = None
        
        for dir1, dir2, path, length in all_paths[0:4]:
            if len(path) < 2:
                continue

            # Determine base direction vector (use dir1)
            dx, dy = self.get_direction_vector(dir1)

            # Choose character based on direction
            if dx != 0 and dy == 0:
                char = '-'
            elif dx == 0 and dy != 0:
                char = '|'
            else:
                char = '/'

            # Try to render the path
            collision, pos = self._render_path(grid, path, rendered_paths, room_positions, dx, dy, char, dir1)
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

    def _render_path(self, grid, path, rendered_paths, room_positions, dx, dy, char, dir1):
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
                # Compute steps for each segment
                steps = []
                for i in range(len(path)-1):
                    elongate = None
                    if path[i] in self.rooms:
                        room = self.rooms[path[i]]
                        for d, exit_info in room.exits.items():
                            if exit_info[0] == path[i+1]:
                                elongate = exit_info[1]
                                break
                    if elongate is None:
                        elongate = 1
                    step = elongate * 2 + 1
                    steps.append(step)
                # Compute positions
                positions = [None] * len(path)
                positions[connection_index] = (conn_x, conn_y)
                # Backward
                cum = 0
                for i in range(connection_index - 1, -1, -1):
                    cum -= steps[i]
                    px = conn_x + cum * actual_dx
                    py = conn_y + cum * actual_dy
                    positions[i] = (px, py)
                # Forward
                cum = 0
                for i in range(connection_index + 1, len(path)):
                    cum += steps[i-1]
                    px = conn_x + cum * actual_dx
                    py = conn_y + cum * actual_dy
                    positions[i] = (px, py)
                # Check bounds and collisions
                for i, (px, py) in enumerate(positions):
                    if i == connection_index:
                        continue
                    if not (0 <= px < len(grid[0]) and 0 <= py < len(grid)):
                        return True, None
                    if grid[py][px] != ' ':
                        return True, (px, py)
                # Place
                for i, (px, py) in enumerate(positions):
                    grid[py][px] = 'O'
                    room_positions[path[i]] = (px, py)
                # Draw connections
                for i in range(len(path)-1):
                    p1x, p1y = positions[i]
                    p2x, p2y = positions[i+1]
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
            start_x = len(grid[0]) - 2
            start_y = center
        elif dx == 0 and dy > 0:  # south
            start_x = center
            start_y = 2
        elif dx == 0 and dy < 0:  # north
            start_x = center
            start_y = len(grid) - 2
        elif dx > 0 and dy < 0:  # ne
            start_x = 2
            start_y = len(grid) - 2
        elif dx < 0 and dy < 0:  # nw
            start_x = len(grid[0]) - 2
            start_y = len(grid) - 2
        elif dx > 0 and dy > 0:  # se
            start_x = 2
            start_y = 2
        elif dx < 0 and dy > 0:  # sw
            start_x = len(grid[0]) - 2
            start_y = 2
        else:
            return True, None

        # Calculate positions with varying elongates
        current_x = start_x
        current_y = start_y
        positions = [(current_x, current_y)]
        for i in range(len(path)-1):
            from_room = path[i]
            to_room = path[i+1]
            elongate = None
            d = dir1  # default to path direction
            if from_room in self.rooms:
                room = self.rooms[from_room]
                for dir_index, (target, el, _) in room.exits.items():
                    if target == to_room:
                        elongate = el
                        d = dir_index
                        break
            if elongate is None:
                elongate = 1
            base_dx, base_dy = self.get_direction_vector(d)
            step = elongate * 2 + 1
            current_x += step * base_dx
            current_y += step * base_dy
            positions.append((current_x, current_y))

        # Check bounds and collisions
        for px, py in positions:
            if not (0 <= px < len(grid[0]) and 0 <= py < len(grid)):
                return True, None
            if grid[py][px] != ' ':
                return True, (px, py)

        # Place
        for i, (px, py) in enumerate(positions):
            grid[py][px] = 'O'
            room_positions[path[i]] = (px, py)

        # Draw connections
        for i in range(len(path)-1):
            x1, y1 = positions[i]
            x2, y2 = positions[i+1]
            if x1 == x2:  # vertical
                for gy in range(min(y1, y2) + 1, max(y1, y2)):
                    grid[gy][x1] = '|'
            elif y1 == y2:  # horizontal
                for gx in range(min(x1, x2) + 1, max(x1, x2)):
                    grid[y1][gx] = '-'
            else:  # diagonal
                mx = (x1 + x2) // 2
                my = (y1 + y2) // 2
                grid[my][mx] = '/'
        rendered_paths.add(frozenset(path))
        return False, None
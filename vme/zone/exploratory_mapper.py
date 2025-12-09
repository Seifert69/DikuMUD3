#!/usr/bin/env python3
"""
Exploratory Path Analyzer for DikuMUD Zones
Implements longest-path-first algorithm for path analysis
"""

import re
from collections import defaultdict, deque
from typing import Dict, List, Tuple, Set, Optional

default_elongate = 1

# Direction mappings for 12 exit directions
DIRECTION_MAP = {
    0: 'north',     1: 'east',      2: 'south',     3: 'west',
    4: 'up',        5: 'down',      6: 'northwest',  7: 'northeast',
    8: 'southwest', 9: 'southeast', 10: 'enter',     11: 'exit'
}

# Opposite direction mappings
OPPOSITE_DIRECTIONS = {
    0: 2,   # north -> south
    1: 3,   # east -> west
    2: 0,   # south -> north
    3: 1,   # west -> east
    4: 5,   # up -> down
    5: 4,   # down -> up
    6: 9,   # northwest -> southeast
    7: 8,   # northeast -> southwest
    8: 7,   # southwest -> northeast
    9: 6,   # southeast -> northwest
    10: 11, # enter -> exit
    11: 10  # exit -> enter
}

class Room:
    def __init__(self, name: str, label: str):
        self.name = name
        self.label = label
        self.exits = {}  # Dictionary: {direction: (target_room_id, path_elongation, path_length)}
        
    def __repr__(self):
        return f"Room({self.name})"

class ExploratoryPathAnalyzer:
    def __init__(self, dot_file: str):
        self.rooms: Dict[str, Room] = {}
        self.zone_name = ""
        self.parse_dot_file(dot_file)
        
    def parse_dot_file(self, filename: str):
        """Parse the DOT file and extract rooms and connections"""
        with open(filename, 'r') as f:
            content = f.read()
            
        # Extract zone name
        zone_match = re.search(r'subgraph "cluster@([^"]+)"', content)
        if zone_match:
            self.zone_name = zone_match.group(1)
            
        # Extract room definitions
        room_pattern = r'"([^"]+@[^"]+)"\s*\[label="([^"]+)"\]'
        for match in re.finditer(room_pattern, content):
            full_name = match.group(1)
            label = match.group(2)
            room = Room(full_name, label)
            self.rooms[full_name] = room
            
        # Extract connections and populate the 12-direction exit structure
        connection_pattern = r'"([^"]+@[^"]+)"\s*->\s*"([^"]+@[^"]+)"\s*\[dir=([^\]]+)\]'
        for match in re.finditer(connection_pattern, content):
            from_room = match.group(1)
            to_room = match.group(2)
            direction = match.group(3)
            
            if from_room in self.rooms and to_room in self.rooms:
                room = self.rooms[from_room]
                # Find direction index
                dir_index = None
                for idx, dir_name in DIRECTION_MAP.items():
                    if dir_name == direction:
                        dir_index = idx
                        break
                        
                if dir_index is not None:
                    # Initialize with default values: (target_room_id, path_elongation=1, path_length=0)
                    room.exits[dir_index] = (to_room, default_elongate, 0)
                    
    def calculate_path_lengths(self):
        """Calculate the longest paths for each direction pair"""
        print("Calculating path lengths for opposite direction pairs...")
        
        # For each direction, find the longest path in that direction
        for dir_index in range(12):
            opposite_dir = OPPOSITE_DIRECTIONS[dir_index]
            
            # Only process each pair once (e.g., process north-south but not south-north)
            if dir_index < opposite_dir:
                self._calculate_bidirectional_path_length(dir_index, opposite_dir)
                
    def _calculate_bidirectional_path_length(self, dir1: int, dir2: int):
        """Calculate path length for a bidirectional pair (e.g., north-south)"""
        dir1_name = DIRECTION_MAP[dir1]
        dir2_name = DIRECTION_MAP[dir2]
        
        # Find all paths in this direction pair (including inter-zone)
        all_paths = []
        
        for room_name, room in self.rooms.items():
            # Try to build path going in dir1 direction
            path1 = self._build_unidirectional_path_with_interzone(room_name, dir1)
            if len(path1) > 1:
                all_paths.append(path1)
                
            # Try to build path going in dir2 direction
            path2 = self._build_unidirectional_path_with_interzone(room_name, dir2)
            if len(path2) > 1:
                all_paths.append(path2)
                
        # Find the longest path
        if all_paths:
            longest_path = max(all_paths, key=len)
            path_length = len(longest_path)
            
            # Update path_length for all rooms in this path (only for internal rooms)
            for room_name in longest_path:
                if room_name in self.rooms:
                    room = self.rooms[room_name]
                    
                    # Update both directions if they exist
                    if dir1 in room.exits:
                        target, elongation, _ = room.exits[dir1]
                        room.exits[dir1] = (target, elongation, path_length)
                        
                    if dir2 in room.exits:
                        target, elongation, _ = room.exits[dir2]
                        room.exits[dir2] = (target, elongation, path_length)
                    
            print(f"  {dir1_name}-{dir2_name}: longest path has {path_length} rooms")
            print(f"    Path: {' -> '.join([self.rooms[r].label if r in self.rooms else r for r in longest_path])}")
            
    def _build_unidirectional_path_with_interzone(self, start_room: str, direction: int) -> List[str]:
        """Build a path going in one direction from start_room (including inter-zone connections)"""
        path = [start_room]
        visited = {start_room}
        current_room = start_room
        
        while True:
            # Get next room from DOT file directly
            next_room = self._get_next_room_from_dot(current_room, direction)
            if next_room is None or next_room in visited:
                break
                
            path.append(next_room)
            visited.add(next_room)
            current_room = next_room
            
        return path
        
    def _get_next_room_from_dot(self, from_room: str, direction: str) -> Optional[str]:
        """Get next room from DOT file for a given direction"""
        direction_name = DIRECTION_MAP[direction] if isinstance(direction, int) else direction
        
        # Re-read DOT file to find the connection
        with open('udgaard.dot', 'r') as f:
            content = f.read()
            
        connection_pattern = fr'"({re.escape(from_room)})"\s*->\s*"([^"]+)"\s*\[dir=({re.escape(direction_name)})\]'
        match = re.search(connection_pattern, content)
        if match:
            return match.group(2)
        return None
        
    def find_all_bidirectional_paths(self) -> List[Tuple[int, int, List[str], int]]:
        """Find all unique maximal bidirectional paths sorted by length (longest first)"""
        bidirectional_paths = []
        seen_path_sets = set()
        
        # Process each direction pair only once
        processed_pairs = set()
        
        for dir_index in range(12):
            opposite_dir = OPPOSITE_DIRECTIONS[dir_index]
            
            # Only process each pair once (e.g., process north-south but not south-north)
            pair = tuple(sorted([dir_index, opposite_dir]))
            if pair in processed_pairs:
                continue
            processed_pairs.add(pair)
            
            # Find all maximal paths for this direction pair
            maximal_paths = self._find_maximal_paths_for_pair(dir_index, opposite_dir)
            
            for path in maximal_paths:
                path_set = frozenset(path)
                if path_set not in seen_path_sets:
                    bidirectional_paths.append((dir_index, opposite_dir, path, len(path)))
                    seen_path_sets.add(path_set)
                
        # Sort by length (longest first)
        bidirectional_paths.sort(key=lambda x: x[3], reverse=True)
        return bidirectional_paths
        
    def _find_maximal_paths_for_pair(self, dir1: int, dir2: int) -> List[List[str]]:
        """Find all maximal paths for a direction pair (paths that can't be extended)"""
        maximal_paths = []
        visited_rooms = set()
        
        for room_name in self.rooms:
            if room_name in visited_rooms:
                continue
                
            # Try to build a maximal path starting from this room in both directions
            path = self._build_maximal_bidirectional_path_with_interzone(room_name, dir1, dir2)
            
            if len(path) > 1:
                maximal_paths.append(path)
                # Mark all rooms in this path as visited to avoid duplicates
                visited_rooms.update([r for r in path if r in self.rooms])
                
        return maximal_paths
        
    def _build_maximal_bidirectional_path_with_interzone(self, start_room: str, dir1: int, dir2: int) -> List[str]:
        """Build maximal bidirectional path starting from start_room (including inter-zone connections)"""
        # Extend backward in dir2 direction
        backward_path = []
        current_room = start_room
        while True:
            next_room = self._get_next_room_from_dot(current_room, dir2)
            if next_room is None or next_room in backward_path or next_room == start_room:
                break
            backward_path.append(next_room)
            current_room = next_room
            
        # Extend forward in dir1 direction
        forward_path = [start_room]
        current_room = start_room
        while True:
            next_room = self._get_next_room_from_dot(current_room, dir1)
            if next_room is None or next_room in forward_path:
                break
            forward_path.append(next_room)
            current_room = next_room
            
        # Combine backward (reversed) and forward paths
        full_path = list(reversed(backward_path)) + forward_path
        return full_path
        
    def _build_maximal_bidirectional_path(self, start_room: str, dir1: int, dir2: int) -> List[str]:
        """Build maximal bidirectional path starting from start_room (including inter-zone connections)"""
        # Extend backward in dir2 direction
        backward_path = []
        current_room = start_room
        while True:
            room = self.rooms[current_room]
            if dir2 not in room.exits:
                break
            next_room = room.exits[dir2][0]
            if next_room in backward_path or next_room == start_room:
                break
            backward_path.append(next_room)
            current_room = next_room
            
        # Extend forward in dir1 direction
        forward_path = [start_room]
        current_room = start_room
        while True:
            room = self.rooms[current_room]
            if dir1 not in room.exits:
                break
            next_room = room.exits[dir1][0]
            if next_room in forward_path:
                break
            forward_path.append(next_room)
            current_room = next_room
            
        # Combine backward (reversed) and forward paths
        full_path = list(reversed(backward_path)) + forward_path
        return full_path
        
    def show_room_structure(self, room_name: str = None) -> str:
        """Display the room data structure for debugging"""
        result = ["Room Data Structure:"]
        result.append("=" * 40)
        
        if room_name and room_name in self.rooms:
            rooms_to_show = {room_name: self.rooms[room_name]}
        else:
            rooms_to_show = self.rooms
            
        for name, room in rooms_to_show.items():
            result.append(f"{name}:")
            result.append(f"  name: '{room.name}'")
            result.append(f"  exits: {{")
            
            for dir_index, exit_info in room.exits.items():
                target, elongation, path_length = exit_info
                dir_name = DIRECTION_MAP[dir_index]
                result.append(f"    {dir_index}: ('{target}', {elongation}, {path_length}),  # {dir_name}")
                
            result.append(f"  }}")
            result.append("")
            
        return '\n'.join(result)
        
    def find_interzone_connections(self) -> List[Tuple[str, str, str]]:
        """Find all inter-zone (one-way) connections from DOT file"""
        interzone_connections = []
        
        # Re-read the DOT file to find connections to other zones
        with open('udgaard.dot', 'r') as f:
            content = f.read()
            
        # Extract all connections and check for inter-zone ones
        connection_pattern = r'"([^"]+@[^"]+)"\s*->\s*"([^"]+)"\s*\[dir=([^\]]+)\]'
        for match in re.finditer(connection_pattern, content):
            from_room = match.group(1)
            to_room = match.group(2)
            direction = match.group(3)
            
            # Check if from_room is in our zone and to_room is NOT in our zone
            if from_room in self.rooms and to_room not in self.rooms:
                from_label = self.rooms[from_room].label
                # Extract zone name from target room
                target_zone = to_room.split('@')[1] if '@' in to_room else to_room
                interzone_connections.append((from_label, target_zone, direction))
                
        return interzone_connections
        
    def render_ascii_map(self) -> str:
        """Render ASCII map based on room data structure"""
        print("Rendering ASCII map...")
        
        # Get all paths
        all_paths = self.find_all_bidirectional_paths()
        
        if not all_paths:
            return "No paths to render"
            
        # Initialize map grid - make it wider for horizontal paths
        grid_width = 120
        grid_height = 80
        grid = [[' ' for _ in range(grid_width)] for _ in range(grid_height)]
        rendered_paths = set()
        room_positions = {}
        
        elong = default_elongate
        direction_lookup = {
            (1, 3): {'dx': 2*elong + 1, 'dy': 0, 'char': '-'},  # east/west
            (3, 1): {'dx': 2*elong + 1, 'dy': 0, 'char': '-'},  # west/east
            (0, 2): {'dx': 0, 'dy': -(2*elong + 1), 'char': '|'},  # north/south
            (2, 0): {'dx': 0, 'dy': -(2*elong + 1), 'char': '|'},  # south/north
            (6, 9): {'dx': -(elong + 1), 'dy': -(elong + 1), 'char': '\\'},  # northwest/southeast
            (9, 6): {'dx': -(elong + 1), 'dy': -(elong + 1), 'char': '\\'},  # southeast/northwest
            (7, 8): {'dx': elong + 1, 'dy': -(elong + 1), 'char': '/'},  # northeast/southwest
            (8, 7): {'dx': elong + 1, 'dy': -(elong + 1), 'char': '/'},  # southwest/northeast
        }
        
        # Render paths: longest first, then those that connect
        collision_occurred = False

        # Step 1: Render the longest path
        dir1, dir2, longest_path, length = all_paths[0]
        config = direction_lookup.get((dir1, dir2)) or direction_lookup.get((dir2, dir1))
        if not config:
            raise ValueError(f"No rendering config found for direction pair ({dir1}, {dir2})")

        collision_occurred, _ = self._render_path(grid, longest_path, rendered_paths, room_positions, config['dx'], config['dy'], config['char'])
        if collision_occurred:
            raise ValueError(f"Impossible collision occurred on the first path!?")

        # Step 2: Render remaining paths that connect
        for dir1, dir2, path, length in all_paths[1:5]:
            path_set = frozenset(path)
            
            # Skip if already rendered
            if path_set in rendered_paths:
                continue
                
            # For the first path, render it regardless of connections
            # For subsequent paths, only render if they connect to existing network
            if len(rendered_paths) > 0:
                connects_to_rendered = any(room in rendered_path for room in path for rendered_path in rendered_paths)
                if not connects_to_rendered:
                    continue
                    
            config = direction_lookup.get((dir1, dir2)) or direction_lookup.get((dir2, dir1))
            if not config:
                raise ValueError(f"No rendering config found for direction pair ({dir1}, {dir2})")
                
            collision_occurred_render, collision_pos = self._render_path(grid, path, rendered_paths, room_positions, config['dx'], config['dy'], config['char'])

            # If collision occurred during rendering, mark it and stop
            if collision_occurred_render:
                collision_occurred = True
                print(f"Collision detected while rendering path: {path[0]} -> {path[-1]}")
                # Mark collision location with !!
                self._mark_collision_at_point(grid, path, collision_pos)
                break
                
        return self._format_grid_output(grid, collision_occurred)

    def _mark_collision_at_point(self, grid: List[List[str]], path: List[str], collision_pos: Optional[Tuple[int,int]] = None):
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
 
    def analyze_paths(self) -> str:
        """Main analysis function"""
        print(f"Analyzing paths for zone: {self.zone_name}")
        print(f"Total rooms: {len(self.rooms)}")
        
        # Calculate path lengths
        self.calculate_path_lengths()
        
        # Find all bidirectional paths
        all_paths = self.find_all_bidirectional_paths()
        
        # Find inter-zone connections
        interzone_connections = self.find_interzone_connections()
        
        # Generate output
        result = [f"Complete Path Analysis for {self.zone_name}"]
        result.append("=" * 60)
        result.append(f"Total rooms: {len(self.rooms)}")
        result.append(f"Total unique bidirectional paths found: {len(all_paths)}")
        result.append(f"Total inter-zone connections: {len(interzone_connections)}")
        result.append("")
        result.append("INTER-ZONE CONNECTIONS (one-way):")
        result.append("-" * 40)
        
        for i, (from_label, target_zone, direction) in enumerate(interzone_connections):
            result.append(f"{i+1}. {from_label} -> {target_zone} ({direction})")
            
        result.append("")
        result.append("INTERNAL PATHS sorted by length (longest first):")
        result.append("-" * 60)
        
        for i, (dir1, dir2, path, length) in enumerate(all_paths):
            dir1_name = DIRECTION_MAP[dir1]
            dir2_name = DIRECTION_MAP[dir2]
            
            result.append(f"{i+1}. {dir1_name}/{dir2_name}: {length} rooms")
            result.append(f"   {path[0]} -> {path[-1]}")
            
            # Build path with elongation/total_length indicators
            path_with_lengths = []
            for j, room_name in enumerate(path):
                # Handle both internal and inter-zone rooms with zone qualification
                if room_name in self.rooms:
                    room_label = f"{self.rooms[room_name].label}@udgaard"
                else:
                    room_label = room_name  # Already includes zone name
                    
                path_with_lengths.append(room_label)
                
                # Add elongation/total_length indicator if not the last room
                if j < len(path) - 1:
                    # Find direction and get connection info
                    next_room = path[j + 1]
                    if room_name in self.rooms:
                        elongation, _ = self._get_connection_info(room_name, next_room)
                    else:
                        elongation = default_elongate  # Default for inter-zone connections
                    # Use the overall path length for consistency
                    path_with_lengths.append(f"-({elongation}/{length})>")
                    
            result.append(f"   {' '.join(path_with_lengths)}")
            result.append("")
            
        return '\n'.join(result)
        
    def _get_connection_info(self, from_room: str, to_room: str) -> Tuple[int, int]:
        """Get (path_elongation, total_length) for a connection between two rooms"""
        if from_room not in self.rooms:
            return (0, 0)
            
        room = self.rooms[from_room]
        for dir_index, (target, elongation, path_length) in room.exits.items():
            if target == to_room:
                return (elongation, path_length)
        return (0, 0)
            
        room = self.rooms[from_room]
        for dir_index, (target, elongation, path_length) in room.exits.items():
            if target == to_room:
                return path_length
        return 0

def main():
    """Test the exploratory path analyzer"""
    import sys
    dot_file = sys.argv[1] if len(sys.argv) > 1 else 'udgaard.dot'
    
    analyzer = ExploratoryPathAnalyzer(dot_file)
    
    # Show path analysis
    analysis = analyzer.analyze_paths()
    print(analysis)
    print("\n" + "="*60)
    
    # Show ASCII map
    ascii_map = analyzer.render_ascii_map()
    print("ASCII MAP:")
    print("-" * 40)
    print(ascii_map)

if __name__ == "__main__":
    main()
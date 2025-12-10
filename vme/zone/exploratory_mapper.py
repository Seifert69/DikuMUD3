#!/usr/bin/env python3
"""
Exploratory Path Analyzer for DikuMUD Zones
Implements longest-path-first algorithm for path analysis
"""

import re
from collections import defaultdict, deque
from typing import Dict, List, Tuple, Set, Optional

from constants import DIRECTION_MAP, OPPOSITE_DIRECTIONS, default_elongate
from dot_reader import Room, DotReader
from ascii_renderer import ASCIIMapRenderer


class ExploratoryPathAnalyzer:
    def __init__(self, dot_file: str):
        self.dot_reader = DotReader(dot_file)
        self.rooms = self.dot_reader.rooms
        self.zone_name = self.dot_reader.zone_name

    def set_room_elongate_bidir(self, room_name: str, direction: int, elongate: int):
        """Set elongate for a room's exit in both directions if the connection exists"""
        if room_name not in self.rooms:
            return
        room = self.rooms[room_name]
        if direction not in room.exits:
            return
        target, _, length = room.exits[direction]
        room.exits[direction] = (target, elongate, length)

        # Set in the opposite direction from the target room
        opp_dir = OPPOSITE_DIRECTIONS[direction]
        if target in self.rooms:
            target_room = self.rooms[target]
            if opp_dir in target_room.exits and target_room.exits[opp_dir][0] == room_name:
                _, _, tgt_length = target_room.exits[opp_dir]
                target_room.exits[opp_dir] = (room_name, elongate, tgt_length)


                    
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
            next_room = self.dot_reader.get_next_room_from_dot(current_room, direction)
            if next_room is None or next_room in visited:
                break
                
            path.append(next_room)
            visited.add(next_room)
            current_room = next_room
            
        return path
        

        
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
            next_room = self.dot_reader.get_next_room_from_dot(current_room, dir2)
            if next_room is None or next_room in backward_path or next_room == start_room:
                break
            backward_path.append(next_room)
            current_room = next_room
            
        # Extend forward in dir1 direction
        forward_path = [start_room]
        current_room = start_room
        while True:
            next_room = self.dot_reader.get_next_room_from_dot(current_room, dir1)
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
        return self.dot_reader.find_interzone_connections()
        
    def get_rooms(self):
        return self.rooms

    def get_all_bidirectional_paths(self):
        return self.find_all_bidirectional_paths()


                    

 
    def analyze_paths(self) -> str:
        """Main analysis function"""
        print(f"Analyzing paths for zone: {self.zone_name}")
        print(f"Total rooms: {len(self.rooms)}")
        
        # Calculate path lengths
        self.calculate_path_lengths()
        
        # Find all bidirectional paths
        all_paths = self.find_all_bidirectional_paths()
        
        # Find inter-zone connections
        interzone_connections = self.dot_reader.find_interzone_connections()
        
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


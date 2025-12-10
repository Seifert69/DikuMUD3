#!/usr/bin/env python3
"""
Dot File Reader for DikuMUD Zones
Handles parsing of .dot files and building room data structures
"""

import re
from typing import Dict, List, Tuple, Optional

from constants import DIRECTION_MAP, default_elongate

class Room:
    def __init__(self, name: str, label: str):
        self.name = name
        self.label = label
        self.exits = {}  # Dictionary: {direction: (target_room_id, path_elongation, path_length)}
        
    def __repr__(self):
        return f"Room({self.name})"

class DotReader:
    def __init__(self, dot_file: str):
        self.dot_file = dot_file
        self.zone_name = ""
        self.rooms: Dict[str, Room] = {}
        self.parse_dot_file()

    def parse_dot_file(self):
        """Parse the DOT file and extract rooms and connections"""
        with open(self.dot_file, 'r') as f:
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

            if from_room in self.rooms:
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

    def get_next_room_from_dot(self, from_room: str, direction: int) -> Optional[str]:
        """Get next room from DOT file for a given direction"""
        direction_name = DIRECTION_MAP[direction] if isinstance(direction, int) else direction
        
        # Re-read DOT file to find the connection
        with open(self.dot_file, 'r') as f:
            content = f.read()
            
        connection_pattern = fr'"({re.escape(from_room)})"\s*->\s*"([^"]+)"\s*\[dir=({re.escape(direction_name)})\]'
        match = re.search(connection_pattern, content)
        if match:
            return match.group(2)
        return None

    def find_interzone_connections(self) -> List[Tuple[str, str, str]]:
        """Find all inter-zone (one-way) connections from DOT file"""
        interzone_connections = []
        
        # Re-read the DOT file to find connections to other zones
        with open(self.dot_file, 'r') as f:
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
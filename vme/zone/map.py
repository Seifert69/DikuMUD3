#!/usr/bin/env python3
"""
Main entry point for the DikuMUD Zone Map Analyzer
Parses command line arguments and orchestrates the analysis
"""

import sys
import random
from exploratory_mapper import ExploratoryPathAnalyzer
from ascii_renderer import ASCIIMapRenderer

def main():
    """Main function to run the zone analysis"""
    args = sys.argv[1:]
    dot_file = 'udgaard.dot'
    num_paths = 10000
    randomize = False

    # Parse arguments, removing -r as we go
    while len(args) > 0:
        if args[0] == '-r':
            randomize = True
            args.pop(0)
            continue

        try:
            num_paths = int(args[0])
            args.pop(0)
            continue
        except ValueError:
            pass

        dot_file = args[0]
        args.pop(0)

    analyzer = ExploratoryPathAnalyzer(dot_file)

    # Show path analysis
    analysis = analyzer.analyze_paths()
    print(analysis)
    print("\n" + "="*60)

    if randomize:
        # Randomize elongates for testing
        rooms = analyzer.get_rooms()
        for room_name, room in rooms.items():
            for direction in room.exits:
                analyzer.set_room_elongate_bidir(room_name, direction, random.randint(1, 3))
        print("\n(Randomized elongates for testing)\n")

    # Show ASCII map
    rooms = analyzer.get_rooms()
    all_paths = analyzer.get_all_bidirectional_paths()
    renderer = ASCIIMapRenderer(rooms)
    ascii_map = renderer.render_ascii_map(all_paths[0:num_paths])

    print("ASCII MAP:")
    print("-" * 40)
    print(ascii_map)

if __name__ == "__main__":
    main()
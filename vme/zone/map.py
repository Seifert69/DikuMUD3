#!/usr/bin/env python3
"""
Main entry point for the DikuMUD Zone Map Analyzer
Parses command line arguments and orchestrates the analysis
"""

import sys
from exploratory_mapper import ExploratoryPathAnalyzer
from ascii_renderer import ASCIIMapRenderer

def main():
    """Main function to run the zone analysis"""
    dot_file = sys.argv[1] if len(sys.argv) > 1 else 'udgaard.dot'
    
    analyzer = ExploratoryPathAnalyzer(dot_file)
    
    # Show path analysis
    analysis = analyzer.analyze_paths()
    print(analysis)
    print("\n" + "="*60)
    
    # Show ASCII map
    rooms = analyzer.get_rooms()
    all_paths = analyzer.get_all_bidirectional_paths()
    renderer = ASCIIMapRenderer(rooms)
    ascii_map = renderer.render_ascii_map(all_paths)
    print("ASCII MAP:")
    print("-" * 40)
    print(ascii_map)

if __name__ == "__main__":
    main()
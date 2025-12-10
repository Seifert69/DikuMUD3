#!/usr/bin/env python3

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
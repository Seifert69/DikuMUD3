#!/bin/bash
#
# Extract a dump of pretty much all items with bonuses
#
../bin/vmc -v -I../include/ *.zon  |& grep "^armor,\|^other,\|^weapon,\|^shield," > ../items.csv

#!/bin/bash

for filename in ./*.zon; do
    echo $filename
    cat $filename | ../vme/bin/fixhtml > $filename.new
done

echo "Done. All .new zones are now completely fixed"

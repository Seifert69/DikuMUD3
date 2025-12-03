#!/bin/bash
for file in *.hlp; do name="${file%.hlp}"; if [[ ! -f "${name}.wiki" ]]; then echo "$file"; break; fi; done

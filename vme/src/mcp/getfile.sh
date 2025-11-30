#!/bin/bash
for file in *_prompt.txt; do name="${file%_prompt.txt}"; if [[ ! -f "${name}.wiki" ]]; then echo "$file"; break; fi; done

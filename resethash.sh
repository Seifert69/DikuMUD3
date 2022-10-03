#!/bin/bash
#
# Run this script to reset the zone hash values.
# Then make twice, and the hash file will be rebuilt
#
sed -i 's/"sha512": "[a-f0-9]\+"/"sha512": ""/' unit_tests/json_configs/zon_file_hashes.json

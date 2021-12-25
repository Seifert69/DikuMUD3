#!/bin/bash
find vme/src -name "*.cpp" -exec clang-format -i {} \;
find vme/src -name "*.h" -exec clang-format -i {} \;

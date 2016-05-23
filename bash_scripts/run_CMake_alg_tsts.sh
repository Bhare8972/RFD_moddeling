#!/bin/sh
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B./build/algorithm_tests -H./source/algorithm_tests
read -p "Press any key to continue or CTRL-C to abort"

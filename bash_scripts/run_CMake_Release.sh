#!/bin/sh
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B./release_build -H./source
read -p "Press any key to continue or CTRL-C to abort

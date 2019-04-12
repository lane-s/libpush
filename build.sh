#!/bin/bash
mkdir -p build
cd build
cmake ..
make
mv compile_commands.json ../compile_commands.json

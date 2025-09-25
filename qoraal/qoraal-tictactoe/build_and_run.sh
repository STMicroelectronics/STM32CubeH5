#!/bin/bash

mkdir build
cd build
cmake ..
cmake --build .
# we want to be in the same directory as tictactoe.e
cd ..
./build/src/tictactoe

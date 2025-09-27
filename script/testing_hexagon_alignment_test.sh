#!/bin/bash

g++ -std=c++17 ./testing/testing_hexagon_alignment_test.cpp -I include -L build -lUIFramework -lSDL2 -o hexagon_alignment_test && LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH ./hexagon_alignment_test

# clear the test binary after running
rm ./hexagon_alignment_test
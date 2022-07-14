#!/bin/bash

g++ stress_test.cpp -o stress_test -g --std=c++11 -pthread
./stress_test
rm -rf stress_test

#!/bin/bash
rm -rf mthr
g++ mthread.cpp -o mthr --std=c++11 -pthread 
./mthr

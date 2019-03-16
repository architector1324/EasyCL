#!/bin/bash

g++ -O2 -Wall -lOpenCL -I ../include/ -I ../vendor/Catch2/single_include/catch2/ -o a.out ./*.cpp
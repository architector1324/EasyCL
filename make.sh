# !/bin/bash

g++ -O2 -fPIC -c src/easycl.cpp -o easycl.o
g++ -shared -o bin/libEasyCL.so easycl.o
rm easycl.o
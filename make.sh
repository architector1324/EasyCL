# !/bin/bash

g++ -O2 -fPIC -c src/gpu.cpp -o gpu.o
g++ -O2 -fPIC -c src/argument.cpp -o argument.o
g++ -O2 -fPIC -c src/kernel.cpp -o kernel.o
g++ -O2 -fPIC -c src/program.cpp -o program.o
g++ -O2 -fPIC -c src/platform.cpp -o platform.o
g++ -O2 -fPIC -c src/error.cpp -o error.o

g++ -shared -o bin/libEasyCL.so error.o platform.o program.o kernel.o argument.o gpu.o
rm gpu.o
rm argument.o
rm kernel.o
rm program.o
rm platform.o
rm error.o

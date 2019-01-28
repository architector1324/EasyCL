#!/bin/bash

mkdir /usr/include/EasyCL
cp src/*.hpp /usr/include/EasyCL/
cp bin/libEasyCL.so /usr/lib/
echo "done."


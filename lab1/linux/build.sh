#!/bin/sh

set -e # exit on error

CXX="c++"

run() {
  echo ">" "$@"
  "$@"
}

echo "Building libnumber.a"
run $CXX number.cpp -fpic -c -o libnumber.a

echo "Building libvector.so"
run $CXX -Wl,--as-needed -L. -lnumber vector.cpp -fpic -shared -o libvector.so

echo "Building main"
run $CXX -Wl,--as-needed -Wl,-rpath='$ORIGIN' -L. main.cpp -lnumber -lvector -fpie -o main

echo "Running main"
./main

echo
echo "For the reference..."
ldd ./main | grep vector

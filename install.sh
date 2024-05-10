#!/bin/bash

mkdir -p build
cmake -B build
cd build
make || exit 1
make uart0 || exit 1
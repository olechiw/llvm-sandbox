#!/bin/bash
rm -r ./build/*
cmake -S . -B ./build -DCMAKE_C_COMPILER=clang-16 -DCMAKE_CXX_COMPILER=clang++-16 -DCMAKE_CXX_FLAGS="-stdlib=libstdc++" 
cmake --build build/

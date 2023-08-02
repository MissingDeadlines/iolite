#!/bin/bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

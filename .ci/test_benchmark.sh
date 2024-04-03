#!/bin/bash

echo "Build renamenoise"

rm -f ./examples/renamenoise_benchmark

cmake -DRENAMENOISE_DEMO_EXECUTABLE=OFF -DRENAMENOISE_BENCHMARK_EXECUTABLE=ON .
make

echo "Benchmark"
./examples/renamenoise_benchmark

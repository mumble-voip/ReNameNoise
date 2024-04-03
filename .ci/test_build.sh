#!/bin/bash

echo "Build renamenoise"

rm -f ./sample/renamenoise_babble_15dB.pcm
rm -f ./examples/renamenoise_demo

cmake .
make

bash ./.ci/test_checksum.sh
bash ./.ci/test_symbols.sh

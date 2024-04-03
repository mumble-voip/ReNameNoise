#!/bin/bash

echo "Build renamenoise without VLA"

rm -f ./sample/renamenoise_babble_15dB.pcm
rm -f ./examples/renamenoise_demo

export CFLAGS="-DRENAMENOISE_NO_VLA=1 -Werror=vla"

cmake .
make

bash ./.ci/test_checksum.sh
bash ./.ci/test_symbols.sh

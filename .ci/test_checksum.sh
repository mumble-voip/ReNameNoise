#!/bin/bash

echo "Apply noise suppression to test sample"
./examples/renamenoise_demo ./sample/babble_15dB.pcm ./sample/renamenoise_babble_15dB.pcm
sha1sum -c --strict ./sample/CHECKSUMS

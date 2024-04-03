#!/bin/bash

echo "Check exported symbols"
FAILED=""
EXPORTED_SYMBOLS=$(nm ./librenamenoise.a | awk '/ T / {print $3}' | sort)
SYMBOL_REGEX='^_?renamenoise.*$'

while IFS= read -r line; do
    if [[ "$line" =~ $SYMBOL_REGEX ]]; then
        echo "passed '$line'";
    else
        echo "failed '$line'";
        FAILED="$FAILED,$line";
    fi;
done <<< "$EXPORTED_SYMBOLS"

if [[ -n "$FAILED" ]]; then
    echo "$FAILED";
    exit 1;
fi;

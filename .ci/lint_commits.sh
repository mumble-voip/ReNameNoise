#!/bin/bash

git fetch

FAILED=""
SUBJECTS=$(git log --pretty=%s origin/$1..HEAD)
SUBJECTS_REGEX='^(BREAK|FEAT|FIX|FORMAT|DOCS|TEST|MAINT|CI|REFAC|BUILD|TRANSLATION|CHANGE|REVERT): .*$'

while IFS= read -r line; do
    if [[ "$line" =~ $SUBJECTS_REGEX ]]; then
        echo "passed '$line'";
    else 
        echo "failed '$line'"; 
        FAILED="$FAILED,$line"; 
    fi; 
done <<< "$SUBJECTS"

if [[ -n "$FAILED" ]]; then
    echo "$FAILED";
    exit 1;
fi;

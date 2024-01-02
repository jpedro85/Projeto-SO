#!/bin/bash

# The executable for which the stack trace was generated
EXECUTABLE=./build/simulador
EXECUTABLE_NAME=$(basename "$EXECUTABLE")

# Read the stack trace from stdin or from a file
while IFS= read -r line; do
    # Check if the line contains the executable name and extract the address
    if [[ $line == *"$EXECUTABLE_NAME"* && $line =~ \(\+(0x[0-9a-fA-F]+)\) ]]; then
        address=${BASH_REMATCH[1]}
        # Use addr2line to get the file name and line number
        addr2line -e "$EXECUTABLE" -f -C -p "$address"
    fi
done

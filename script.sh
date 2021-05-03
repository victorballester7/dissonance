#!/bin/bash
gcc -Wall -pedantic -o dissonance dissonance.c -lm
./dissonance
if [[ $? -eq 0 ]]; then
  gnuplot -p -e "plot [1:2] 'diss.txt' with lines"
fi

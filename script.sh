#!/bin/bash
gcc -Wall -pedantic -o dissonance dissonance.c -lm
./dissonance
if [[ $? -eq 0 ]]; then
  gnuplot -p -e "plot 'diss.txt' using 1:2 with lines"
fi

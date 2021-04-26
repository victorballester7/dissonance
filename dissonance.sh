#!/bin/bash
gcc -Wall -pedantic -o dissonance dissonance.c -lm
./dissonance
gnuplot -p -e "plot 'diss.txt' using 1:2 with lines title 'nostre'"


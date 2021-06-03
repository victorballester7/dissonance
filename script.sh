#!/bin/bash
gcc -Wall -pedantic -o dissonance dissonance.c -lm
./dissonance
returnvalue=$?
if [[ $returnvalue -eq 0 ]]; then
  gnuplot -p -e "plot [1:2] 'diss.txt' with lines"
elif [[ $returnvalue -eq 4 ]]; then
  gnuplot -p -e "plot [60:2000] 'diss.txt' with lines"
fi

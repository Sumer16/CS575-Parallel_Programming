#!/bin/bash

#number of threads:
for t in 1 2 4 8
do
        echo NUMT = $t
        #number of trials:
        for s in 1 10 100 1000 10000 100000 1000000 5000000
        do
            echo NUMTRIALS = $s
            g++ -DNUMT=$t -DNUMTRIALS=$s main.c -o main -lm -fopenmp
            ./main
        done
        echo "=============================================================================="
done
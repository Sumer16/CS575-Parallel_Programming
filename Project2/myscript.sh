#!/bin/bash

#number of threads:
for t in 1 2 4 8
do
        echo NUMT = $t
        #number of nodes:
        for s in 2 5 10 50 100 1000 2000 5000
        do
            echo NUMNODES = $s
            g++ -DNUMT=$t -DNUMNODES=$s main.c -o main -lm -fopenmp
            ./main
        done
        echo "=============================================================================="
done
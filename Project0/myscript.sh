#!/bin/bash

g++ main.c -o main -lm -fopenmp -O3

#for i in {1..10}
#do
        echo "------------------------------------"
        #echo "Number of threads is 1"
        ./main 1

        #echo "Number of threads is 4"
        ./main 4
#done
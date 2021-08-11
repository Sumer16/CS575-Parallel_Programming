#!/bin/bash

#Array size:
    echo "ArraySize(K), NumThds, NonSIMD_Thd, SIMD_Thd"
for t in 1024 4096 16384 30000 65536 100000 262144 1048576 4194304 8388608
#for t in 1024 512000 1049472 2097920 3146368 4194816 5243264 6291712 7340160 8388608
do          
      for s in 1 2 4 8
      do         
            g++ -DARRAYSIZE=$t -DNUMT=$s  main.c -o main -lm -fopenmp
            ./main
      done

done
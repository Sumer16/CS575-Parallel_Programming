#!/bin/bash
#SBATCH -J main
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o main.out
#SBATCH -e main.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=kaowe@oregonstate.edu


#block size:
for t in 8 16 32 64 128 256
do
        #echo BLOCKSIZE = $t
        #number of trials:
        for s in 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 
        do
            #echo NUMTRIALS = $s
            /usr/local/apps/cuda/cuda-10.1/bin/nvcc -DBLOCKSIZE=$t -DNUMTRIALS=$s main.cu -o main
            ./main
        done
        echo "=============================================================================="
done
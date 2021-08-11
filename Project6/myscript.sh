#!/bin/bash
#SBATCH -J main
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o main.out
#SBATCH -e main.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=kaowe@oregonstate.edu


#Number of elements:
echo "MultArray:"
echo "Num_elements,   Local_Size,    Performance"
for t in 1024 4096 8192 32768 131072 524288 1048576 2097152 4194304 8388608 16777216
do
        #echo Number of elements = $t
        #BLOCKSIZE:
        for s in 8 32 128 256 512 1024
        do
            #echo BLOCKSIZE = $s
            g++ -DNUM_ELEMENTS=$t -DLOCAL_SIZE=$s -o Mult Mult.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
            ./Mult
        done
done

echo "MultArrayAdd:"
echo "Num_elements,   Local_Size,    Performance"
for t in 1024 4096 8192 32768 131072 524288 1048576 2097152 4194304 8388608 16777216
do
        #echo Number of elements = $t
        #BLOCKSIZE:
        for s in 8 32 128 256 512 1024
        do
            #echo BLOCKSIZE = $s
            g++ -DNUM_ELEMENTS=$t -DLOCAL_SIZE=$s -o MultAdd MultAdd.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
            ./MultAdd
        done
done

echo "MultArrayReduction"
echo "Num_elements,   Local_Size,    Performance"
for t in 1024 4096 8192 32768 131072 524288 1048576 2097152 4194304 8388608 16777216
do
        #echo Number of elements = $t
        #BLOCKSIZE:
        for s in 8 32 128 256 512 1024
        do
            #echo BLOCKSIZE = $s
            g++ -DNUM_ELEMENTS=$t -DLOCAL_SIZE=$s -o reduction reduction.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
            ./reduction
        done
done
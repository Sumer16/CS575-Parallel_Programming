#!/bin/bash
#SBATCH -J main
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o main.out
#SBATCH -e main.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=kaowe@oregonstate.edu

#module load slurm  (Use this when in DGX server)
module load openmpi/3.1

#Number of processors
        for s in 1 2 4 8 16 32
        do
            mpic++ main.c -o main -lm
            mpiexec -np $s main
        done

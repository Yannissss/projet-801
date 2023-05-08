#!/bin/bash

#SBATCH -J "801 - KOKKOS"

#SBATCH --output=job.out
#SBATCH --error=job.err

#SBATCH -p short

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --gres=gpu:1
#SBATCH --reservation=CHPS
#SBATCH --exclusive

# Modules
source env.sh

# Info
echo "JobID: $SLURM_JOB_ID"
echo "Job start time: `date`"
echo "Running on master node: `hostname`"
echo "Current directory: `pwd`"

for N in 1 10 25 50 100 200 500 1000 2000 5000 10000; do
    ./gauss-seidel.cuda ../img/Amphi.jpg $N
done
#!/bin/bash

export KOKKOS_PATH=/home/cchevalier/kokkos_work/kokkos
export OMP_PROC_BIND=true

module purge
module load gcc/10.2
module load cuda/11.4
module load opencv/4.5.1
module list
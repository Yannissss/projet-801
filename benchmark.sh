#!/bin/bash  
#SBATCH -J "CHPS0801 Project" 

#SBATCH --error=job.%J.err  
#SBATCH --output=job.%J.out

#SBATCH -p instant

#SBATCH --time=00:30:00

module purge
module load opencv/4
module load gcc/10.2
module load llvm/14
module list

echo "Algorithme de Gauss-Seidel"
echo "--------------------------"
echo "Modele     | Nb Taches | temps exec"
echo "--------------------------"
echo -n "Sequentiel |           | "
./gauss-seidel img/Loki.jpg -v
for i in 10 20 50 100
do
    echo -n "Parallele  | $i        | "
    ./gauss-seidel img/Loki.jpg $i -v -p
done

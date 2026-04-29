##!/bin/bash 

# Do not forget to select a proper partition if the default 
# one is no fit for the job! 

#SBATCH --output=out.%j
#SBATCH --error=err.%j
#SBATCH --nodes=2 # number of nodes
#SBATCH --ntasks=12 # number of processor cores 
#SBATCH --tasks-per-node=2 # number of tasks per node 
#SBATCH --reservation=kpp
#SBATCH --exclusive
#SBATCH --time=00:10:00 # walltime

# stop on first error
set -e

srun script.sh

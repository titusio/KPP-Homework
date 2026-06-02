#!/bin/bash

#SBATCH --output=out.%j
#SBATCH --error=err.%j
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=24
#SBATCH --reservation=kpp
#SBATCH --exclusive
#SBATCH --time=00:40:00

set -e

srun bash -l script_thread1.sh

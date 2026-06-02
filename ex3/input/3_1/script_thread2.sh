#!/bin/bash

module load --force linux-debian10-x86_64/binutils/2.46.0-gcc-8.3.0-lshvitw
module load --force linux-debian10-x86_64/gcc/14.3.0-gcc-8.3.0-ibzduwh
gcc --version

make clean
make

for threads in 2 6 12 24; do
    echo "--- NUM_THREADS=$threads ---"
    for i in 1 2 3; do
        echo "Run $i:"
        NUM_THREADS=$threads ./mmul_thread2
    done
done

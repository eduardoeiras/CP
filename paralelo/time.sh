#!/bin/bash

MY=/home/pg<numero>/TP/paralelo

module load papi/5.4.1

export OMP_NUM_THREADS=1

time  $MY/bspar
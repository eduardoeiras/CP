#!/bin/bash

MY=/home/pg<numero>/TP/paralelo

module load papi/5.4.1

export OMP_NUM_THREADS=2

time  $MY/bspar

export OMP_NUM_THREADS=4

time  $MY/bspar

export OMP_NUM_THREADS=8

time  $MY/bspar
#!/bin/bash
#$ -l h_rt=1:00:00
#$ -l mem=1G 
#$ -l killable
#$ -pe single 1
#$ -cwd
#$ -j y 
#$ -N OpenMP_iThreads_Run
#$ -q \*@@elves
#$ -m a
#$ -M mmcguir8@ksu.edu
#$ -t 1:5

/homes/mmcguir8/Homework2/openmpCode $1

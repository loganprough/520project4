#!/bin/sh
if [ -d "output" ]
then
	rm -r output
fi
mkdir output
cd output
for i in 1 2 4 8 16
do
	qsub ../openmp_iThreads_shell.sh $i
	qsub ../thread_iThreads_shell.sh $i
	qsub -pe single $i ../openmp_iCores_shell.sh $i
	qsub -pe single $i ../thread_iCores_shell.sh $i
done

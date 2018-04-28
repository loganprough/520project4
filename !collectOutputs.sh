#!/bin/bash
grep -h "DATA" output/OpenMP_*_Run.o* > OpenMP_RunRunOutput.csv
grep -h "DATA" output/Thread_*_Run.o* > Thread_RunOutput.csv
grep -h "code on host" output/*_Run.o* > ListHost.txt
##echo "done"

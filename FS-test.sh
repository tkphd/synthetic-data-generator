#!/usr/bin/env bash

# Sweep over dataset and file sizes to test BeeGFS scaling:
# test datasets in the range of 10 MB to 1 GB
# with mean filesize in the range of 10 KB to 10 MB

# Path variables
DIR=~/research/projects/BeeGFS/synthetic-dataset-generator
BIN=${DIR}/./data-generator     # absolute path to binary file
TYM=$(date +%s)                 # UNIX epoch timestamp
LOG="${DIR}/${TYM}_FS-test.log" # absolute path to log file

# CLI options
DM=1000000                      # data-size multiplier
FM=1000                         # file-size multiplier
RP=3                            # number of repetitions

# Real Values
DSA=( 10 20 50 100 200 500 1000 2000 )
FSA=( 10 20 50 100 200 500 1000 2000 5000 10000 50000 )

# Total number of jobs
DSN=${#DSA[@]}
FSN=${#FSA[@]}
NJ=$(( DSN*FSN ))

# Write log file column headers
echo "Writing summary to ${LOG}"
echo -ne "data         file            " > "${LOG}"
for ((i=0; i<RP; i++))
do
    echo -ne "time         " >> "${LOG}"
done
echo -e "write        stdev        remove" >> "${LOG}"

n=1
for j in "${DSA[@]}"
do
    datasize=$(( DM*j ))
    for i in "${FSA[@]}"
    do
        filesize=$(( FM*i ))
        printf "%2d/${NJ} " ${n}
        /usr/bin/time -f'     (%E wall, %U user, %S sys)' \
                      ${BIN} -t ${datasize} -m ${filesize} -r ${RP} >> "${LOG}"
        n=$(( n+1 ))
    done
done
echo "Wrote summary to ${LOG}"

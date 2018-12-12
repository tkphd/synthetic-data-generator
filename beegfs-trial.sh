#!/usr/bin/env bash

# Sweep over dataset and file sizes to test BeeGFS scaling:
# test datasets in the range of 100MB to 50GB
# with mean filesize in the range of 100KB to 1MB

# Path variables
TYM=$(date +%s)                       # UNIX epoch timestamp
DIR=/home/thor/research/projects/BeeGFS/beegfs-v7-conf/data-generator
BIN=${DIR}/./data-generator           # absolute path to binary file
LOG="${DIR}/${TYM}_beegfs-trial.log"  # absolute path to log file

# CLI options
DM=1048576                            # data-size multiplier (Should be 1000000)
FM=1024                               # file-size multiplier
RP=3                                  # repetitions

## Testing Values
#DSA=( 100 200 500 1000 )
#FSA=( 100 200 )

# Real Values
DSA=( 1 2 5 10 20 50 100 200 500 1000 2000 5000 10000 )
FSA=( 50 100 200 )

# Total number of jobs
DSN=${#DSA[@]}
FSN=${#FSA[@]}
NJ=$(( ${DSN}*${FSN} ))

# Write log file column headers
echo "Writing summary to ${LOG}"
echo -ne "data         file      " > ${LOG}
for ((i=0; i<RP; i++))
do
    echo -ne "time     " >> ${LOG}
done
echo -e "avg      std" >> ${LOG}

n=1
for j in "${DSA[@]}"
do
    datasize=$(( ${DM}*${j} ))
    for i in "${FSA[@]}"
    do
        filesize=$(( ${FM}*${i} ))
        #printf "%'15d\t%'12d\n" ${datasize} ${filesize} >> ${LOG}
        printf "%2d/${NJ}" ${n}
        /usr/bin/time -f'      (%E wall, %U user, %S sys)' \
                      ${BIN} ${datasize} ${filesize} ${RP} >> ${LOG}
        n=$(( ${n}+1 ))
    done
done
echo "Wrote summary to ${LOG}"

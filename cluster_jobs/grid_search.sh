#!/usr/bin/env bash
#SBATCH -N 1
#SBATCH -n 16
#SBATCH -t 1:00:00
#SBATCH -J grid_search

# Stop execution after any error
set -e

# Useful variables
MYUSER=$(whoami)
LOCALDIR=/tmp
DATADIR=~/data
PROJDIR=$PWD

# or another folder where you want your data to be stored
# Job-related variables
THISJOB=grid_search

# some way to identify this job execution
WORKDIR=$LOCALDIR/$MYUSER/$THISJOB

# Create work dir from scratch, enter it
rm -rf $WORKDIR && mkdir -p $WORKDIR && cd $WORKDIR

# setup the directories and files we expect
cp -r $PROJDIR/bin .
cp -r $PROJDIR/build .
cp -r $PROJDIR/controllers .
cp -r $PROJDIR/loop_functions .
cp -r $PROJDIR/experiments .
cp $PROJDIR/CMakeLists.txt .
cp $PROJDIR/bin/grid_search.py .

# Execute program (this also writes files in work dir)
echo $WORKDIR
# this will evaluate 16(configs)*4(trials) = 64 total trials per parameter
# each trial takes .83 seconds, so each parameter takes 53 seconds
# so we can do roughly 65 parameters per hour
./grid_search.py --resolution=7 --pool-size=16 --trials 4 --skip=0 --stop-at=-1 experiments/1_class/*.argos experiments/2_class/*.argos experiments/4_class/*.argos build/loop_functions/libn_class.so

# Transfer generated files into home directory
cp grid_search_output* $DATADIR

# Cleanup
rm -r $WORKDIR

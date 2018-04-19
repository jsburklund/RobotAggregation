#!/usr/bin/env bash
#SBATCH -J jobname

# Stop execution after any error
set -e

# Useful variables
MYUSER=$(whoami)
LOCALDIR=/tmp
DATADIR=~/data
PROJDIR=$PWD

# or another folder where you want your data to be stored
# Job-related variables
THISJOB=grid_search_$1_$2

# some way to identify this job execution
WORKDIR=$LOCALDIR/$MYUSER/$THISJOB

# Create work dir from scratch, enter it
rm -rf $WORKDIR && mkdir -p $WORKDIR && cd $WORKDIR

# setup the directories and files we expect
mkdir -p build/bin
cp -r $PROJDIR/build .
cp -r $PROJDIR/experiments .
cp $PROJDIR/bin/grid_search.py .

# Execute program (this also writes files in work dir)
echo $WORKDIR
echo "STARTING AT: "
echo $1
echo "STOPPING AT: "
echo $2
./grid_search.py experiments/2_class/*.argos -n 10 -t 10 -v --skip=$1 --stop-at=$2

# Transfer generated files into home directory
cp -a grid_search_output* $DATADIR

# Cleanup
# rm -rf $WORKDIR

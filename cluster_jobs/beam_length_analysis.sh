#!/usr/bin/env bash
#SBATCH -N 1
#SBATCH -n 9
#SBATCH -t 12:00:00
#SBATCH -J beam_length

# Stop execution after any error
set -e

# Useful variables
MYUSER=$(whoami)
LOCALDIR=/tmp
DATADIR=~/data
PROJDIR=$PWD

# or another folder where you want your data to be stored
# Job-related variables
THISJOB=n_classes

# some way to identify this job execution
WORKDIR=$LOCALDIR/$MYUSER/$THISJOB

# Create work dir from scratch, enter it
rm -rf $WORKDIR && mkdir -p $WORKDIR && cd $WORKDIR

# setup the directories and files we expect
cp -r $PROJDIR/venv .
cp -r $PROJDIR/bin .
cp -r $PROJDIR/python .
cp -r $PROJDIR/build .
cp -r $PROJDIR/controllers .
cp -r $PROJDIR/loop_functions .
cp -r $PROJDIR/experiments .
cp -r $PROJDIR/params .
cp $PROJDIR/CMakeLists.txt .

# Execute program (this also writes files in work dir)
echo $WORKDIR
./python/analyze_beam_length.py evaluate experiments/beam_length/*.argos build/loop_functions/libsegregation_loop_function.so params/new_grid_search.dat -p 9 -t 100

# Transfer generated files into home directory
cp beam_length_* $DATADIR/beam_length

# Cleanup
rm -r $WORKDIR

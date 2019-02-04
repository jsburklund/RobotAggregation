#!/usr/bin/env bash
#SBATCH -N 1
#SBATCH -n 35
#SBATCH -t 12:00:00
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
cp -r $PROJDIR/venv .
cp -r $PROJDIR/bin .
cp -r $PROJDIR/python .
cp -r $PROJDIR/build .
cp -r $PROJDIR/controllers .
cp -r $PROJDIR/loop_functions .
cp -r $PROJDIR/experiments .
cp $PROJDIR/CMakeLists.txt .
cp $PROJDIR/python/grid_search.py .

# Execute program (this also writes files in work dir)
echo $WORKDIR
source venv/bin/activate
./grid_search.py --resolution=7 --pool-size=35 --trials 1 --skip=0 --stop-at=10000 experiments/1_class/*.argos experiments/2_class/*.argos experiments/4_class/*.argos build/loop_functions/libsegregation_loop_function.so

# Transfer generated files into home directory
cp grid_search_output* $DATADIR/centroid_of_centroids_grid_search/

# Cleanup
rm -r $WORKDIR

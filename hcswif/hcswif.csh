#!/usr/bin/csh

if ( $#argv != 3 ) then
    echo Usage: hcswif.sh SCRIPT RUN EVENTS
    exit 1
endif
set script=$1
set run=$2
set evt=$3

# Setup environment
set hcswif_script=`readlink -f "$0"`
set hcswif_dir=`dirname "$hcswif_script"`
source $hcswif_dir/setup.csh

# Check environment
# Not sure how best to do this. How do we make sure hcana is in the path?
#if [ -z "$(which hcana)" ]; then
#    echo Environment not set up! Please edit $hcswif_dir/setup.sh appropriately
#    exit 1
#fi

# Replay the run
cd $hallc_replay_dir
echo pwd: $PWD
set runHcana=`./hcana -b -q .x $script"($run,$evt)"`

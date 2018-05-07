#!/usr/bin/csh
if ($#argv != 3) then
    echo Usage: hcswif.csh SCRIPT RUN EVENTS
    exit 1
endif
set script=$1
set run=$2
set evt=$3

# Setup environment
set hcswif_dir=`dirname $0`
set hcswif_dir=`cd $hcswif_dir && pwd`
source $hcswif_dir/setup.csh

# Check environment
if ( ! { command -v hcana } ) then
    echo Could not find hcana! Please edit $hcswif_dir/setup.csh appropriately
    exit 1
endif

# Replay the run
set runHcana="./hcana "\""$script($run,$evt)"\"
cd $hallc_replay_dir
echo pwd: `pwd`
echo $runHcana
eval $runHcana

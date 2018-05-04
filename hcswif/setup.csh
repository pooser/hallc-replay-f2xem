#!/usr/bin/csh

set hcana_dir=/w/hallc-scifs17exp/xem2/$USER/hcana
set hallc_replay_dir=/w/hallc-scifs17exp/xem2/$USER/hallc-replay-f2xem

source /site/12gev_phys/production.sh 2.1

set curdir=$PWD

cd $hcana_dir
source setup.csh
set PATH=$hcana_dir/bin:$PATH
echo Sourced $hcana_dir/setup.csh

cd $hallc_replay_dir
source setup.csh
echo Sourced $hallc_replay_dir/setup.csh

echo cd back to $curdir
cd $curdir

#!/bin/bash

list=$1

while read line
do

fname=ROOTfiles/hms-f2-xem-scalers/hms_replay_scalers_${line}_-1.root

echo Processing ${fname}

root -l -b<<EOF
.x run.C("${fname}", "H")
EOF

[ ! -d "hms" ] && `mkdir hms`

mv bcmcurrent_${line}.param ./hms/

echo Created bcmcurrent_${line}.param

done < ${list}

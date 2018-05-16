#!/bin/bash

list=$1

while read line
do

fname=ROOTfiles/shms-f2-xem-scalers/shms_replay_scalers_${line}_-1.root

echo Processing ${fname}

root -l -b<<EOF
.x run.C("${fname}", "P")
EOF

[ ! -d "shms" ] && `mkdir shms`

mv bcmcurrent_${line}.param ./shms/

echo Created bcmcurrent_${line}.param

done < ${list}

#!/bin/bash

xlist=$(seq 68.5 68.5)
#ylist="$(seq 900 50 1200) $(seq 1200 200 3600) $(seq 3600 25 4400)" 
ylist=$(seq 36 -2 26)
# ylist=$(seq 2500 2500)
zlist=$(seq 0 0)

for x in $xlist; do
for y in $ylist; do
for z in $zlist; do

echo "$x $y $z"

done; done; done

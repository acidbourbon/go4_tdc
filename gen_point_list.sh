#!/bin/bash

xlist=$(seq 4100 4100)
#ylist="$(seq 900 50 1200) $(seq 1200 200 3600) $(seq 3600 25 4400)" 
ylist="3300 3400 3500 $(seq 3600 25 4000)" 
# ylist=$(seq 2500 2500)
zlist=$(seq 3100 3100)

for x in $xlist; do
for y in $ylist; do
for z in $zlist; do

echo "$x $y $z"

done; done; done

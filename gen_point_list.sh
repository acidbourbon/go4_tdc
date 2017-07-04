#!/bin/bash

xlist=$(seq 4100 4100)
ylist="$(seq 4600 100 8000)"
zlist=$(seq 4400 4400)

for x in $xlist; do
for y in $ylist; do
for z in $zlist; do

echo "$x $y $z"

done; done; done

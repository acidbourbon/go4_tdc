#!/bin/bash

xlist=$(seq 4750 4750)
ylist="$(seq 2300 400 5000)"
zlist=$(seq 4500 4500)

for x in $xlist; do
for y in $ylist; do
for z in $zlist; do

echo "$x $y $z"

done; done; done

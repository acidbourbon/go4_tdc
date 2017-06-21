#!/bin/bash
root -l -q 'fit_t1_offsets.C("'$1'")' $2
cat t1_offsets.txt
cp t1_offsets.txt $(dirname $1)/
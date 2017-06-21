#!/bin/bash


fname=$1
i=$2 # number of triggers to plot

dir=$(dirname $fname)

rm t1_offsets.txt

{
  . trb3login_x230

  rm Go4AutoSave.root
  go4analysis -user $fname -num $i
  mv Go4AutoSave.root $fname.root
  cp second.C $dir/second.C_bak
}

./fit_t1_offsets.sh $fname.root
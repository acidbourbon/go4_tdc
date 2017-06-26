#!/bin/bash


fname=$1
i=$2 # number of triggers to plot

dir=$(dirname $fname)


{
  . trb3login_x230

  rm Go4AutoSave.root
  go4analysis -user $fname -num $i
  mv Go4AutoSave.root $fname.root
  cp second.C $dir/second.C_bak
}


#!/bin/bash


#fname=$1
i=$2 # number of triggers to plot

#dir=$(dirname $fname)
dir=$1


{
  #. trb3login_x230
  . my_trb3login

  rm Go4AutoSave.root
  go4analysis -user "$dir/*.hld" -num $i
  mv Go4AutoSave.root $dir/go4_out.root
  mv tree_out.root $dir/
  cp second.C $dir/second.C_bak
}


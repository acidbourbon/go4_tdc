#!/bin/bash

# usage: ./plot_intensity.sh <datafile.txt>

datadir=$1

cat <<EOF | gnuplot
set timefmt "%Y-%m-%d_%H:%M:%S"
set xdata time
set format x "%m-%d %H:%M"
#set datafile separator " "
# set yrange [0:1500000]
set key top left
plot\
  "<(paste $datadir/point_list.txt $datadir/triggers.txt)" using 1:2 w p title "triggers per acquisition"

pause 1000

EOF
#!/bin/bash

# usage: ./plot_intensity.sh <datafile.txt>

datafile=$1

cat <<EOF | gnuplot
set timefmt "%Y-%m-%d_%H:%M:%S"
set xdata time
set format x "%m-%d %H:%M"
#set datafile separator " "
set yrange [0:1500000]
set key top left
plot\
  "$datafile" using 1:2 w p title "PASTTREC ch 1"\
, "$datafile" using 1:3 w p title "PASTTREC ch 2"\
, "$datafile" using 1:4 w p title "PASTTREC ch 3"\
, "$datafile" using 1:5 w p title "PASTTREC ch 4"\
, "$datafile" using 1:6 w p title "PASTTREC ch 5"\
, "$datafile" using 1:7 w p title "PASTTREC ch 6"\
, "$datafile" using 1:8 w p title "PASTTREC ch 7"\
, "$datafile" using 1:9 w p title "PASTTREC ch 8"\

pause 1000

EOF
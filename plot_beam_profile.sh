#!/bin/bash

dir=$1

cat <<EOF | gnuplot
plot "<(paste $dir/point_list.txt $dir/triggers.txt)" using 2:4 w lp

pause 1000

EOF


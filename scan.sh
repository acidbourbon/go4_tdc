#!/bin/bash

scan_name=$1
point_list="./point_list.txt"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

mkdir -p $data_dir/$scan_name
cp $point_list $data_dir/$scan_name/

while read line <&3; do
#     echo $line
    x=$( echo $line | cut -d " " -f 1);
    y=$( echo $line | cut -d " " -f 2);
    z=$( echo $line | cut -d " " -f 3);
    echo "x $x y $y z $z"
    
    acq_name="x${x}_y${y}_z${z}"
    ./move_laser.sh $x $y $z
    ./acquisition.sh $scan_name $acq_name
done 3<$point_list
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
    #curl http://localhost:1148/commands/put.pl?0353-d400-309373-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1
    
    #./set_asd8_thresh.sh 37747
    #./set_asd8_thresh.sh 30000
    ./acquisition.sh $scan_name $acq_name
done 3<$point_list
cp $0 $data_dir/$scan_name/copy_of_scan.sh
cp acquisition.sh $data_dir/$scan_name/copy_of_acquisition.sh
echo "scan finished"
./go4_on_all.sh $data_dir/$scan_name/
echo "pre-analysis finished"
./mail.pl m.wiebusch@gsi.de "scan $scan_name finished"
./mail.pl c.wendisch@gsi.de "scan $scan_name finished"
./next_scan.sh


#!/bin/bash

scan_name=$1
point_list="./point_list.txt"
. set_env
#data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

export DAQOPSERVER=localhost:148

mkdir -p $data_dir/$scan_name
cp $point_list $data_dir/$scan_name/


date '+%Y-%m-%d_%H:%M:%S' > $data_dir/$scan_name/ts_scan_start.txt

while read line <&3; do
#     echo $line
    x=$( echo $line | cut -d " " -f 1);
    y=$( echo $line | cut -d " " -f 2);
    z=$( echo $line | cut -d " " -f 3);
    echo "x $x y $y z $z"
    
    acq_name="x${x}_y${y}_z${z}"
    ./move_laser.sh $x $y $z
    #curl http://localhost:1148/commands/put.pl?1483-d400-309373-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1
    
    #./set_asd8_thresh.sh 37747
    if [ -z $pktime ]; then
    ./init_asics
    else
	    echo $(./init_asics_pt$pktime)
	    echo "set pktime $pktime"
    fi
    if [ -z $thr ]; then
      ./threshold_all 10 
    else
	    ./threshold_all $thr
	    echo "set threshold $thr"
    fi
    ./acquisition.sh $scan_name $acq_name
done 3<$point_list
date '+%Y-%m-%d_%H:%M:%S' > $data_dir/$scan_name/ts_scan_stop.txt
cp $0 $data_dir/$scan_name/copy_of_scan.sh
cp acquisition.sh $data_dir/$scan_name/copy_of_acquisition.sh
echo "scan finished"
./go4_on_all.sh $data_dir/$scan_name/
echo "pre-analysis finished"

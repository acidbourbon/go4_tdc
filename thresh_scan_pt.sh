#!/bin/bash

scan_name=$1
thr_list="./thr_list.txt"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

mkdir -p $data_dir/$scan_name
cp $thr_list $data_dir/$scan_name/
export DAQOPSERVER=localhost:148

#    /home/hadaq/mnt/mount_rossendorf_share.sh
while read line <&3; do
    echo "setting thr: $line"
    
    #thresh=$(printf "%04x" $line)
    #curl http://localhost:1148/commands/put.pl?1483-d400-30${thresh}-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1
    #./set_asd8_thresh.sh $line
    if [ -z $pktime ]; then
    ./init_asics
    else
	    echo $(./init_asics_pt$pktime)
	    echo "set pktime $pktime"
    fi
    ./threshold_all $line 
    ./acquisition.sh $scan_name thr_$line
done 3<$thr_list

./go4_on_all.sh $data_dir/$scan_name/

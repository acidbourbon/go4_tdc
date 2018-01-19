#!/bin/bash

scan_name=$1
vlist="./vlist.txt"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

export DAQOPSERVER=localhost:35

mkdir -p $data_dir/$scan_name
cp $vlist $data_dir/$scan_name/


date '+%Y-%m-%d_%H:%M:%S' > $data_dir/$scan_name/ts_scan_start.txt

while read line <&3; do
#     echo $line
    v=$( echo $line | cut -d " " -f 1);
#     y=$( echo $line | cut -d " " -f 2);
#     z=$( echo $line | cut -d " " -f 3);
    echo "Voltage: $v"
    
    acq_name="v${v}"
    
    echo "setting signal generator to $v V"
    ./keysight_cmd.sh ":VOLT1 $v"; sleep 7
    echo "starting TDC acquisition"
    
    
    
    #./set_asd8_thresh.sh 37747
    if [ -z $pktime ]; then
    ./init_asics
    else
	    echo $(./init_asics_pt$pktime)
	    echo "set pktime $pktime"
    fi
    ./set_pt_amp_pkt.sh ## this gets gain and pktime from environment variables
    if [ -z $thr ]; then
      ./threshold_all 10 
    else
	    ./threshold_all $thr
	    echo "set threshold $thr"
    fi
    ./acquisition.sh $scan_name $acq_name
done 3<$vlist
date '+%Y-%m-%d_%H:%M:%S' > $data_dir/$scan_name/ts_scan_stop.txt
cp $0 $data_dir/$scan_name/copy_of_scan.sh
cp acquisition.sh $data_dir/$scan_name/copy_of_acquisition.sh
echo "scan finished"
# ./go4_on_all.sh $data_dir/$scan_name/
# echo "pre-analysis finished"

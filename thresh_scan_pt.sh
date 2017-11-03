#!/bin/bash

scan_name=$1
thr_list="./thr_list_pt.txt"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

mkdir -p $data_dir/$scan_name
cp $thr_list $data_dir/$scan_name/
export DAQOPSERVER=localhost:35

#    /home/hadaq/mnt/mount_rossendorf_share.sh
while read line <&3; do
    echo "setting thr: $line"
    
    #thresh=$(printf "%04x" $line)
    #curl http://localhost:1148/commands/put.pl?0353-d400-30${thresh}-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1
    #./set_asd8_thresh.sh $line
    if [ -z $pktime ]; then
    ./init_asics
    else
	    echo $(./init_asics_pt$pktime)
	    echo "set pktime $pktime"
    fi
    ./set_pt_amp_pkt.sh ## this gets gain and pktime from environment variables

    ./threshold_all $line 
    ./acquisition.sh $scan_name gain_${gain}_thr_${line} &
    ./vxi/go.sh; killall dabc_exe; wait

done 3<$thr_list
#./mail.pl m.wiebusch@gsi.de "scan $scan_name finished"
#./mail.pl c.wendisch@gsi.de "scan $scan_name finished"

#./go4_on_all.sh $data_dir/$scan_name/

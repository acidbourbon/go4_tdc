#!/bin/bash

scan_name=$1
thr_list="./thr_list.txt"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

mkdir -p $data_dir/$scan_name
cp $thr_list $data_dir/$scan_name/

#    /home/hadaq/mnt/mount_rossendorf_share.sh
while read line <&3; do
    echo "setting thr: $line"
    
    #thresh=$(printf "%04x" $line)
    #curl http://localhost:1148/commands/put.pl?0353-d400-30${thresh}-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1
    #./set_asd8_thresh.sh $line
    echo $line > current_asd8_thresh.txt
    sleep 2
    acq_name=$(printf "thr_%05d" $line)
    ./acquisition.sh $scan_name $acq_name #&
    #./vxi/go.sh; sleep 5; killall dabc_exe; wait
done 3<$thr_list

#./go4_on_all.sh $data_dir/$scan_name/

#./mail.pl m.wiebusch@gsi.de "scan $scan_name finished"
#./mail.pl c.wendisch@gsi.de "scan $scan_name finished"
./next_scan.sh

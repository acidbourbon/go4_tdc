#!/bin/bash

export DAQOPSERVER=localhost:35

scan_name=$1
point_list="./point_list.txt"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"

mkdir -p $data_dir/$scan_name
cp $point_list $data_dir/$scan_name/

#./linear_drive/reset.sh


while read line <&3; do
#     echo $line
    x=$( echo $line | cut -d " " -f 1);
    y=$( echo $line | cut -d " " -f 2);
    echo "pos: x $x y $y"
    
    acq_name="x${x}_y${y}"
    ./linear_drive/movex $x
    ./linear_drive/movey $y
    sleep 5
    ./wait_for_spill.sh
    oa=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " ))  ]
    ./acquisition.sh $scan_name $acq_name
    #sleep 25
    a=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " )) - $oa ]
    echo "recorded $a triggers"
    echo $a >> $data_dir/$scan_name/triggers.txt
done 3<$point_list
cp $0 $data_dir/$scan_name/copy_of_scan.sh
cp acquisition.sh $data_dir/$scan_name/copy_of_acquisition.sh
cp second.C $data_dir/$scan_name/copy_of_second.C
echo "scan finished"
#  ./go4_on_all.sh $data_dir/$scan_name/
echo "pre-analysis finished"
# ./mail.pl m.wiebusch@gsi.de "scan $scan_name finished"
# ./mail.pl c.wendisch@gsi.de "scan $scan_name finished"
./next_scan.sh


#!/bin/bash

postfix=$1


./vxi/vxbatch.sh 192.168.100.48 trigger_diamond_edge.txt

# asd8 xscan

echo "timeout=30" > acq_info.txt
echo "25000" > current_asd8_thresh.txt
cp point_list_asd8_ch7_xscan.txt point_list.txt
./juelich_scan.sh $(./run_counter.sh)_xscan_ASD8_$postfix

# pt xscan

#echo "timeout=140" > acq_info.txt
#./favorite_pt_setting.sh
#cp point_list_pasttrec_ch7_xscan.txt point_list.txt
#./juelich_scan.sh $(./run_counter.sh)_xscan_PT_fav_$postfix


#./vxi/vxbatch.sh 192.168.100.48 trigger_diamond_edge.txt
#echo "timeout=400" > acq_info.txt
#./acquisition.sh $(./run_counter.sh)_PT_fav_diamond_edge_$postfix
#
#./vxi/vxbatch.sh 192.168.100.48 trigger_full_diamond.txt
#echo "timeout=160" > acq_info.txt
#./acquisition.sh $(./run_counter.sh)_PT_fav_diamond_full_$postfix
#


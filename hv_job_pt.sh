#!/bin/bash

./linear_drive/movex 62.95
sleep 4

./vxi/vxbatch.sh 192.168.100.48 trigger_diamond_edge.txt
echo "timeout=400" > acq_info.txt
./acquisition.sh $(./run_counter.sh)_ASD8_diamond_edge

./vxi/vxbatch.sh 192.168.100.48 trigger_full_diamond.txt
echo "timeout=160" > acq_info.txt
./acquisition.sh $(./run_counter.sh)_ASD8_diamond_full



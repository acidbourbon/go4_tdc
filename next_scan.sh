#!/bin/bash

# if you want me to be executed, don't forget to chmod +x me!

chmod -x $0 # disable this script, so it won't run again
#echo "there is no additional job"
mv point_list_quarter_40deg_extra.txt point_list.txt
./scan.sh yz_vol_scan_ASD8_61

#!/bin/bash

# if you want me to be executed, don't forget to chmod +x me!

chmod -x $0 # disable this script, so it won't run again
#echo "there is no additional job"
#mv point_list_89.txt point_list.txt
export DAQOPSERVER=localhost:35
./init_asics_pt20
cp point_list_pasttrec.txt point_list.txt
./juelich_scan.sh 011_x_scan_pt20


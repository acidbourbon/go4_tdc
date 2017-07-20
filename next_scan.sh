#!/bin/bash

# if you want me to be executed, don't forget to chmod +x me!

chmod -x $0 # disable this script, so it won't run again
#echo "there is no additional job"
mv point_list_89.txt point_list.txt
./scan.sh y_scan_ASD8_89


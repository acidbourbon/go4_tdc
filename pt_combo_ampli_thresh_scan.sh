#!/bin/bash

# does a thresh scan for all three peaking times of PASTTREC in a row


scan_name=$1
./reset_pt
sleep 1

gain=1 pktime=10 ./thresh_scan_pt.sh ${scan_name}_pkt10
gain=1 pktime=15 ./thresh_scan_pt.sh ${scan_name}_pkt15
gain=1 pktime=20 ./thresh_scan_pt.sh ${scan_name}_pkt20

gain=2 pktime=10 ./thresh_scan_pt.sh ${scan_name}_pkt10
gain=2 pktime=15 ./thresh_scan_pt.sh ${scan_name}_pkt15
gain=2 pktime=20 ./thresh_scan_pt.sh ${scan_name}_pkt20

gain=4 pktime=10 ./thresh_scan_pt.sh ${scan_name}_pkt10
gain=4 pktime=15 ./thresh_scan_pt.sh ${scan_name}_pkt15
gain=4 pktime=20 ./thresh_scan_pt.sh ${scan_name}_pkt20

echo "done!"

./next_scan.sh

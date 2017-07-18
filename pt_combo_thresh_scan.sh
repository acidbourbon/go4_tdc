#!/bin/bash

# does a thresh scan for all three peaking times of PASTTREC in a row


scan_name=$1
./reset_pt
sleep 1

pktime=20 ./thresh_scan_pt.sh ${scan_name}_pkt20
pktime=15 ./thresh_scan_pt.sh ${scan_name}_pkt15
pktime=10 ./thresh_scan_pt.sh ${scan_name}_pkt10
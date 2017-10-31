#!/bin/bash

# if you want me to be executed, don't forget to chmod +x me!

chmod -x $0 # disable this script, so it won't run again
export DAQOPSERVER=localhost:35


./linear_drive/movex 70.45
sleep 10
./pt_combo_ampli_thresh_scan.sh 026_param_scan_pt_edge_ho

#should be reached by 7 AM




sleep 10

# scan analog waveforms with rohde + schwarz

./vxi/vxi11_cmd 192.168.100.48 "TIMebase:SCALe 75E-9" ## correct timescale for PASTTREC waveform acquisition
sleep 10
mv delme_thresh_scan_pt.sh thresh_scan_pt.sh ## this changes the script to Scope DAQ

./pt_combo_ampli_thresh_scan.sh 027_param_scan_pt_edge_ho_scope
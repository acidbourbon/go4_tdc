#!/bin/bash

# if you want me to be executed, don't forget to chmod +x me!

chmod -x $0 # disable this script, so it won't run again
export DAQOPSERVER=localhost:35


#./linear_drive/movex 70.45
#sleep 10
#./pt_combo_ampli_thresh_scan.sh 026_param_scan_pt_edge_ho

#should be reached by 7 AM


cp point_list_pasttrec_ch7_xscan.txt point_list.txt

./init_asics_pt15 ## shaper settings
pktime=15 gain=4 ./set_pt_amp_pkt.sh; ## gain and peaking time must be changed together
./threshold_all 10 ## threshold

./juelich_scan.sh 036_xscan_pasttrec_gain4_pkt15_thr10_140s


./init_asics_pt20 ## shaper settings
pktime=20 gain=4 ./set_pt_amp_pkt.sh; ## gain and peaking time must be changed together
./threshold_all 7 ## threshold

./juelich_scan.sh 037_xscan_pasttrec_gain4_pkt20_thr07_140s



./init_asics_pt20 ## shaper settings
pktime=20 gain=2 ./set_pt_amp_pkt.sh; ## gain and peaking time must be changed together
./threshold_all 7 ## threshold

./juelich_scan.sh 038_xscan_pasttrec_gain2_pkt20_thr07_140s


#./vxi/vxi11_cmd 192.168.100.48 "TIMebase:SCALe 75E-9" ## correct timescale for PASTTREC waveform acquisition

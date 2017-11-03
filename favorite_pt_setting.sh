#!/bin/bash

export DAQOPSERVER=localhost:35

./init_asics_pt15 ## shaper settings
pktime=15 gain=4 ./set_pt_amp_pkt.sh; ## gain and peaking time must be changed together
./threshold_all 10 ## threshold


#!/bin/bash

export DAQOPSERVER=localhost:35

./init_asics_pt20 ## shaper settings
pktime=20 gain=1 ./set_pt_amp_pkt.sh; ## gain and peaking time must be changed together
./threshold_all 3 ## threshold


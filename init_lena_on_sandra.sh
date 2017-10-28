#!/bin/bash

trbcmd reload 0x0351
trbcmd reload 0x0353
sleep 4
./addresses.sh
# set trigger to cts
trbcmd w 0x0351 0xcf00 0x300
trbcmd w 0x0353 0xcf00 0x300
# set data limit
trbcmd w 0x0351 0xc804 100
trbcmd w 0x0353 0xc804 100
# set trigger windows -300 +100 ns (cosmic setup)
trbcmd w 0x0351 0xc801 0x80140050
trbcmd w 0x0353 0xc801 0x80140050


# enable relevant TDC channels
trbcmd w 0x0351 0xc802 0xbf03ff
trbcmd w 0x0353 0xc802 0xff03ff

./init_asics_pt10
./threshold_all 10

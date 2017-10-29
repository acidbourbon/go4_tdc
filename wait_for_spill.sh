#!/bin/bash

delay=1
thresh_low=1
thresh_high=1

while true; do
	oa=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " ))  ]
	sleep $delay
	a=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " )) - $oa ]
# 	echo "triggers / $delay s: $a"
	if [ $a -lt $thresh_low ]; then
    echo " --- end of spill --- "
    break
  fi
done


while true; do
	oa=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " ))  ]
	sleep $delay
	a=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " )) - $oa ]
# 	echo "triggers / $delay s: $a"
	if [ $a -gt $thresh_high ]; then
    echo " --- beginning of new spill --- "
    break
  fi
done

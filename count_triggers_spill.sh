#!/bin/bash
export DAQOPSERVER=localhost:35

./wait_for_spill.sh
	oa=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " ))  ]
	sleep 25
	a=$[ $(printf "%d" $(trbcmd r 0x0353 0xc000 | cut -f 3 -d " " )) - $oa ]
	echo "counts in this spill: $a"

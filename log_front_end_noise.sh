#!/bin/bash
outfile="data/noise_log/"$(date '+%Y-%m-%d_%H:%M:%S')".txt"
while true; do
	oa=$[ $(printf "%d" $(trbcmd r 0x0353 0xc001 | cut -f 3 -d " " ))  ]
	ob=$[ $(printf "%d" $(trbcmd r 0x0353 0xc002 | cut -f 3 -d " " ))  ]
	oc=$[ $(printf "%d" $(trbcmd r 0x0353 0xc003 | cut -f 3 -d " " ))  ]
	od=$[ $(printf "%d" $(trbcmd r 0x0353 0xc004 | cut -f 3 -d " " ))  ]
	oe=$[ $(printf "%d" $(trbcmd r 0x0353 0xc005 | cut -f 3 -d " " ))  ]
	of=$[ $(printf "%d" $(trbcmd r 0x0353 0xc006 | cut -f 3 -d " " ))  ]
	og=$[ $(printf "%d" $(trbcmd r 0x0353 0xc007 | cut -f 3 -d " " ))  ]
	oh=$[ $(printf "%d" $(trbcmd r 0x0353 0xc008 | cut -f 3 -d " " ))  ]
	sleep 1
	a=$[ $(printf "%d" $(trbcmd r 0x0353 0xc001 | cut -f 3 -d " " )) - $oa ]
	b=$[ $(printf "%d" $(trbcmd r 0x0353 0xc002 | cut -f 3 -d " " )) - $ob ]
	c=$[ $(printf "%d" $(trbcmd r 0x0353 0xc003 | cut -f 3 -d " " )) - $oc ]
	d=$[ $(printf "%d" $(trbcmd r 0x0353 0xc004 | cut -f 3 -d " " )) - $od ]
	e=$[ $(printf "%d" $(trbcmd r 0x0353 0xc005 | cut -f 3 -d " " )) - $oe ]
	f=$[ $(printf "%d" $(trbcmd r 0x0353 0xc006 | cut -f 3 -d " " )) - $of ]
	g=$[ $(printf "%d" $(trbcmd r 0x0353 0xc007 | cut -f 3 -d " " )) - $og ]
	h=$[ $(printf "%d" $(trbcmd r 0x0353 0xc008 | cut -f 3 -d " " )) - $oh ]
	echo $(date '+%Y-%m-%d_%H:%M:%S')	$a	$b	$c	$d	$e	$f	$g	$h| tee -a $outfile
	sleep 9
	#export oa=$a
	#export ob=$b
	#export oc=$c
	#export od=$d
	#echo $oa $ob $oc $od
done


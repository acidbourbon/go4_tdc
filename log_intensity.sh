#!/bin/bash
outfile="data/intensity_log/"$(date '+%Y-%m-%d_%H:%M:%S')".txt"
while true; do
	echo $(date '+%Y-%m-%d_%H:%M:%S') $(cat lasertest/intensity.config) | tee -a $outfile
	sleep 2
done

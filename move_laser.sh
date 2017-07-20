#!/bin/bash

echo "mounting shared folder with LabView PC"
#/home/hadaq/mnt/mount_rossendorf_share.sh
echo "sending new position"
echo "x: $1 y: $2 z:$3" 
./check_connection.sh
echo -e "$1\n$2\n$3\n" > lasertest/position.config
rm lasertest/msgArrived.config
> lasertest/cmdMove.config
echo "waiting for confirmation"
while ! [ -e lasertest/msgArrived.config ]; do sleep 0.5
        ./check_connection.sh
	done
echo "arrived!"

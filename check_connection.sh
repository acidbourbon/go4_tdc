#!/bin/bash

while ! [ $(cat lasertest/test.txt) == "connected" ]; do
       echo "connection to LabView lost, please mount shared folder";
       sleep 1
done 
printf "connection to LabView okay ...\r"

#!/bin/bash


stty -F /dev/ttyUSB_stage_pollux_house speed 19200
sleep 0.1
echo "3 ncal" > /dev/ttyUSB_stage_pollux_house
sleep 0.1
echo "4 ncal" > /dev/ttyUSB_stage_pollux_house
sleep 0.1

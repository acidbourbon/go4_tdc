#!/bin/bash

while true; do
	./set_asd8_thresh.sh $(cat current_asd8_thresh.txt)
	sleep 2
done

#!/bin/bash

for i in $(seq $@); do
	        #./go4_on_all.sh l8n8_data/$(printf "%03d*" $i)
	        ./go4_on_all.sh elements_data/$(printf "%03d*" $i)
	done


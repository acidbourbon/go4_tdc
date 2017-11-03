#!/bin/bash

for i in $(seq $@); do
	./go4_on_all.sh data/$(printf "%03d*" $i)
done

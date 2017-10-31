#!/bin/bash

host=$1
while true; do
if [ "$(./vxi11_cmd $host "*OPC?")" == "1" ]; then
	echo "operation complete!"
	break
else
	sleep 1
	printf "\b.-"
fi
done

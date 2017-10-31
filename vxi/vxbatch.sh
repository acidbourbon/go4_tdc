#!/bin/bash

host=$1
infile=$2
delay=0.1

while read line; do 
echo $host "$line"
./vxi11_cmd $host "$line"
sleep $delay
done < $infile
echo done

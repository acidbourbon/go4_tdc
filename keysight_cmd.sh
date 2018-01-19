#!/bin/bash

exec 4<>/dev/tcp/192.168.103.142/5025
echo -e $1 >&4;
# do i expect an answer? -> read answer
if [ $(echo "$1" | grep "?") ]; then
read t <&4; echo $t
fi
 
exec 4<>$-;

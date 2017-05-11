#!/bin/bash
    threshx=$(printf "%04x" $1)
    threshd=$(printf "thr_%05d" $1)
    echo "setting thresh $threshd = 0x$threshx"
    curl http://localhost:1148/commands/put.pl?1483-d400-30${threshx}-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1
    curl http://localhost:1148/commands/put.pl?1483-d400-30${threshx}-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-2-1

#!/bin/bash
cd $(dirname $0)
./vxbatch.sh 192.168.100.48 acq_cmd.txt; ./wait_for_opc.sh 192.168.100.48
./vxbatch.sh 192.168.100.48 make_scope_fast.txt; ./wait_for_opc.sh 192.168.100.48

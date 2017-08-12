#!/bin/bash

export FN=/home/hadaq/mdctest/tmp/*.hld #assuming only one file to be there
export CALMODE=-1
rm -f *.root
go4analysis -number 1000000 -user $FN
export CALMODE=0
rm -f *.root




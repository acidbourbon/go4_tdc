#!/bin/bash

scan_name=$1
acq_name=$2
dump_dir="/home/hadaq/mdctest/tmp/"
data_dir="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/data"
acq_info="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/acq_info.txt"
laser_pos="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/lasertest/position.config"
laser_intensity="/home/hadaq/mdctest/trbsoft/daqtools/users/asd8_tdc/lasertest/intensity.config"

ts=$(date '+%Y-%m-%d_%H:%M:%S')
timeout=$( grep -Po '(?<=timeout=).*' $acq_info )


. my_trb3login

outdir=$data_dir/$scan_name/${ts}_$acq_name
mkdir -p $outdir
killall dabc_exe
rm $dump_dir/*.hld

echo "re-setting threshold:"
#curl http://localhost:1148/commands/put.pl?0353-d400-309373-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-1-1

date '+%Y-%m-%d_%H:%M:%S' > $outdir/ts_acq_start.txt
timeout $timeout dabc_exe TdcEventBuilder.xml
date '+%Y-%m-%d_%H:%M:%S' > $outdir/ts_acq_stop.txt
mv $dump_dir/*.hld $outdir/

cp $acq_info $outdir/acq_info.txt
echo -e "\n\nlaser position:\n" >> $outdir/acq_info.txt
#./check_connection.sh
cat $laser_pos>> $outdir/acq_info.txt
echo -e "\n\nlaser intensity:\n" >> $outdir/acq_info.txt
cat $laser_intensity>> $outdir/acq_info.txt

cp $laser_intensity $outdir/laser_intensity.txt


#!/bin/bash

. set_env
sudo service isc-dhcp-server start
killall trbnetd
sleep 10

#sudo /usr/sbin/rcrpcbind start
sudo service rpcbind start
#TRB3_SERVER=192.168.4.240 trbnetd -i 148
trbnetd -i 148
#export DAQOPSERVER=localhost:148
trbcmd i 0xffff

echo "if you see a list of FPGAs, trbnetd is running properly"
echo
echo



echo "trbcmd reset"
trbcmd reset
echo 
echo "distribute addresses"
./addresses.sh


#disable unwanted FPGAs
bm=$[ 0*8 + 0*4 + 0*2 + 0*1 ]
hub=0xc148

trbcmd clearbit $hub 0xc0 $bm
trbcmd clearbit $hub 0xc1 $bm
trbcmd clearbit $hub 0xc3 $bm

# list online FPGAs
echo
echo "online FPGAs after addressing"
trbcmd i 0xffff
echo
echo "Hubs"
#timeout setting (default value)
trbcmd w 0xfffe 0xc5 0x50ff



echo
echo "load registerdb.pl ..."
../../tools/loadregisterdb.pl register_configgbe.db
../../tools/loadregisterdb.pl register_configgbe_ip.db


#echo "cts"
# trigger rate 
#trbcmd w 0xc148 0xa154  1000000


#echo "tdc" 
#trbcmd w 0xfe48 0xc801 0x000f0000
#trbcmd w 0xfe48 0xc804 0x00000040

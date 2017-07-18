#!/bin/bash
#
#pktime=20 thr=10 ./scan_pt.sh y_scan_PASTTREC_24
#pktime=20 thr=40 ./scan_pt.sh y_scan_PASTTREC_25
#pktime=15 thr=10 ./scan_pt.sh y_scan_PASTTREC_26
#pktime=15 thr=40 ./scan_pt.sh y_scan_PASTTREC_27
#pktime=10 thr=10 ./scan_pt.sh y_scan_PASTTREC_28
#pktime=10 thr=40 ./scan_pt.sh y_scan_PASTTREC_29

#./scan.sh yz_vol_scan_ASD8_31
#mv point_list_3600.txt point_list.txt
#./scan.sh yz_vol_scan_ASD8_32
#mv point_list_3400.txt point_list.txt
#./scan.sh yz_vol_scan_ASD8_33
#mv point_list_3300.txt point_list.txt
#./scan.sh yz_vol_scan_ASD8_34
#mv point_list_3200.txt point_list.txt
#./scan.sh yz_vol_scan_ASD8_35


#cat point_list_quarter_fine.txt | sed 's/^3200/4000/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_36
#cat point_list_quarter_fine.txt | sed 's/^3200/3900/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_37
#cat point_list_quarter_fine.txt | sed 's/^3200/3800/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_38
#cat point_list_quarter_fine.txt | sed 's/^3200/3700/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_39
#cat point_list_quarter_fine.txt | sed 's/^3200/3500/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_40
#cp point_list_2pi.txt point_list.txt
#./scan.sh yz_vol_scan_ASD8_41
#cat point_list_quarter_fine.txt | sed 's/^3200/4200/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_42
#cat point_list_quarter_fine.txt | sed 's/^3200/4300/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_43
#cat point_list_quarter_fine.txt | sed 's/^3200/4400/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_44
#cat point_list_quarter_fine.txt | sed 's/^3200/4500/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_45
#cat point_list_quarter_fine.txt | sed 's/^3200/4600/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_46

# will not be measured

#cat point_list_quarter_fine.txt | sed 's/^3200/4700/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_47
#cat point_list_quarter_fine.txt | sed 's/^3200/4800/' >point_list.txt
#./scan.sh yz_vol_scan_ASD8_48

cat point_list_quarter_fine.txt | sed 's/^3200/3150/' >point_list.txt
./scan.sh yz_vol_scan_ASD8_47
cat point_list_quarter_fine.txt | sed 's/^3200/3250/' >point_list.txt
./scan.sh yz_vol_scan_ASD8_48



cat point_list_quarter_fine.txt | sed 's/^3200/4900/' >point_list.txt
./scan.sh yz_vol_scan_ASD8_49
cat point_list_quarter_fine.txt | sed 's/^3200/5000/' >point_list.txt
./scan.sh yz_vol_scan_ASD8_50

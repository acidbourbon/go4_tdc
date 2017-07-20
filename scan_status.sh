#!/bin/bash

newest_scan="data/"$(ls data/ -t1 | head -n 1)

newest_acq=$newest_scan/$(ls $newest_scan/*/ -d -t1 | head -n 1)
first_acq=$newest_scan/$(ls $newest_scan/*/ -d -t1 | tail -n 1)

echo "newest scan : "$newest_scan
echo "first acq   : "$first_acq
echo "newest acq  : "$newest_acq

first_acq_epoch=$(stat --printf="%Z" $(ls -d $newest_scan/*/ -t1 | tail -n 1))
newest_acq_epoch=$(stat --printf="%Z" $(ls -d $newest_scan/*/ -t1 | head -n 1))
# echo "newest epoch" $newest_acq_epoch
# echo "first epoch" $first_acq_epoch
echo

sec_running=$(echo "$newest_acq_epoch-$first_acq_epoch" | bc )
min_running=$(echo "scale=2;($newest_acq_epoch-$first_acq_epoch)/60" | bc )
hrs_running=$(echo "scale=2;($newest_acq_epoch-$first_acq_epoch)/3600" | bc )

points_sched=$(wc -l $newest_scan/point_list.txt| cut -f 1 -d " ")
points_done=$(find $newest_scan -name "*.hld" | wc -l)

echo "points scheduled   : "$points_sched
echo "points done        : "$points_done
echo "% finished         : "$(echo "scale=2;$points_done / $points_sched * 100" | bc)"%"
echo "minutes running    : $min_running ($hrs_running hours)"
fraction=$(echo "scale=5;$points_done / $points_sched" | bc)
hrs_left=$(echo "scale=2; $hrs_running*(1/$fraction-1)" | bc)
min_left=$(echo "scale=2; $min_running*(1/$fraction-1)" | bc)
hrs_total=$(echo "scale=2; $hrs_running*(1/$fraction)" | bc)
echo

echo "hours total        :" $hrs_total
echo "hours running      :" $hrs_running
echo "hours left         :" $hrs_left 
echo "minutes left       :" $min_left 
echo
echo "should be finished at (rough estimation):"
date --date="@$(echo "$first_acq_epoch + $sec_running/$fraction"| bc)"

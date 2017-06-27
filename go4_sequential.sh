#!/bin/bash


fname=$1

dir=$(dirname $fname)
rm $dir/all_results.txt $dir/events.txt

for i in $(seq 0 200 18000); do

{
  . trb3login_x230

  rm Go4AutoSave.root
  go4analysis -user $fname -num $i
  mv Go4AutoSave.root $fname.root
  cp second.C $dir/second.C_bak
}

./draw_things.sh $fname.root -q
subdir=$dir/$(printf "%06d" $i)
mkdir -p $subdir
mv $dir/*.png $subdir/
cat $dir/results.txt >> $dir/all_results.txt
echo $i >> $dir/events.txt

done

cd $dir
find_and_animate
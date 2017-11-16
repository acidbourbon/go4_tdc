#!/bin/bash


#. my_trb3login
. trb3login_x230
for scan_folder in $@; do
for i in $(find $scan_folder -name "*.hld"); do
  rm Go4AutoSave.root
  go4analysis -user $i -disable-errstop
  mv Go4AutoSave.root ${i}.root
  mv tree_out.root ${i}.tree_out.root
  cp second.C $scan_folder/second.C_bak
done
done

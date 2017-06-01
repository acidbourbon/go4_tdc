#!/bin/bash

scan_folder=$1

. my_trb3login
for i in $(find $scan_folder -name "*.hld"); do
  rm Go4AutoSave.root
  go4analysis -user $i
  mv Go4AutoSave.root ${i}.root
  cp second.C $scan_folder/second.C_bak
done

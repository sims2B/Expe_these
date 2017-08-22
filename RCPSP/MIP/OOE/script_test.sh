#!/bin/sh
for file in $1/*
do 
    new=`echo ${file%.sm.optW}`
    echo "transformation $file => $new.rcp.sm.optW"
    mv -i "$file" "$new.rcp.sm.optW"
done

#!/bin/bash

dir=$1

if $(echo $dir | grep -E '/$' > /dev/null 2> /dev/null); then
    true;
else
    dir=$dir/
fi

for x in "$dir"*-{a,a1,e}.*.out; do
    echo $x
    n=$(echo $x | cut -d. -f2)
    c=$(echo $x | cut -d. -f1 | cut -d- -f4)
    gawk -f scripts/extract"$2".awk $x > "$dir"all-$c-$n
done
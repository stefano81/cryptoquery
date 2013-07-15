#!/bin/bash

for curve in examples/param/*.param; do
    file=data/$(date "+%Y-%m-%d-%H.%M")-$(basename $curve | cut -f1 -d.)
    echo $curve
    for x in $(jot 50 2); do
	echo $x
	./src/testing $curve $x > $file.$x.out 2> $file.$x.err
    done
done
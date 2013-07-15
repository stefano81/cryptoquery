#!/bin/bash

if [ -z "$1" ]; then
    for curve in examples/param/*.param; do
	file=data/$(date "+%Y-%m-%d")-$(basename $curve | cut -f1 -d.)
	echo $curve
	for x in $(jot 50 2); do
	    echo $x
	    ./src/testing $curve $x > $file.$x.out #2> $file.$x.err
	done
    done
else
    for x in $(jot 50 2); do
	echo $x
	./src/testing $1 $x > data/$(date "+%Y-%m-%d").$(basename $1 | cut -f1 -d.).$x.out
    done
fi
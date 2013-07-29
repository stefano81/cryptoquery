#!/bin/bash

if [ -z "$1" ]; then
    for curve in examples/param/*.param; do
	file=data/amm/$(date "+%Y-%m-%d")-$(basename $curve | cut -f1 -d.)
	echo $curve
	for x in $(jot 30 2); do
	    echo $x
	    ./src/testingA $curve $x > $file.$x.out #2> $file.$x.err
	done
    done
else
    if [ -z "$2" ]; then
	for x in $(jot 30 2); do
	    echo $x
	    ./src/testingA $1 $x > data/amm/$(date "+%Y-%m-%d").$(basename $1 | cut -f1 -d.).$x.out
	done
    else
	echo $2
	./src/testingA $1 $2 > data/amm/$(date "+%Y-%m-%d").$(basename $1 | cut -f1 -d.).$2.out
    fi
fi
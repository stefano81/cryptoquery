#!/bin/bash

if [ -z "$1" ]; then
    for curve in examples/param/{a,a1,e}.param; do
	file=data/$(date "+%Y-%m-%d")-$(basename $curve | cut -f1 -d.)
	echo $curve
	for x in $(jot 30 2); do
	    echo $x
	    ./src/testing $curve $x > $file.$x.out #2> $file.$x.err
	    if [ ! $? ]; then
		break;
	    fi
	done
    done
else
    if [ -z "$2" ]; then
	for x in $(jot 30 2); do
	    echo $x
	    ./src/testing $1 $x > data/$(date "+%Y-%m-%d").$(basename $1 | cut -f1 -d.).$x.out
	    if [ ! $? ]; then
		break;
	    fi
	done
    else
	echo $2
	./src/testing $1 $2 > data/$(date "+%Y-%m-%d").$(basename $1 | cut -f1 -d.).$2.out
    fi
fi
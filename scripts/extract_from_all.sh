#!/bin/bash

for x in all-*; do
    t=$(echo $x | cut -f2 -d-)

    # setup
    v=$(cat $x | grep "setup")
    if [ "$v" ]; then
	echo $v | cut -f2- -d\  >> setup-$t
    fi

    # encrypt
    v=$(cat $x | grep "encrypt")
    if [ "$v" ]; then
	echo $v | cut -f2- -d\  >> encrypt-$t
    fi

    # keygen
    v=$(cat $x | grep "keygen")
    if [ "$v" ]; then
	echo $v | cut -f2- -d\  >> keygen-$t
    fi

    # decrypt
    v=$(cat $x | grep "decrypt")
    if [ "$v" ]; then
	echo $v | cut -f2- -d\  >> decrypt-$t
    fi

done
BEGIN {
    n=0
    s=0.0
}
/^.+ setup / {
    print "setup",$1,$3
}
/^.+ encrypt / {
    print "encrypt",$1,$3
}
/^.+ decrypt / {
    vals[n]= $3
    n++
    s += $3
    l = $1
}
/^.+ keygen / {
    print "keygen",$1,$3
}
END {
    asort(vals)
    if (0 != n) {
	print "decrypt", l, s/n, vals[1],vals[n]
    } else {
	print "decrypt",l,"FAILED!"
    }
}
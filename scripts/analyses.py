#!/usr/bin/env python

import sqlite3
import sys

def main() :
    with sqlite3.connect(sys.argv[1]) as db :
        hist = {}
        for t in db.execute('SELECT c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 FROM plain_data'):
            for i in range(0, 10):
                c = hist.setdefault(i, {})
                if t[i] in c :
                    c[t[i]] += 1
                else:
                    c[t[i]] = 1
        
        for k in sorted(hist) :
            print '=========='
            print k
            print '=========='
            for v in sorted(hist[k]) :
                print v,' : ', hist[k][v]
            

if __name__ == '__main__' :
    main()

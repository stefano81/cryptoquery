#!/usr/bin/env python

import sys

def main():
    file = open(sys.argv[1], r'r')
    b = ''
    c = 0
    s = 0
    vals = []
    for line in file:
        l = line.strip().split(' ')

        if b != l[1]:
            if 0 != len(b):
                vals.sort()
                if 0 == len(vals) % 2:
                    m = (vals[len(vals) / 2] + vals[len(vals)  / 2 - 1]) / 2.0
                else:
                    m = vals[len(vals) / 2]
                print b, c, (s/len(vals)), vals[9], vals[len(vals) - 10]

            s = 0
            vals = []
            c = l[2]
            b = l[1]
        vals.append(int(l[4]))
        s += int(l[4])

if __name__ == '__main__':
    main()


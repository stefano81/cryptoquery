#!/usr/bin/env python

import sys

def main():
    with open(sys.argv[1], 'r') as file:
        for line in file:
            print 'insert into plain_data values (',
            nd = []
            for val in line.strip().split(' '):
                v = int(val)
                if v < 0:
                    v = -v
                nd.append(str(v))
            print ','.join(nd),
            print ');'

if __name__ == '__main__':
    main()

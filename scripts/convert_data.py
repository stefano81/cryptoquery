#!/usr/bin/env python

import sys

def main():
    with open(sys.argv[1], 'r') as file:
        for line in file:
            print 'insert into plain_data values (',
            print line.strip().replace(' ', ', '),
            print ');'

if __name__ == '__main__':
    main()

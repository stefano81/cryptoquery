#!/usr/bin/env python

import sys

def main():
    times = {}
    with open(sys.argv[1], 'r') as f:
        for line in f:
            parts = line.split()
            if 'setup' == parts[1]:
                times['setup'] = float(parts[2])
            elif 'encrypt' == parts[1]:
                times['encrypt'] = float(parts[2])
            elif 'keygen' == parts[1]:
                times.setdefault('keygen', []).append(float(parts[3]))
            elif 'decrypt' == parts[1]:
                times.setdefault('decrypt', {}).setdefault((int(parts[2]) + 1), []).append(float(parts[4]))

        if 'setup' in times:
            print 'setup', times['setup']
            print 'encrypt', times['encrypt']

            t = times['keygen']
            print 'keygen', sum(t) / len(t), min(t), max(t)
    
            ts = times['decrypt']
            for k in ts.keys():
                t = ts[k]
                print 'decrypt', k, sum(t) / len(t), min(t), max(t)

if __name__ == '__main__':
    main()

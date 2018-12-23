#!/usr/bin/env python3

def parse_size(s):
    s = s.lower()
    SUFFIXES = {'k': 1e3, 'm': 1e6, 'g': 1e9}
    if s[-1] in SUFFIXES:
        return int(int(s[:-1]) * SUFFIXES[s[-1]])
    return int(s)

def hexdump(data, byte_per_line=64):
    for i in range(0, len(data), byte_per_line):
        out = data[i:i+byte_per_line]
        print(' '.join(f'{out[k]:02x}' for k in range(len(out)))) # use len(out) to handle partial lines

if __name__ == '__main__':
    from itertools import product
    import os
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('cmd')
    parser.add_argument('-l', '--length', default='1M', help='Generate [length} byte test vector. Only applicable to random.')
    parser.add_argument('-s', '--syncfreq', default=None, help='Emit comma every [syncfreq] bytes. Default: don\'t emit any commas. Only applicable to random.')
    args = parser.parse_args()

    length = parse_size(args.length)
    syncfreq = parse_size(args.syncfreq)


    if args.cmd == 'exhaustive_separated':
        for i, j, k in product(range(256), range(256), range(256)):
            print(f'K.28.1 {i:02x} {j:02x} {k:02x}')

    elif args.cmd == 'exhaustive_block':
        print('K.28.1')
        for i, j, k in product(range(256), range(256), range(256)):
            print(f'{i:02x} {j:02x} {k:02x}')

    elif args.cmd == 'random':
        for chunk in range(0, length, syncfreq):
            print('K.28.1')
            hexdump(os.urandom(min(length-chunk, syncfreq)))


#!/usr/bin/env python3

import statistics

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('infile')

    parser.add_argument('-b', '--bitlen', type=float, default=250, help='Average length of a bit in fractional samples')
    parser.add_argument('-j', '--jitter', type=float, default=0, help='Bit length jitter standard deviation in fractional samples')

    parser.add_argument('-v', '--high', type=float, default=12.0, help='High level in V')
    parser.add_argument('-l', '--low', type=float, default=0.2, help='Low level in V')
    parser.add_argument('-n', '--noise', type=float, default=0, help='Gaussian white voltage noise amplitude on top of signal')

    parser.add_argument('-r', '--risetime', type=int, default=1, help='Rise time in sp/V')
    parser.add_argument('-f', '--falltime', type=int, default=1, help='Fall time in sp/V')
    #parser.add_argument('-g', '--glitchfreq', type=float, default=0, help='Add glitches every {g} samples on average (set to 0 to turn off)')
    #parser.add_argument('--glitchlen', type=float, default=0, help='Length of glitches to add in samples')
    #parser.add_argument('-t', '--glitch-constant', type=float, default=0, help='When the signal stays constant between bits add a glitch with probability {t}')
    args = paresr.parse_args()

    with open(args.infile, 'r') as f:
        bits = f.read().replace(' ', '').splitlines()

    
    def join_transitions(chunks):
        for l, r in zip(chunks, chunks[1:]):
            t = args.risetime if l[-1] < r[0] else args.falltime
            yield from (l[-1] + (r[0] - l[-1] * i/t) for i in range(t))
            yield from c[args.risetime:-args.falltime]

    def add_noise(vals):
        yield from (val + random.gauss(0, args.noise) for val in vals)

    def encode_bits(bitstring):
        return add_noise(join_transitions([ args.high if bit == '1' else args.low for _ in range(random.gauss(args.bitlen, args.jitter)) ]))


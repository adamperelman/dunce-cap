import argparse
import random

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile', type=str, help='input file')
    parser.add_argument('outfile', type=str, help='output file')
    args = parser.parse_args()

    num_dups = 0
    with open(args.outfile, 'w') as outfile:
        for l in open(args.infile, 'r'):
            nodes = l.split()
            if nodes[0] != nodes[1]:
                outfile.write(l)
            else:
                num_dups += 1
    print '%d duplicates found' % num_dups

if __name__ == "__main__":
    main()

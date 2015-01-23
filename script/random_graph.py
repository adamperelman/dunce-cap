import argparse
import random

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('N', type = int, help='number of nodes')
  parser.add_argument('p', type=float, help='probability of two distinct nodes being connected')
  args = parser.parse_args()

  node_list = range(args.N)

  for node1 in node_list:
  	for node2 in node_list:
  		if node1 > node2 and random.random() <= args.p:
  			print node1, node2
  			print node2, node1

if __name__ == "__main__":
  main()

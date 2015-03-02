import argparse
import json

RELATIONS = [
  ["A", "B"],
  ["B", "C"],
  ["C", "A"],
  ["A", "D"],
  ["D", "E"]
]

# Yield all distinct assignments of relations to bags.
def bucket_assignments():
  buckets = [[] for _ in range(len(RELATIONS))]

  def _assignments(rel_index):
    for possible_bucket in range(rel_index + 1):
      buckets[possible_bucket].append(rel_index)
      if rel_index == len(RELATIONS) - 1:
        yield
      else:
        for _ in _assignments(rel_index + 1):
          yield
      buckets[possible_bucket].pop()

  for _ in _assignments(0):
    yield buckets


class Node(object):
  def __init__(self, relations):
    self.relations = [RELATIONS[rel] for rel in relations]
    self.attrs = set(a for rel in self.relations for a in rel)
    self.neighbors = []

def build_tree(buckets):
  nodes = [Node(bucket) for bucket in buckets if bucket]
  for i, node_i in enumerate(nodes):
    for node_j in nodes[i+1:]:
      if len(node_i.attrs & node_j.attrs) > 0:
        node_i.neighbors.append(node_j)
        node_j.neighbors.append(node_i)

  return nodes

def check_for_cycle(node, visited, prev_node):
  if node in visited:
    return True

  visited.add(node)

  return any(check_for_cycle(n, visited, node)
             for n in node.neighbors
             if n is not prev_node)

def is_tree(nodes):
  visited = set()
  for node in nodes:
    if node not in visited:
      if check_for_cycle(node, visited, None):
        return False
  return True


def tree_width(nodes):
  bag_widths = [len(n.attrs) - 1 for n in nodes]
  width = max(bag_widths)
  return width, bag_widths.count(width)

def make_json(node, visited):
  visited.add(node)

  result = {}
  result['relations'] = [{'attrs': rel}
                         for rel in node.relations]

  if any(n not in visited for n in node.neighbors):
    result['children'] = [make_json(n, visited)
                          for n in node.neighbors
                          if n not in visited]

  return result

def write_tree(nodes, outfile):
  # TODO: assumes graph is connected
  json_data = make_json(nodes[0], set())
  with open(outfile, 'w') as f:
    json.dump(json_data, f, indent=4, separators=(',', ': '))

def print_tree(node, visited=set()):
  if node in visited:
    return

  visited.add(node)
  print node.relations
  print node.attrs
  print 'neighbors:', [n.relations for n in node.neighbors]
  print
  for n in node.neighbors:
    print_tree(n, visited)

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('outfile', help='output file')
  args = parser.parse_args()

  best_tree = None
  best_width, best_count = float('inf'), float('inf')
  for buckets in bucket_assignments():
    tree = build_tree(buckets)
    if not is_tree(tree):
      continue
    # TODO do we need to check any other conditions
    # to make sure this is a valid hypertree decomp?
    width, count = tree_width(tree)
    if width < best_width or width == best_width and count < best_count:
      best_tree = tree
      best_width = width
      best_count = count

  print_tree(best_tree[0])
  write_tree(best_tree, args.outfile)

if __name__ == "__main__":
  main()

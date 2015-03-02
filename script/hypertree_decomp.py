RELATIONS = [
  ["A", "B"],
  ["B", "C"],
  ["C", "A"],
  ["A", "D"],
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
    self.relations = relations
    self.attrs = set(a for rel in self.relations for a in RELATIONS[rel])
    self.neighbors = []

def build_tree(buckets):
  nodes = [Node(bucket) for bucket in buckets if bucket]
  for i, node_i in enumerate(nodes):
    for node_j in nodes[i+1:]:
      if len(node_i.attrs & node_j.attrs) > 0:
        node_i.neighbors.append(node_j)
        node_j.neighbors.append(node_i)

  return nodes

def check_for_cycle(node, visited):
  if node in visited:
    return True

  visited.add(node)

  for neighbor in node.neighbors:
    if check_for_cycle(neighbor, visited):
      return True

  return False

def is_tree(nodes):
  visited = set()
  for node in nodes:
    if node not in visited:
      if check_for_cycle(node, visited):
        return False
  return True


def tree_width(nodes):
  return max(len(n.attrs)-1 for n in nodes)

def main():
  best_tree = None
  best_width = float('inf')
  for buckets in bucket_assignments():
    tree = build_tree(buckets)
    if is_tree(tree):
      width = tree_width(tree)
      if width < best_width:
        best_tree = tree
        best_width = width


  print best_tree

if __name__ == "__main__":
  main()

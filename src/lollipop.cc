#include <iostream>
#include <chrono>
#include <algorithm>

#include "relation.h"
#include "yannakakis.h"
#include "generic_join.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "Usage: ./lollipop.exe <filename>" << endl;
    exit(0);
  }

  unique_ptr<BagNode> root_bag(new BagNode());

  cout << "loading relations..." << endl;
  root_bag->relations.push_back(TrieNode::FromFile(argv[1], {"a", "b"}));
  root_bag->relations.push_back(TrieNode::FromFile(argv[1], {"b", "c"}));
  root_bag->relations.push_back(TrieNode::FromFile(argv[1], {"c", "d"}));
  root_bag->relations.push_back(TrieNode::FromFile(argv[1], {"d", "a"}));
  root_bag->relations.push_back(TrieNode::FromFile(argv[1], {"a", "c"}));
  root_bag->relations.push_back(TrieNode::FromFile(argv[1], {"b", "d"}));

  root_bag->children.push_back(unique_ptr<BagNode>(new BagNode));
  root_bag->children[0]->relations.push_back(TrieNode::FromFile(argv[1], {"a", "e"}));

  cout << "loaded relations" << endl;

  cout << "performing join..." << endl;
  typedef chrono::high_resolution_clock Clock;
  typedef chrono::milliseconds ms;

  Clock::time_point start_join = Clock::now();
  TrieNode* result = YannakakisJoin(root_bag.get());
  Clock::time_point end_join = Clock::now();

  cout << "done performing join" << endl;
  ms join_time = chrono::duration_cast<ms>(end_join - start_join);

  cout << join_time.count() << "ms\n" << endl;
  cout << "size: " << result->size() << endl;

  delete result;
}

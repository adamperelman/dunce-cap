#include <iostream>
#include <chrono>
#include <algorithm>

#include "relation.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Usage: ./pairwise.exe <filename> [<filename2>]" << endl;
    exit(0);
  }

  const char* fname2 = argv[1];
  if (argc > 2) {
    fname2 = argv[2];
  }

  cout << "loading relations..." << endl;
  unique_ptr<TrieNode> r1(TrieNode::FromFile(argv[1], {"a", "b"}));
  unique_ptr<TrieNode> r2(TrieNode::FromFile(fname2, {"b", "c"}));
  cout << "loaded relations" << endl;

  cout << "performing join..." << endl;
  typedef chrono::high_resolution_clock Clock;
  typedef chrono::milliseconds ms;

  Clock::time_point start_join = Clock::now();
  TrieNode* result = TrieNode::PairwiseJoin(r1.get(), r2.get());
  Clock::time_point end_join = Clock::now();

  cout << "done performing join" << endl;
  ms join_time = chrono::duration_cast<ms>(end_join - start_join);

  cout << join_time.count() << "ms\n" << endl;
  cout << "size: " << result->size() << endl;

  delete result;
}

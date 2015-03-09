// This file is an example of what generated code should look like.
// In particular, it runs the triangle query.
#include <chrono>
#include <algorithm>
#include "relation.h"
#include "yannakakis.h"
#include "generic_join.h"

using namespace std;

bool vectorLengthCmp(const vector<int>* a, const vector<int>* b) {
  return a->size() < b->size();
}

vector<int> Intersection(const vector<const vector<int>*>& ordered_sets) {
  if (ordered_sets.size() == 1) {
    return *ordered_sets[0];
  }

  sort(ordered_sets.begin(), ordered_sets.end(), vectorLengthCmp);

  vector<int> scratch_space[2];
  scratch_space[0] = vector<int>(*ordered_sets[0]);
  scratch_space[1] = vector<int>();
  scratch_space[1].reserve(scratch_space[0].size());

  for (int i = 1; i < ordered_sets.size(); i++) {
    const vector<int>& prev_result = scratch_space[(i+1) % 2];
    vector<int>& new_result = scratch_space[i % 2];
    new_result.clear();
    set_intersection(prev_result.begin(), prev_result.end(),
                     ordered_sets.at(i)->begin(), ordered_sets.at(i)->end(),
                     inserter(new_result, new_result.begin()));
  }

  return scratch_space[(ordered_sets.size()+1) % 2];
}

int main(int argc, char* argv[]) {

  vector<const TrieNode*> relations;

  cout << "loading files..." << endl;
  TrieNode* r1 = TrieNode::FromFile("data/facebook_bidirectional.txt", {"a","b"});
  TrieNode* r2 = TrieNode::FromFile("data/facebook_bidirectional.txt", {"b","c"});
  TrieNode* r3 = TrieNode::FromFile("data/facebook_bidirectional.txt", {"a","c"});

  typedef chrono::high_resolution_clock Clock;
  typedef chrono::milliseconds ms;

  cout << "performing join..." << endl;
  Clock::time_point start_join = Clock::now();

  vector<const vector<int>*> rels = {r1->values(), r3->values()};
  vector<int> shared_a = Intersection(rels);

  int result = shared_a.size();


  Clock::time_point end_join = Clock::now();

  cout << "done performing join" << endl;
  ms join_time = chrono::duration_cast<ms>(end_join - start_join);

  cout << join_time.count() << "ms\n" << endl;
  cout << "size: " << result << endl;
}

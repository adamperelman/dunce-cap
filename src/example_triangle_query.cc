#include <chrono>
#include <algorithm>

#include "relation.h"

using namespace std;

// TODO: make sure we use these global vars safely
// if we do multithreading!
static vector<int> scratch_space[2];

static bool vectorLengthCmp(const vector<int>* a, const vector<int>* b) {
  return a->size() < b->size();
}

static inline vector<int> Intersection(vector<const vector<int>*>& ordered_sets) {
  sort(ordered_sets.begin(), ordered_sets.end(), vectorLengthCmp);

  const vector<int>* prev_result = ordered_sets[0];
  vector<int>* new_result = &scratch_space[0];

  for (int i = 1; i < ordered_sets.size(); i++) {
    new_result->clear();
    set_intersection(prev_result->begin(), prev_result->end(),
                     ordered_sets[i]->begin(), ordered_sets[i]->end(),
                     inserter(*new_result, new_result->begin()));

    prev_result = new_result;
    new_result = &scratch_space[i % 2];
  }

  return *prev_result;
}

int main(int argc, char* argv[]) {
  scratch_space[0] = vector<int>();
  scratch_space[1] = vector<int>();

  cout << "loading files..." << endl;
  const TrieNode* r1 = TrieNode::FromFile("data/facebook_bidirectional.txt", {"a","b"});
  const TrieNode* r2 = TrieNode::FromFile("data/facebook_bidirectional.txt", {"b","c"});
  const TrieNode* r3 = TrieNode::FromFile("data/facebook_bidirectional.txt", {"a","c"});

  typedef chrono::high_resolution_clock Clock;
  typedef chrono::milliseconds ms;

  cout << "performing join..." << endl;

  Clock::time_point start_join = Clock::now();
  vector<const TrieNode*> relations = {r1, r2, r3};

  //
  // Start of join code
  //

  vector<const vector<int>*> matching = {&r1->values(), &r3->values()};

  int result = 0;
  vector<int> shared_a = Intersection(matching);

  for (int a : shared_a) {
    const TrieNode* r1_a = r1->MatchingNode("a", a);
    const TrieNode* r3_a = r3->MatchingNode("a", a);
    assert(r1_a && r3_a);

    vector<const vector<int>*> matching = {&r1_a->values(), &r2->values()};
    vector<int> shared_b = Intersection(matching);

    for (int b : shared_b) {
      const TrieNode* r2_b = r2->MatchingNode("b", b);
      assert(r2_b);

      vector<const vector<int>*> matching = {&r2_b->values(), &r3_a->values()};

      result += Intersection(matching).size();
    }
  }

  //
  // End of join code
  //

  Clock::time_point end_join = Clock::now();

  cout << "done performing join" << endl;
  ms join_time = chrono::duration_cast<ms>(end_join - start_join);

  cout << join_time.count() << "ms\n" << endl;
  cout << "size: " << result << endl;

  delete r1;
  delete r2;
  delete r3;
}

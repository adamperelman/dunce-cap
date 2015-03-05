#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"

using namespace std;

TEST_CASE("Relations are created correctly") {
  TrieNode* r = TrieNode::FromFile("data/simple.txt", {"a", "b"});
  REQUIRE(r->size() == 4);
  REQUIRE(r->contains({1, 3}));
  REQUIRE(r->contains({1, 4}));
  REQUIRE(r->contains({2, 3}));
  REQUIRE(r->contains({2, 4}));
  REQUIRE(!r->contains({3,2}));
}

TEST_CASE("Iterating over a relation") {
  TrieNode* r = TrieNode::FromFile("data/simple.txt", {"a", "b"});

  vector<vector<int>> expected_vals = {{1, 3}, {1, 4}, {2, 3}, {2, 4}};

  int i = 0;
  for (TrieNode::const_iterator it = r->begin();
       it != r->end();
       ++it) {
    REQUIRE(*it == expected_vals[i]);
    ++i;
  }

  REQUIRE(i == 4);
}

TEST_CASE("Make sure we can create a relation with 3 attrs") {
  TrieNode* r = TrieNode::FromFile("data/triples.txt", {"c", "a", "b"});
  REQUIRE(r->size() == 3);
  REQUIRE(r->contains({2, 3, 1}));
  REQUIRE(r->contains({5, 6, 4}));
  REQUIRE(r->contains({8, 9, 7}));
  REQUIRE(!r->contains({1, 2, 3}));
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(r->attrs() == expected_attrs);
}

TEST_CASE("Nodes are appended correctly") {
  vector<string> attrs = {"a", "b", "c"};
  unique_ptr<TrieNode> r(new TrieNode("a"));

  vector<vector<int>> tuples = {
    {1, 1, 1},
    {1, 1, 2},
    {1, 2, 1},
    {1, 2, 2},
    {2, 1, 1}
  };

  for (const vector<int>& t : tuples) {
    r->AppendTuple(t.begin(), t.end(), attrs.begin(), attrs.end());
  }

  int i = 0;
  for (TrieNode::const_iterator it = r->begin();
       it != r->end();
       ++it) {
    REQUIRE(*it == tuples[i]);
    ++i;
  }
  REQUIRE(i == tuples.size());
}

TEST_CASE("left semi join works") {
  TrieNode* from = TrieNode::FromFile("data/semijoin_test1.txt", {"a", "b", "d"});
  TrieNode* filter = TrieNode::FromFile("data/semijoin_test2.txt", {"b", "c", "d", "e"});

  TrieNode* result = from->LeftSemijoin(filter);

  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({7, 8, 9}));
  REQUIRE(result->contains({10, 11, 12}));

  vector<string> expected_attrs = {"a", "b", "d"};
  REQUIRE(result->attrs() == expected_attrs);
}

TEST_CASE("pairwise join works") {
  TrieNode* a = TrieNode::FromFile("data/semijoin_test1.txt", {"a", "b", "d"});
  TrieNode* b = TrieNode::FromFile("data/semijoin_test2.txt", {"b", "c", "d", "e"});

  TrieNode* result = TrieNode::PairwiseJoin(a, b);
  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({7, 8, 100, 9, 18}));
  REQUIRE(result->contains({10, 11, 32, 12, 101}));

  vector<string> expected_attrs = {"a", "b", "c", "d", "e"};
  REQUIRE(result->attrs() == expected_attrs);
}

TEST_CASE("simple pairwise count works") {
  TrieNode* a = TrieNode::FromFile("data/joincount_test1.txt", {"a", "b"});
  TrieNode* b = TrieNode::FromFile("data/joincount_test2.txt", {"a", "b"});

  int result = TrieNode::PairwiseCount(a, b);
  REQUIRE(result == 6);
}

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

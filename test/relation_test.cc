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

#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"
#include "../src/generic_join.h"
#include <memory>

using namespace std;

TEST_CASE("Single attribute case works correctly") {
  Database db;

  db.AddRelation("R", TrieNode::FromFile("data/single1.txt", {"a"}));
  db.AddRelation("S", TrieNode::FromFile("data/single2.txt", {"a"}));
  db.AddRelation("T", TrieNode::FromFile("data/single3.txt", {"a"}));

  unique_ptr<TrieNode> result(db.GenericJoin({"R", "S", "T"}));

  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({1}));
  REQUIRE(result->contains({3}));
}


TEST_CASE("Triangle query works correctly") {
  Database db;
  db.AddRelation("R", TrieNode::FromFile("data/triangle.txt", {"a", "b"}));
  db.AddRelation("S", TrieNode::FromFile("data/triangle.txt", {"b", "c"}));
  db.AddRelation("T", TrieNode::FromFile("data/triangle.txt", {"c", "a"}));

  unique_ptr<TrieNode> result(db.GenericJoin({"R", "S", "T"}));
  REQUIRE(result->size() == 3);
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(result->attrs() == expected_attrs);
  REQUIRE(result->contains({1, 2, 3}));
  REQUIRE(result->contains({2, 3, 1}));
  REQUIRE(result->contains({3, 1, 2}));
}

TEST_CASE("4 cliques query works correctly") {
  Database db;
  db.AddRelation("R", TrieNode::FromFile("data/four_cliques.txt", {"a", "b"}));
  db.AddRelation("S", TrieNode::FromFile("data/four_cliques.txt", {"b", "c"}));
  db.AddRelation("T", TrieNode::FromFile("data/four_cliques.txt", {"c", "a"}));
  db.AddRelation("U", TrieNode::FromFile("data/four_cliques.txt", {"a", "d"}));
  db.AddRelation("V", TrieNode::FromFile("data/four_cliques.txt", {"b", "d"}));
  db.AddRelation("W", TrieNode::FromFile("data/four_cliques.txt", {"c", "d"}));

  unique_ptr<TrieNode> result(db.GenericJoin({"R", "S", "T", "U", "V", "W"}));
  // there should be 24 ways to count this one 4-clique
  REQUIRE(result->size() == 24);

  vector<string> expected_attrs({"a", "b", "c", "d"});
  REQUIRE(result->attrs() == expected_attrs);
}

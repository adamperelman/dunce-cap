#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"

using namespace std;

TEST_CASE("Relations are created correctly") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  REQUIRE(r.size() == 4);
  REQUIRE(r.contains({1, 3}));
  REQUIRE(r.contains({1, 4}));
  REQUIRE(r.contains({2, 3}));
  REQUIRE(r.contains({2, 4}));
  REQUIRE(!r.contains({3,2}));
}

TEST_CASE("Make sure we can create a relation with 3 attrs") {
  Relation r("data/triples.txt", "triples", {"a", "b", "c"});
  REQUIRE(r.size() == 3);
  REQUIRE(r.contains({1, 2, 3}));
  REQUIRE(r.contains({4, 5, 6}));
  REQUIRE(r.contains({7, 8, 9}));
  REQUIRE(!r.contains({1, 2, 6}));
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(r.attrs() == expected_attrs);
}

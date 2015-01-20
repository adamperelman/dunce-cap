#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"

using namespace std;

TEST_CASE("Relations are created correctly") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  REQUIRE(r.size() == 4);
}

/* TODO: fix memory leak*/
TEST_CASE("Relations project correctly") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  Relation* projection = r.Project({"a"});
  REQUIRE(projection->size() == 2);
  REQUIRE(projection->contains({1}));
  REQUIRE(projection->contains({2}));
  vector<string> expected_attrs({"a"});
  REQUIRE(projection->attrs() == expected_attrs);
}

TEST_CASE("Left semijoin with a single tuple") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  vector<int> tuple = {3, 5};
  vector<string> attrs = {"b", "c"};
  Relation* result = r.LeftSemiJoin(tuple, attrs);
  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({1,3}));
  REQUIRE(result->contains({2,3}));
  vector<string> expected_attrs({"a", "b"});
  REQUIRE(result->attrs() == expected_attrs);
}

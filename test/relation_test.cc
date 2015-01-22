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

TEST_CASE("Cartersian product with a single tuple") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  vector<int> tuple = {8, 0};
  vector<string> attrs = {"c", "d"};
  Relation* result = r.CartesianProduct(tuple, attrs);
  REQUIRE(result->size() == 4);
  REQUIRE(result->contains({1, 3, 8, 0}));
  REQUIRE(result->contains({1, 4, 8, 0}));
  REQUIRE(result->contains({2, 3, 8, 0}));
  REQUIRE(result->contains({2, 4, 8, 0}));
  vector<string> expected_attrs({"a", "b", "c", "d"});
  REQUIRE(result->attrs() == expected_attrs);
}

TEST_CASE("Intersection of three single-attribute relations") {
  vector<Relation*> relations;
  relations.push_back(new Relation("data/single1.txt", "R", {"a"}));
  relations.push_back(new Relation("data/single2.txt", "S", {"a"}));
  relations.push_back(new Relation("data/single3.txt", "T", {"a"}));

  Relation* result = Relation::Intersect(relations);

  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({1}));
  REQUIRE(result->contains({3}));
}

TEST_CASE("Sort attrs in relation in dictionary order") {
  Relation r("data/triples.txt", "simple", {"c", "a", "b"});
  Relation* result = r.SortedByAttributes();
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(result->attrs() == expected_attrs);
  REQUIRE(result->contains({2, 3, 1}));
  REQUIRE(result->contains({5, 6, 4}));
  REQUIRE(result->contains({8, 9, 7}));
  REQUIRE(result->size() == 3);
}

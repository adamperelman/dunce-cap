#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"
#include "../src/database.h"

using namespace std;

TEST_CASE("Single attribute case works correctly") {
  Database db;

  db.AddRelation(new Relation("data/single1.txt", "R", {"a"}));
  db.AddRelation(new Relation("data/single2.txt", "S", {"a"}));
  db.AddRelation(new Relation("data/single3.txt", "T", {"a"}));

  Relation* result = db.GenericJoin({"R", "S", "T"});

  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({1}));
  REQUIRE(result->contains({3}));
}

TEST_CASE("Triangle query works correctly") {
  Database db;
  db.AddRelation(new Relation("data/triangle.txt", "R", {"a", "b"}));
  db.AddRelation(new Relation("data/triangle.txt", "S", {"b", "c"}));
  db.AddRelation(new Relation("data/triangle.txt", "T", {"c", "a"}));

  // TODO: fix memory leak
  Relation* result = db.GenericJoin({"R", "S", "T"});
  REQUIRE(result->size() == 3);

  Relation* sorted = result->SortedByAttributes();
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(sorted->attrs() == expected_attrs);
  REQUIRE(sorted->contains({1, 2, 3}));
  REQUIRE(sorted->contains({2, 3, 1}));
  REQUIRE(sorted->contains({3, 1, 2}));
}

#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"
#include "../src/generic_join.h"

using namespace std;

TEST_CASE("Triangle query works correctly") {
  vector<Relation> relations;
  relations.push_back(Relation("data/triangle.txt", "R", {"a", "b"}));
  relations.push_back(Relation("data/triangle.txt", "S", {"a", "b"}));
  relations.push_back(Relation("data/triangle.txt", "T", {"a", "b"}));

  // TODO: fix memory leak
  Relation* result = GenericJoin(relations);
  REQUIRE(result->size() == 3);

  Relation* sorted = result->SortedByAttributes();
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(sorted->attrs() == expected_attrs);
  REQUIRE(sorted->contains({1, 2, 3}));
  REQUIRE(sorted->contains({2, 3, 1}));
  REQUIRE(sorted->contains({3, 1, 2}));
}

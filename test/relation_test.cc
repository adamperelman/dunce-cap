#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"

TEST_CASE("Relations are created correctly") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  REQUIRE(r.size() == 4);
}

TEST_CASE("Relations project correctly") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  Relation* projection = r.Project({"a"});
  REQUIRE(projection->size() == 2);
  REQUIRE(projection->contains({1}));
  REQUIRE(projection->contains({2}));
}

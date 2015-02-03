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

#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../relation.h"

TEST_CASE("Relations are created correctly") {
  Relation r("data/simple.txt", "simple", {"a", "b"});
  REQUIRE(r.size() == 4);
}

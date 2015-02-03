#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"
#include "../src/database.h"
#include <memory>

using namespace std;

TEST_CASE("Single attribute case works correctly") {
  Database db;

  db.AddRelation(new Relation("data/single1.txt", "R", {"a"}));
  db.AddRelation(new Relation("data/single2.txt", "S", {"a"}));
  db.AddRelation(new Relation("data/single3.txt", "T", {"a"}));

  unique_ptr<Relation> result(db.GenericJoin({"R", "S", "T"}));

  REQUIRE(result->size() == 2);
  REQUIRE(result->contains({1}));
  REQUIRE(result->contains({3}));
}


TEST_CASE("Triangle query works correctly") {
  Database db;
  db.AddRelation(new Relation("data/triangle.txt", "R", {"a", "b"}));
  db.AddRelation(new Relation("data/triangle.txt", "S", {"b", "c"}));
  db.AddRelation(new Relation("data/triangle.txt", "T", {"c", "a"}));

  unique_ptr<Relation> result(db.GenericJoin({"R", "S", "T"}));
  REQUIRE(result->size() == 3);
  vector<string> expected_attrs({"a", "b", "c"});
  REQUIRE(result->attrs() == expected_attrs);
  REQUIRE(result->contains({1, 2, 3}));
  REQUIRE(result->contains({2, 3, 1}));
  REQUIRE(result->contains({3, 1, 2}));
}

TEST_CASE("4 cliques query works correctly") {
  Database db;
  db.AddRelation(new Relation("data/four_cliques.txt", "R", {"a", "b"}));
  db.AddRelation(new Relation("data/four_cliques.txt", "S", {"b", "c"}));
  db.AddRelation(new Relation("data/four_cliques.txt", "T", {"c", "a"}));
  db.AddRelation(new Relation("data/four_cliques.txt", "U", {"a", "d"}));
  db.AddRelation(new Relation("data/four_cliques.txt", "V", {"b", "d"}));
  db.AddRelation(new Relation("data/four_cliques.txt", "W", {"c", "d"}));

  unique_ptr<Relation> result(db.GenericJoin({"R", "S", "T", "U", "V", "W"}));
  // there should be 24 ways to count this one 4-clique
  REQUIRE(result->size() == 24);

  vector<string> expected_attrs({"a", "b", "c", "d"});
  REQUIRE(result->attrs() == expected_attrs);
}

#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"
#include "../src/yannakakis.h"
#include <memory>

using namespace std;

TEST_CASE("Tadpole query") {
  vector<vector<TrieNode*>> bags(3);
  bags[0].push_back(TrieNode::FromFile("data/tadpole.txt", {"a", "b"}));
  bags[0].push_back(TrieNode::FromFile("data/tadpole.txt", {"b", "c"}));
  bags[0].push_back(TrieNode::FromFile("data/tadpole.txt", {"c", "d"}));
  bags[0].push_back(TrieNode::FromFile("data/tadpole.txt", {"d", "a"}));

  bags[1].push_back(TrieNode::FromFile("data/tadpole.txt", {"a", "e"}));

  bags[2].push_back(TrieNode::FromFile("data/tadpole.txt", {"e", "f"}));

  TrieNode* result = YannakakisJoin(bags);

  REQUIRE(result->size() == 1);
  vector<string> expected_attrs = {"a", "b", "c", "d", "e", "f"};
  REQUIRE(result->attrs() == expected_attrs());
  REQUIRE(result->contains({1, 2, 3, 4, 5, 6}));
}

#define CATCH_CONFIG_MAIN
#include "catch.h"

#include "../src/relation.h"
#include "../src/yannakakis.h"
#include <memory>

using namespace std;

TEST_CASE("Tadpole query") {
  unique_ptr<BagNode> root_bag(new BagNode);
  root_bag->relations.push_back(TrieNode::FromFile("data/tadpole.txt", {"a", "e"}));

  root_bag->children.push_back(unique_ptr<BagNode>(new BagNode));
  root_bag->children[0]->relations.push_back(TrieNode::FromFile("data/tadpole.txt", {"e", "f"}));

  root_bag->children.push_back(unique_ptr<BagNode>(new BagNode));
  root_bag->children[1]->relations.push_back(TrieNode::FromFile("data/tadpole.txt", {"a", "b"}));
  root_bag->children[1]->relations.push_back(TrieNode::FromFile("data/tadpole.txt", {"b", "c"}));
  root_bag->children[1]->relations.push_back(TrieNode::FromFile("data/tadpole.txt", {"c", "d"}));
  root_bag->children[1]->relations.push_back(TrieNode::FromFile("data/tadpole.txt", {"d", "a"}));

  TrieNode* result = YannakakisJoin(root_bag.get());

  REQUIRE(result->size() == 1);
  vector<string> expected_attrs = {"a", "b", "c", "d", "e", "f"};
  REQUIRE(result->attrs() == expected_attrs);
  REQUIRE(result->contains({1, 2, 3, 4, 5, 6}));
}

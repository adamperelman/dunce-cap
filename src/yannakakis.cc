#include "generic_join.h"
#include "yannakakis.h"

using namespace std;

void JoinWithinBags(BagNode* root_bag) {
  TrieNode* new_relation = GenericJoin(root_bag->relations);
  root_bag->joined.reset(new_relation);
  for (const auto& child : root_bag->children) {
    JoinWithinBags(child.get());
  }
}

void LeftSemijoinWithChildren(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    LeftSemijoinWithChildren(child.get());
    TrieNode* reduced = root_bag->joined->LeftSemijoin(child->joined.get());
    root_bag->joined.reset(reduced);
  }
}

void LeftSemijoinWithParent(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    TrieNode* reduced = child->joined->LeftSemijoin(root_bag->joined.get());
    child->joined.reset(reduced);
    LeftSemijoinWithParent(child.get());
  }
}

void JoinWithChildren(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    JoinWithChildren(child.get());
    TrieNode* new_joined = root_bag->joined->Join(child->joined.get());
    root_bag->joined.reset(new_joined);
  }
}

TrieNode* YannakakisJoin(BagNode* root_bag) {
  JoinWithinBags(root_bag);
  LeftSemijoinWithChildren(root_bag);
  LeftSemijoinWithParent(root_bag);
  JoinWithChildren(root_bag);
  return root_bag->joined.release();
}

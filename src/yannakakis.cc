#include "generic_join.h"
#include "yannakakis.h"

void JoinWithinBags(BagNode* root_bag) {
  TrieNode* new_relation = GenericJoin(root_bag->relations);
  root_bag->joined.reset(new_relation);
  for (const auto& child : root_bag->children) {
    JoinWithinBags(child.get());
  }
}

TrieNode* YannakakisJoin(BagNode* root_bag) {
  JoinWithinBags(root_bag);
  return NULL;

}



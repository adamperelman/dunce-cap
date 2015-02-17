#include "generic_join.h"
#include "yannakakis.h"

using namespace std;

void JoinWithinBags(BagNode* root_bag) {
  TrieNode* new_relation = GenericJoin(root_bag->relations);
  root_bag->joined.reset(new_relation);
  cout << *new_relation << endl;
  for (const auto& child : root_bag->children) {
    JoinWithinBags(child.get());
  }
}

void LeftSemijoinWithChildren(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    LeftSemijoinWithChildren(child.get());
    root_bag->joined->LeftSemijoin(child->joined.get());
  }
}

void LeftSemiJoinWithParent(BagNode* root_bag) {

}

TrieNode* YannakakisJoin(BagNode* root_bag) {
  JoinWithinBags(root_bag);


  return NULL;

}



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
    TrieNode* new_joined = TrieNode::PairwiseJoin(root_bag->joined.get(),
                                                  child->joined.get());
    root_bag->joined.reset(new_joined);
  }
}

int CountWithChildren(BagNode* root_bag) {
  // TODO: assumes that there are only 2 bags.
  return TrieNode::PairwiseCount(root_bag->joined.get(),
                                 root_bag->children[0]->joined.get());
}

void FullReducer(BagNode* root_bag) {
  cout << "starting full reducer..." << endl;
  LeftSemijoinWithChildren(root_bag);
  LeftSemijoinWithParent(root_bag);
  cout << "finished full reducer" << endl;
}

TrieNode* YannakakisJoin(BagNode* root_bag) {
  JoinWithinBags(root_bag);
  FullReducer(root_bag);
  JoinWithChildren(root_bag);
  return root_bag->joined.release();
}

int YannakakisCount(BagNode* root_bag) {
  // TODO: only works for 2 bag case right now
  JoinWithinBags(root_bag);
  return CountWithChildren(root_bag);
}

#include "generic_join.h"
#include "yannakakis.h"

using namespace std;

void JoinWithinBags(BagNode* root_bag) {
  TrieNode* new_relation = GenericJoin(root_bag->relations);
  root_bag->joined_.reset(new_relation);
  for (const auto& child : root_bag->children) {
    JoinWithinBags(child.get());
  }
}

void PairwiseJoinWithinBags(BagNode* root_bag) {
  for (int i = 1; i < root_bag->relations.size(); i++) {
    root_bag->joined_.reset(TrieNode::PairwiseJoin(root_bag->joined(),
                                                   root_bag->relations[i]));
  }

  for (const auto& child : root_bag->children) {
    PairwiseJoinWithinBags(child.get());
  }
}

long PairwiseCountWithinBag(BagNode* root_bag) {
  int num_relations = root_bag->relations.size();
  for (int i = 1; i < num_relations - 1; i++) {
    root_bag->joined_.reset(TrieNode::PairwiseJoin(root_bag->joined(),
                                                   root_bag->relations[i]));
  }

  return TrieNode::PairwiseCount(root_bag->joined(), root_bag->relations[num_relations-1]);
}

void LeftSemijoinWithChildren(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    LeftSemijoinWithChildren(child.get());
    TrieNode* reduced = root_bag->joined()->LeftSemijoin(child->joined());
    root_bag->joined_.reset(reduced);
  }
}

void LeftSemijoinWithParent(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    TrieNode* reduced = child->joined()->LeftSemijoin(root_bag->joined());
    child->joined_.reset(reduced);
    LeftSemijoinWithParent(child.get());
  }
}

void JoinWithChildren(BagNode* root_bag) {
  for (const auto& child : root_bag->children) {
    JoinWithChildren(child.get());
    TrieNode* new_joined = TrieNode::PairwiseJoin(root_bag->joined(),
                                                  child->joined());
    root_bag->joined_.reset(new_joined);
  }
}

long CountWithChildren(BagNode* root_bag) {
  // TODO: assumes that there are only 2 bags.
  return TrieNode::PairwiseCount(root_bag->joined(),
                                 root_bag->children[0]->joined());
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
  return root_bag->joined_.release();
}

long YannakakisCount(BagNode* root_bag) {
  // TODO: only works for 1 or 2 bag case right now
  if (root_bag->children.empty()) {
    return GenericJoinCount(root_bag->relations);
  }

  JoinWithinBags(root_bag);
  return CountWithChildren(root_bag);
}

long YannakakisPairwiseCount(BagNode* root_bag) {
  if (root_bag->children.empty()) {
    return PairwiseCountWithinBag(root_bag);
  }

  PairwiseJoinWithinBags(root_bag);
  return CountWithChildren(root_bag);
}

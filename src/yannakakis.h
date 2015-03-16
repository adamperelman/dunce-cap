#ifndef __YANNAKAKIS_H__
#define __YANNAKAKIS_H__

struct BagNode {
  std::vector<std::unique_ptr<BagNode>> children;
  std::vector<const TrieNode*> relations;

  const TrieNode* joined() {
    if (joined_) return joined_.get();
    return relations[0];
  }
  std::unique_ptr<TrieNode> joined_;

  ~BagNode() {
    for (const TrieNode* rel : relations) {
      delete rel;
    }
  }
};

TrieNode* YannakakisJoin(BagNode* root_bag);
long YannakakisCount(BagNode* root_bag);
long YannakakisPairwiseCount(BagNode* root_bag);

#endif

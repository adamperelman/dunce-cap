#ifndef __YANNAKAKIS_H__
#define __YANNAKAKIS_H__

struct BagNode {
  std::vector<std::unique_ptr<BagNode>> children;
  std::vector<const TrieNode*> relations;
  std::unique_ptr<TrieNode> joined;

  ~BagNode() {
    for (const TrieNode* rel : relations) {
      delete rel;
    }
  }
};

TrieNode* YannakakisJoin(BagNode* root_bag);
long YannakakisCount(BagNode* root_bag);

#endif

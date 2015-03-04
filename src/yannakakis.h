#ifndef __YANNAKAKIS_H__
#define __YANNAKAKIS_H__

struct BagNode {
  std::vector<std::unique_ptr<BagNode>> children;
  std::vector<const TrieNode*> relations;
  std::unique_ptr<TrieNode> joined;
};

TrieNode* YannakakisJoin(BagNode* root_bag);
int YannakakisCount(BagNode* root_bag);

#endif

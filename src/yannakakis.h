#ifndef __YANNAKAKIS_H__
#define __YANNAKAKIS_H__

struct BagNode {
  std::vector<std::unique_ptr<BagNode>> children;
  std::vector<Relation*> relations;
};

#endif

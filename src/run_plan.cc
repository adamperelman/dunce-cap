#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include "../include/picojson.h"

#include "relation.h"
#include "yannakakis.h"
#include "generic_join.h"

using namespace std;

BagNode* BuildPlanSubtree(picojson::value v, string data_file) {
  cout << v.get("relations") << endl;
  BagNode* bag_node = new BagNode();
  for (const picojson::value& rel: v.get("relations").get<picojson::array>()) {
    vector<string> attrs;
    for (const picojson::value& attr : rel.get("attrs").get<picojson::array>()) {
      attrs.push_back(attr.get<string>());
    }
    bag_node->relations.push_back(TrieNode::FromFile(data_file, attrs));
  }
  const picojson::value& children = v.get("children");
  if (children.is<picojson::array>()) {
    for (const picojson::value& child: children.get<picojson::array>()) {
      bag_node->children.push_back(unique_ptr<BagNode>(BuildPlanSubtree(child, data_file)));
    }
  }
  return bag_node;
}

BagNode* BuildPlanTree(string plan_file, string data_file) {
  picojson::value v;
  ifstream ifs(plan_file);
  ifs >> v;
  if (!picojson::get_last_error().empty()) {
    cerr << "error parsing json" << endl;
    exit(1);
  }
  return BuildPlanSubtree(v, data_file);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Usage: ./run_plan.exe <plan file> <data file>" << endl;
    exit(0);
  }

  BagNode* root_bag = BuildPlanTree(argv[1], argv[2]);

  cout << "done building plan tree" << endl;

  cout << "performing join..." << endl;
  typedef chrono::high_resolution_clock Clock;
  typedef chrono::milliseconds ms;

  Clock::time_point start_join = Clock::now();
  TrieNode* result = YannakakisJoin(root_bag);
  Clock::time_point end_join = Clock::now();

  cout << "done performing join" << endl;
  ms join_time = chrono::duration_cast<ms>(end_join - start_join);

  cout << join_time.count() << "ms\n" << endl;
  cout << "size: " << result->size() << endl;

  delete result;
  delete root_bag;
}
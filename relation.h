#ifndef __RELATION_H__
#define __RELATION_H__

#include <string>
#include <vector>

class Relation {

public:
  Relation(const std::string& filename, std::string relation_name, std::vector<std::string> attrs);
  ~Relation() {}

  Relation* Project(std::vector<std::string> attrs); 
  Relation* LeftSemiJoin(Relation relation);
private:
  std::vector<std::string> attrs_;
  std::string relation_name_;
  std::vector<std::vector<int>> tuples_; 
};
#endif /* __DATABASE_H__ */

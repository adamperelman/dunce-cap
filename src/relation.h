#ifndef __RELATION_H__
#define __RELATION_H__

#include <string>
#include <vector>
#include <set>

class Relation {

public:
  Relation(const std::string& filename, std::string relation_name, std::vector<std::string> attrs);
  Relation(std::vector<std::string> attrs);
  Relation(std::vector<std::string> attrs, std::set<std::vector<int>> tuples);
  ~Relation() {}

  static Relation* Intersect(const std::vector<Relation>& relations);

  void AddTuple(std::vector<int> tuple);
  Relation* Project(const std::set<std::string>& attrs) const;
  Relation* LeftSemiJoin(const std::vector<int>& tuple, const std::vector<std::string>& attrs) const;
  Relation* CartesianProduct(const std::vector<int>& tuple, const std::vector<std::string>& attrs) const;

  Relation* SortedByAttributes();

  int size() const;
  bool contains(const std::vector<int>& tuple) const;
  const std::vector<std::string>& attrs() const;

private:
  std::vector<std::string> attrs_;
  std::string relation_name_;
  std::set<std::vector<int>> tuples_;
};
#endif /* __DATABASE_H__ */

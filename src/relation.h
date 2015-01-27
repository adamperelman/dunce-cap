#ifndef __RELATION_H__
#define __RELATION_H__

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <memory>

class Relation {

public:
  Relation(const std::string& filename, std::string relation_name, std::vector<std::string> attrs);
  Relation(std::vector<std::string> attrs);
  Relation(std::vector<std::string> attrs, std::set<std::vector<int>> tuples);
  ~Relation() {}

  static Relation* Intersect(const std::vector<Relation*>& relations);
  static Relation* Union(const std::vector<std::unique_ptr<Relation>>& relations);

  void AddTuple(std::vector<int> tuple);
  Relation* Project(const std::set<std::string>& attrs) const;
  Relation* LeftSemiJoin(const std::vector<int>& tuple, const std::vector<std::string>& attrs) const;
  Relation* LeftSemiJoinAndProject(const std::vector<int>& tuple, const std::vector<std::string>& attrs,const std::set<std::string>& projection_attrs) const;
  Relation* CartesianProduct(const std::vector<int>& tuple, const std::vector<std::string>& attrs) const;
  bool ContainsAttributes(const std::set<std::string>& attrs) const;

  Relation* SortedByAttributes();

  int size() const;
  bool contains(const std::vector<int>& tuple) const;
  const std::vector<std::string>& attrs() const;
  std::string name() const { return relation_name_; }
  const std::set<std::vector<int>>& tuples() const { return tuples_; }

private:
  std::vector<std::string> attrs_;
  std::string relation_name_;
  std::set<std::vector<int>> tuples_;

  void PopulateIncludeAndOrderedProjectedCols(const std::set<std::string>& attrs, std::vector<bool>& include_column, std::vector<std::string>& ordered_projected_columns) const;
};

std::ostream& operator<<(std::ostream& os, const Relation& rel);

#endif /* __DATABASE_H__ */

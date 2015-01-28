#ifndef __RELATION_H__
#define __RELATION_H__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <memory>
#include <boost/functional/hash.hpp>

struct tuple_hash {
  size_t operator()(const std::vector<int>& tuple) const {
    return boost::hash_range(tuple.begin(), tuple.end());
  }
};

typedef std::unordered_set<std::vector<int>, tuple_hash> tuple_set;

class Relation {

public:
  Relation(const std::string& filename, std::string relation_name, std::vector<std::string> attrs);
  Relation(std::vector<std::string> attrs, size_t num_tuples_hint);
  Relation(std::vector<std::string> attrs, tuple_set tuples);
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
  const tuple_set& tuples() const { return tuples_; }

private:
  std::vector<std::string> attrs_;
  std::string relation_name_;
  tuple_set tuples_;


  void PopulateIncludeAndOrderedProjectedCols(const std::set<std::string>& attrs, std::vector<int>& include_column, std::vector<std::string>& ordered_projected_columns) const;
};

std::ostream& operator<<(std::ostream& os, const Relation& rel);

#endif /* __RELATION_H__ */

# dunce-cap

This project uses some libraries from Boost, so to build this, you might need to add boost libraries to your include path (something like this: 'export CPATH=$CPATH:/usr/local/boost_1_57_0').

### TODO:
* implement and benchmark faster pairwise join algorithms (maybe without an index - just sorted merge join outputting a vector of tuples?)
* implement yannakakis with these faster pairwise join algorithms
* think about what the theory says about how GenericJoin should behave on a pairwise join (before/after running a full reducer)
* parallelize generic join (and/or yannakakis) (using OpenMP?)

### Ideas:
* instead of having each node store its values separately, have the first list of values point to locations in a second list of values; this should improve caching when we stream over all tuples
* iterator for TrieNode should take as an optional argument the set of attributes you're interested in
* there are better ways to implement left semi join

### Algorithms we should compare:
* GenericJoin on the whole thing
* Y Reduction, then pairwise joins via hash join
* Y Reduction, then pairwise joins via merge join
* Y Reduction, then pairwise joins via GenericJoin
* Y Reduction, then join all reduced relations together simultaneously via GenericJoin

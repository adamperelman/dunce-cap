# dunce-cap

This project uses some libraries from Boost, so to build this, you might need to add boost libraries to your include path (something like this: 'export CPATH=$CPATH:/usr/local/boost_1_57_0').

IDEAS:
-iterator for TrieNode should take as an optional argument the set of attributes you're interested in
-there are better ways to implement left semi join

Algorithms we should compare:
-GenericJoin on the whole thing
-Y Reduction, then pairwise joins via hash join
-Y Reduction, then pairwise joins via merge join
-Y Reduction, then pairwise joins via GenericJoin
-Y Reduction, then join all reduced relations together simultaneously via GenericJoin

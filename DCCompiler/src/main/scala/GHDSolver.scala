package DCCompiler

import scala.collection.mutable

class Relation(val attrs: List[String]) {
}

class GHDSolver {
  class Node(rels: List[Relation]) {
    var children: Option[List[Node]] = None
    var width: Int = 0
    var fractionalWidth: Double = 0
  }

  def getAttrSet(rels: List[Relation]): Set[String] = {
    val attrSet = mutable.Set[String]()
    for (rel <- rels) {
      for (attr <- rel.attrs) {
        attrSet += attr
      }
    }
    attrSet.toSet
  }

  private def getConnectedComponents(rels: mutable.Set[Relation], comps: List[List[Relation]]): List[List[Relation]] = {
    if (rels.isEmpty) return comps
    val component = getOneConnectedComponent(rels)
    return getConnectedComponents(rels, component::comps)
  }

  private def getOneConnectedComponent(rels: mutable.Set[Relation]): List[Relation] = {
    val curr = rels.head
    rels -= curr
    return DFS(mutable.Set[Relation](curr), curr, rels)
  }

  private def DFS(seen: mutable.Set[Relation], curr: Relation, rels: mutable.Set[Relation]): List[Relation] = {
    for (rel <- rels) {
      // if these two hyperedges are connected
      if (!(curr.attrs.toSet[String] & rel.attrs.toSet[String]).isEmpty) {
        seen += curr
        rels -= curr
        DFS(seen, rel, rels)
      }
    }
    return seen.toList
  }

  def getPartitions(rels: List[Relation], // this can contain chosen
                    chosen: List[Relation],
                    parentAttrs: Set[String],
                    tryBagAttrSet: Set[String]): Option[List[List[Relation]]] = {
    // first we need to check that we will still be able to satisfy
    // the concordance condition in the rest of the subtree
    val leftoverBags = rels.toSet[Relation] &~ chosen.toSet[Relation]
    for (bag <- leftoverBags.toList) {
      if (!(bag.attrs.toSet[String] & parentAttrs).subsetOf(tryBagAttrSet)) {
        return None
      }
    }

    // if the concordance condition is satisfied, figure out what components you just
    // partitioned your graph into, and do ghd on each of those disconnected components
    val relations = mutable.Set[Relation]() ++ rels
    return Some(getConnectedComponents(relations, List[List[Relation]]()))
  }

  def getMinFractionalWidthDecomposition(rels: List[Relation], parentAttrs: Set[String]): List[Node] =  {
    val attrSet = getAttrSet(rels)
    val treesFound = mutable.ListBuffer[Node]()
    for (tryWidth <- (0 to attrSet.size).toList) {
      for (bag <- rels.combinations(tryWidth)) {
        val bag_attr_set = getAttrSet(bag)
        val partitions = getPartitions(rels, bag, parentAttrs, bag_attr_set)
        if (partitions.isDefined) {
          for (partition <- partitions.get) {
            val newNode = new Node(bag)
            val subtreeGHDs = getMinFractionalWidthDecomposition(partition, bag_attr_set)
            newNode.children = Some(subtreeGHDs)
            treesFound.append(newNode)
          }
        } // otherwise, move onto the next bag
      }
    }
    return treesFound.toList
  }
}





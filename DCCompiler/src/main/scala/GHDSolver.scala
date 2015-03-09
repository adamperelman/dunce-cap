package DCCompiler

import java.io.{File, FileWriter, BufferedWriter}

import scala.collection.mutable
import argonaut._, Argonaut._

class Relation(val attrs: List[String]) {
}

class GHDNode(val rels: List[Relation]) {
  var children: Option[List[GHDNode]] = None
  var subtreeWidth: Int = 0
  var subtreeFractionalWidth: Double = 0
  var bagWidth: Int = 0
  var bagFractionalWidth: Double = 0

  override def equals(o: Any) = o match {
    case that: GHDNode => that.rels.equals(rels) && that.children.equals(children)
    case _ => false
  }

  override def hashCode = 41 * rels.hashCode() + children.fold(0)((l: List[GHDNode]) => l.hashCode())

  def getAttrSet(): Set[String] = {
    val attrSet = mutable.Set[String]()
    for (rel <- rels) {
      for (attr <- rel.attrs) {
        attrSet += attr
      }
    }
    attrSet.toSet
  }

  def scoreTree(): Int = {
    bagWidth = getAttrSet().size
    if (!children.isDefined) {
      return bagWidth
    }
    val childrenScore = children.get.map((child: GHDNode) => child.scoreTree()).foldLeft(bagWidth)((accum: Int, x: Int) => if (x > accum) x else accum)
    return childrenScore
  }

  def toJson(): Json = {
    val relationsJson = jArray(rels.map((rel : Relation) => Json("attrs" -> jArray(rel.attrs.map((str: String) => jString(str))))))

    if (children.isDefined) {
      return Json("relations" -> relationsJson, "children" -> jArray(children.get.map((child: GHDNode) => child.toJson())))
    } else {
      return Json("relations" -> relationsJson)
    }
  }
}

object GHDSolver {
  def getAttrSet(rels: List[Relation]): Set[String] = {
    val attrSet = mutable.Set[String]()
    for (rel <- rels) {
      for (attr <- rel.attrs) {
        attrSet += attr
      }
    }
    attrSet.toSet
  }

  private def getConnectedComponents(rels: mutable.Set[Relation], comps: List[List[Relation]], ignoreAttrs: Set[String]): List[List[Relation]] = {
    if (rels.isEmpty) return comps
    val component = getOneConnectedComponent(rels, ignoreAttrs)
    return getConnectedComponents(rels, component::comps, ignoreAttrs)
  }

  private def getOneConnectedComponent(rels: mutable.Set[Relation], ignoreAttrs: Set[String]): List[Relation] = {
    val curr = rels.head
    rels -= curr
    return DFS(mutable.LinkedHashSet[Relation](curr), curr, rels, ignoreAttrs)
  }

  private def DFS(seen: mutable.Set[Relation], curr: Relation, rels: mutable.Set[Relation], ignoreAttrs: Set[String]): List[Relation] = {
    for (rel <- rels) {
      // if these two hyperedges are connected
      if (!((curr.attrs.toSet[String] & rel.attrs.toSet[String]) &~ ignoreAttrs).isEmpty) {
        seen += curr
        rels -= curr
        DFS(seen, rel, rels, ignoreAttrs)
      }
    }
    return seen.toList
  }

  def getPartitions(leftoverBags: List[Relation], // this cannot contain chosen
                    chosen: List[Relation],
                    parentAttrs: Set[String],
                    tryBagAttrSet: Set[String]): Option[List[List[Relation]]] = {
    // first we need to check that we will still be able to satisfy
    // the concordance condition in the rest of the subtree
    for (bag <- leftoverBags.toList) {
      if (!(bag.attrs.toSet[String] & parentAttrs).subsetOf(tryBagAttrSet)) {
        return None
      }
    }

    // if the concordance condition is satisfied, figure out what components you just
    // partitioned your graph into, and do ghd on each of those disconnected components
    val relations = mutable.LinkedHashSet[Relation]() ++ leftoverBags
    return Some(getConnectedComponents(relations, List[List[Relation]](), getAttrSet(chosen).toSet[String]))
  }

  /**
   * @param partitions
   * @param parentAttrs
   * @return Each list in the returned list could be the children of the parent that we got parentAttrs from
   */
  def getListsOfPossibleSubtrees(partitions: List[List[Relation]], parentAttrs: Set[String]): List[List[GHDNode]] = {
    assert(!partitions.isEmpty)
    val subtreesPerPartition: List[List[GHDNode]] = partitions.map((l: List[Relation]) => getMinFractionalWidthDecomposition(l, parentAttrs))
    val foldFunc: (List[List[GHDNode]], List[GHDNode]) => List[List[GHDNode]]
      = (accum: List[List[GHDNode]], subtreesForOnePartition: List[GHDNode]) => subtreesForOnePartition.map(
      (subtreeForOnePartition: GHDNode) => accum.map((children: List[GHDNode]) => subtreeForOnePartition::children).flatten)
    return subtreesPerPartition.foldLeft(List[List[GHDNode]](List[GHDNode]()))(foldFunc)
  }

  def getMinFractionalWidthDecomposition(rels: List[Relation], parentAttrs: Set[String]): List[GHDNode] =  {
    val attrSet = getAttrSet(rels)
    val treesFound = mutable.ListBuffer[GHDNode]()
    for (tryNumRelationsTogether <- (1 to rels.size).toList) {
      for (bag <- rels.combinations(tryNumRelationsTogether).toList) {
        val leftoverBags = rels.toSet[Relation] &~ bag.toSet[Relation]
        if (leftoverBags.toList.isEmpty) {
          val newNode = new GHDNode(bag)
          treesFound.append(newNode)
        } else {
          val bagAttrSet = getAttrSet(bag)
          val partitions = getPartitions(leftoverBags.toList, bag, parentAttrs, bagAttrSet)
          if (partitions.isDefined) {
            // lists of possible children for |bag|
            val possibleSubtrees: List[List[GHDNode]] = getListsOfPossibleSubtrees(partitions.get, bagAttrSet)
            for (subtrees <- possibleSubtrees) {
              val newNode = new GHDNode(bag)
              newNode.children = Some(subtrees)
              treesFound.append(newNode)
            }
          }
        }
      }
    }
    return treesFound.toList
  }
}









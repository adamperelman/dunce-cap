package DCCompiler

import java.util

import argonaut.Argonaut._
import argonaut.Json
import org.apache.commons.math3.optim.linear._
import org.apache.commons.math3.optim.nonlinear.scalar.GoalType

import scala.collection.mutable

/**
 * Created by sctu on 3/9/15.
 */
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

  def getMatrixRow(attr : String, rels : List[Relation]): Array[Double] = {
    val presence = rels.map((rel : Relation) => if (rel.attrs.toSet.contains(attr)) 1.0 else 0)
    return presence.toArray
  }

  def fractionalScoreNode(): Double = { // TODO: catch UnboundedSolutionException
    val bagAttrs = getAttrSet().toList
    // objective:
    val objective = new LinearObjectiveFunction(rels.map((rel : Relation) => 1.0).toArray, 0)
    // constraints:
    val constraintList = new util.ArrayList[LinearConstraint]
    bagAttrs.map((attr : String) => constraintList.add(new LinearConstraint(getMatrixRow(attr, rels), Relationship.GEQ,  1.0)))
    val constraints = new LinearConstraintSet(constraintList)
    val solver = new SimplexSolver
    val solution = solver.optimize(objective, constraints, GoalType.MINIMIZE, new NonNegativeConstraint(true))
    println(solution.getValue)
    return solution.getValue
  }

  def fractionalScoreTree() : Double = {
    bagFractionalWidth = fractionalScoreNode()
    if (!children.isDefined) {
      return bagFractionalWidth
    }
    val childrenScore = children.get.map((child: GHDNode) => child.fractionalScoreTree())
      .foldLeft(bagFractionalWidth)((accum: Double, x: Double) => if (x > accum) x else accum)
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

package DCCompiler

import java.util
import scala.collection.immutable.TreeSet

import argonaut.Argonaut._
import argonaut.Json
import org.apache.commons.math3.optim.linear._
import org.apache.commons.math3.optim.nonlinear.scalar.GoalType

class Relation(val attrs: List[String]) {
}

class GHDNode(val rels: List[Relation]) {
  val attrSet = rels.foldLeft(TreeSet[String]())(
    (accum: TreeSet[String], rel : Relation) => accum | TreeSet[String](rel.attrs : _*))
  var children: List[GHDNode] = List()
  var subtreeWidth: Int = 0
  var subtreeFractionalWidth: Double = 0
  var bagWidth: Int = 0
  var bagFractionalWidth: Double = 0
  var attributeReordering : Option[Map[String, Int]] = None
  final val ATTRIBUTE_NUMBERING_START = 0

  override def equals(o: Any) = o match {
    case that: GHDNode => that.rels.equals(rels) && that.children.equals(children)
    case _ => false
  }

  override def hashCode = 41 * rels.hashCode() + children.hashCode()

  def scoreTree(): Int = {
    bagWidth = attrSet.size
    return children.map((child: GHDNode) => child.scoreTree()).foldLeft(bagWidth)((accum: Int, x: Int) => if (x > accum) x else accum)
  }

  private def getMatrixRow(attr : String, rels : List[Relation]): Array[Double] = {
    val presence = rels.map((rel : Relation) => if (rel.attrs.toSet.contains(attr)) 1.0 else 0)
    return presence.toArray
  }

  private def fractionalScoreNode(): Double = { // TODO: catch UnboundedSolutionException
    val objective = new LinearObjectiveFunction(rels.map((rel : Relation) => 1.0).toArray, 0)
    // constraints:
    val constraintList = new util.ArrayList[LinearConstraint]
    attrSet.map((attr : String) => constraintList.add(new LinearConstraint(getMatrixRow(attr, rels), Relationship.GEQ,  1.0)))
    val constraints = new LinearConstraintSet(constraintList)
    val solver = new SimplexSolver
    val solution = solver.optimize(objective, constraints, GoalType.MINIMIZE, new NonNegativeConstraint(true))
    return solution.getValue
  }

  def fractionalScoreTree() : Double = {
    bagFractionalWidth = fractionalScoreNode()
    return children.map((child: GHDNode) => child.fractionalScoreTree())
      .foldLeft(bagFractionalWidth)((accum: Double, x: Double) => if (x > accum) x else accum)
  }

  def toJson(): Json = {
    val relationsJson = jArray(rels.map((rel : Relation) => Json("attrs" -> jArray(rel.attrs.map((str: String) => jString(str))))))
    if (!children.isEmpty) {
      return Json("relations" -> relationsJson, "children" -> jArray(children.map((child: GHDNode) => child.toJson())))
    } else {
      return Json("relations" -> relationsJson)
    }
  }

  private def getAttributeReordering(reordering : Map[String, Int], newAttrName : Int): (Map[String, Int], Int) = {
    val newAttrs : Set[String] = attrSet.filter((attr : String) => !reordering.contains(attr))
    val (newReordering, updatedNewAttrName) = newAttrs.foldLeft(
        (reordering, newAttrName))((accum: (Map[String, Int], Int), origAttrName: String) => (accum._1 + (origAttrName -> accum._2), accum._2 +1))
    return children.foldLeft(
      (newReordering, updatedNewAttrName))((accum: (Map[String, Int], Int), child : GHDNode) => child.getAttributeReordering(
        accum._1, accum._2))
  }

  def reorderAttributes() = {
    attributeReordering = Some(getAttributeReordering(Map(), ATTRIBUTE_NUMBERING_START)._1)
  }

  private def translateAttribute(attr : String) = {
    attributeReordering.fold(attr)((reordering: Map[String, Int]) => "attr_" + reordering.get(attr).toString)
  }
}

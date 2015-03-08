package DCCompiler

import org.scalatest.FunSuite

/**
 * Created by sctu on 3/4/15.
 */
class GHDSolverTest extends FunSuite {
  final val RELATIONS: List[Relation] = List(
    new Relation(List("a", "b", "c")),
    new Relation(List("g", "a")),
    new Relation(List("c", "d", "e")),
    new Relation(List("e", "f")))
  final val PATH2: List[Relation] = List(
    new Relation(List("a", "b")),
    new Relation(List("b", "c")))
  final val TADPOLE: List[Relation] = List(
    new Relation(List("a", "b")),
    new Relation(List("b", "c")),
    new Relation(List("c", "a")),
    new Relation(List("a", "e")))
  final val solver: GHDSolver = new GHDSolver

  test("Can identify connected components of graph when removing the chosen hyper edge leaves 2 disconnected components") {
    val chosen = List(RELATIONS.head)
    val partitions: Option[List[List[Relation]]] = solver.getPartitions(
      RELATIONS.tail, chosen, Set(), solver.getAttrSet(chosen))
    assert(partitions.isDefined)
    assert(partitions.get.size == 2)

    val firstPart = partitions.get.head
    val secondPart = partitions.get.tail.head
    assert(firstPart.size == 2 && secondPart.size == 1)
    assert(secondPart.head == RELATIONS(1))
    assert(firstPart.head == RELATIONS(2))
    assert(firstPart.tail.head == RELATIONS(3))
  }

  test("Finds all possible decompositions of len 2 path query)") {
    val decompositions = solver.getMinFractionalWidthDecomposition(PATH2, Set[String]()).toSet[GHDNode]
    /*
     * The decompositions we expect are [ABC] and [AB]--[BC] and [BC]--[AB]
     */
    assert(decompositions.size == 3)
    val singleBag = new GHDNode(PATH2)
    val twoBagWithRootAB = new GHDNode(PATH2.take(1))
    twoBagWithRootAB.children = Some(List(new GHDNode(PATH2.tail.take(1))))
    val twoBagWithRootBC = new GHDNode(PATH2.tail.take(1))
    twoBagWithRootAB.children = Some(List(new GHDNode(PATH2.take(1))))
    assert(decompositions.contains(singleBag))
    assert(decompositions.contains(twoBagWithRootAB))
    assert(decompositions.contains(twoBagWithRootBC))
  }
}

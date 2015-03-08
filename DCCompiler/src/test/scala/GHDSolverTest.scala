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
    /**
     * The decompositions we expect are [ABC] and [AB]--[BC] and [BC]--[AB]
     */
    assert(decompositions.size == 3)
    val singleBag = new GHDNode(PATH2)
    val twoBagWithRootAB = new GHDNode(PATH2.take(1))
    twoBagWithRootAB.children = Some(List(new GHDNode(PATH2.tail.take(1))))
    val twoBagWithRootBC = new GHDNode(PATH2.tail.take(1))
    twoBagWithRootBC.children = Some(List(new GHDNode(PATH2.take(1))))
    assert(decompositions.contains(singleBag))
    assert(decompositions.contains(twoBagWithRootAB))
    assert(decompositions.contains(twoBagWithRootBC))
  }

  test("Finds all possible decompositions of tadpole query)") {
    val decompositions = solver.getMinFractionalWidthDecomposition(TADPOLE, Set[String]())
    assert(decompositions.size == 27)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 1).size == 14)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 2).size == 8)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 3).size == 4)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 4).size == 1)
    val decompositionsSet = decompositions.toSet[GHDNode]
    /**
     * The decompositions we expect are
     * [AB]--[ABC]--[AE] (*)
     * [AC]--[ABC]--[AE]
     * [BC]--[ABC]--[AE]
     * [AB]--[ABCE]
     * [AC]--[ABCE]
     * [BC]--[ABCE]
     * [AC]--[ABCE]--[AB]
     * [AE]--[AB]--[ABC]
     *
     * [ABC]--[AE]
     * [ABE]--[ABC] (*)
     * [ACE]--[ABC]
     * [ABCE]--[ABC]
     *
     * all of the above also work if you invert the tree
     *
     * [ABCE] (*)
     *
     * Check that the ones marked (*) were found:
     */
    val decomp1 = new GHDNode(List(TADPOLE(0)))
    val decomp1Child = new GHDNode(List(TADPOLE(1), TADPOLE(2)))
    val decomp1GrandChild = new GHDNode(List(TADPOLE(3)))
    decomp1Child.children = Some(List(decomp1GrandChild))
    decomp1.children = Some(List(decomp1Child))
    assert(decompositionsSet.contains(decomp1))

    val decomp2 = new GHDNode(List(TADPOLE(0), TADPOLE(3)))
    val decomp2Child = new GHDNode(List(TADPOLE(1), TADPOLE(2)))
    decomp2.children = Some(List(decomp2Child))
    assert(decompositionsSet.contains(decomp2))

    val decomp3 = new GHDNode(List(TADPOLE(0), TADPOLE(1), TADPOLE(2), TADPOLE(3)))
    assert(decompositionsSet.contains(decomp3))
  }
}

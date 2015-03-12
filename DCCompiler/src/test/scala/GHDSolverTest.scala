package DCCompiler

import org.scalatest.FunSuite

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
  final val SPLIT: List[Relation] = List(
    new Relation(List("b", "c", "d", "z", "y")),
    new Relation(List("c", "d", "e", "i", "j")),
    new Relation(List("b", "d", "f", "g", "h")),
    new Relation(List("f", "g", "h", "k", "b")),
    new Relation(List("f", "g", "h", "n", "b")))
  final val solver = GHDSolver

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
    val decompositions = solver.getMinFractionalWidthDecomposition(PATH2).toSet[GHDNode]
    /**
     * The decompositions we expect are [ABC] and [AB]--[BC] and [BC]--[AB]
     */
    assert(decompositions.size == 3)
    val singleBag = new GHDNode(PATH2)
    val twoBagWithRootAB = new GHDNode(PATH2.take(1))
    twoBagWithRootAB.children = List(new GHDNode(PATH2.tail.take(1)))
    val twoBagWithRootBC = new GHDNode(PATH2.tail.take(1))
    twoBagWithRootBC.children = List(new GHDNode(PATH2.take(1)))
    assert(decompositions.contains(singleBag))
    assert(decompositions.contains(twoBagWithRootAB))
    assert(decompositions.contains(twoBagWithRootBC))
  }

  test("Decomps and scores triangle query correctly") {
    val decompositions = solver.getMinFractionalWidthDecomposition(TADPOLE.take(3)) // drop the tail
    /**
     * The decompositions we expect are
     * [ABC]
     * [any one rel] -- [other two rels] (this can be inverted)
     */
    assert(decompositions.size == 7)
    val fractionalScores = decompositions.map((root: GHDNode) => root.fractionalScoreTree())
    assert(fractionalScores.min === 1.5)
  }

  test("Find max bag size 5 decomposition of query") {
    val decompositions2 = solver.getMinFractionalWidthDecomposition(SPLIT)
    assert(!decompositions2.filter((root: GHDNode) => root.scoreTree <= 5).isEmpty)
  }

  test("Finds all possible decompositions of tadpole query)") {
    val decompositions = solver.getMinFractionalWidthDecomposition(TADPOLE)
    assert(decompositions.size == 21)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 1).size == 10)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 2).size == 6)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 3).size == 4)
    assert(decompositions.filter((root: GHDNode) => root.rels.size == 4).size == 1)
    val decompositionsSet = decompositions.toSet[GHDNode]
    /**
     * The decompositions we expect are
     * [AB]--[ABC] (*)
     *  |
     * [AE]
     *
     * root of above tree could also be AC
     *
     * [AB]--[ABCE]
     * [AC]--[ABCE]
     * [BC]--[ABCE]
     *
     * [ABC]--[AE]
     * [ABE]--[ABC] (*)
     * [ACE]--[ABC]
     * [AEBC]--[ABC]
     *
     * all of the above 2-node options also work if you switch the root and leaf
     *
     * [AE]--[AB]--[ABC]
     * [AE]--[ABC]--[AB]
     * [BC]--[ABC]--[AE]
     * [AE]--[ABC]--[BC]
     *
     * [ABCE] (*)
     *
     * Check that the ones marked (*) were found:
     */
    val decomp1 = new GHDNode(List(TADPOLE(0)))
    val decomp1Child1 = new GHDNode(List(TADPOLE(1), TADPOLE(2)))
    val decomp1Child2 = new GHDNode(List(TADPOLE(3)))
    decomp1.children = List(decomp1Child1, decomp1Child2)
    assert(decompositionsSet.contains(decomp1))

    val decomp2 = new GHDNode(List(TADPOLE(0), TADPOLE(3)))
    val decomp2Child = new GHDNode(List(TADPOLE(1), TADPOLE(2)))
    decomp2.children = List(decomp2Child)
    assert(decompositionsSet.contains(decomp2))

    val decomp3 = new GHDNode(List(TADPOLE(0), TADPOLE(1), TADPOLE(2), TADPOLE(3)))
    assert(decompositionsSet.contains(decomp3))

    // Also check that we found the lowest fhw option
    val decomp4 = new GHDNode(List(TADPOLE(3)))
    val decomp4Child = new GHDNode(List(TADPOLE(0), TADPOLE(1), TADPOLE(2)))
    decomp4.children = List(decomp4Child)
    assert(decompositionsSet.contains(decomp4))

    val fractionalScores = decompositions.map((root: GHDNode) => root.fractionalScoreTree())
    assert(fractionalScores.min === 1.5)
  }
}

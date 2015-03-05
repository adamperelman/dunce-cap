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

  test("Can use identify connected components of graph") {
    val solver: GHDSolver = new GHDSolver
    val chosen = List(RELATIONS.head)
    val partitions: Option[List[List[Relation]]] = solver.getPartitions(
      RELATIONS.tail, chosen, Set(), solver.getAttrSet(chosen))
    assert(partitions.isDefined)
    assert(partitions.get.size == 2)

    val firstPart = partitions.get.head
    val secondPart = partitions.get.tail.head
    assert((firstPart.size == 2 && secondPart.size == 1) || (firstPart.size == 1 && secondPart.size == 2))
  }
}

package DCCompiler

import java.io.{FileWriter, BufferedWriter, File}

/**
 * Created by sctu on 3/9/15.
 */
object GHDWriter {
  def main(args: Array[String]): Unit = {
    val solver = GHDSolver
    val TADPOLE: List[Relation] = List(
      new Relation(List("a", "b")),
      new Relation(List("b", "c")),
      new Relation(List("c", "a")),
      new Relation(List("a", "e")))
    val decompositions = solver.getMinFractionalWidthDecomposition(TADPOLE, Set[String]())
    decompositions.zipWithIndex.map({case (root, i) => print(root, "../query_plans/tadpole_generated" + i+ ".json")})
  }

  def print(root: GHDNode, filename: String) = {
    val json = root.toJson()
    val file = new File(filename)
    val bw = new BufferedWriter(new FileWriter(file))
    bw.append(json.spaces2)
    bw.close()
  }
}
package DCCompiler

import java.io.{FileWriter, BufferedWriter, File}

object GHDWriter {
  def main(args: Array[String]): Unit = {
    val solver = GHDSolver
    val TADPOLE: List[Relation] = List(
      new Relation(List("a", "b")),
      new Relation(List("b", "c")),
      new Relation(List("c", "a")),
      new Relation(List("a", "e")))
    val decompositions = solver.getMinFractionalWidthDecomposition(TADPOLE)
    val fhws = decompositions.map((root : GHDNode) => root.fractionalScoreTree())
    decompositions.map((root : GHDNode) => root.reorderAttributes())
    decompositions.zip(fhws).map({case (root, fhw) => print(root, "../query_plans/gen/tadpole_generated" + fhw + ".json")})
  }

  def print(root: GHDNode, filename: String) = {
    val json = root.toJson()
    val file = new File(filename)
    val bw = new BufferedWriter(new FileWriter(file))
    bw.append(json.spaces2)
    bw.close()
  }
}
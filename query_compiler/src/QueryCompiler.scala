/**
 * Created by adamperelman on 3/9/15.
 */


class Relation(val attrs: List[String]) {
  override def toString(): String = {
    attrs.toString()
  }
}

object QueryCompiler extends App {
  def emitGenericJoinCode(attrs : List[String], relations : List[Relation]): Unit = {
    println(relations)
  }

  override def main (args: Array[String]) {
    val relations = List(
      new Relation(List("a", "b")),
      new Relation(List("b", "c")),
      new Relation(List("a", "c"))
    )

    val attrs = List("a", "b", "c")

    emitGenericJoinCode(attrs, relations)
  }
}

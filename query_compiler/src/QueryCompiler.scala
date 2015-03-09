/**
 * Created by adamperelman on 3/9/15.
 */

import java.io._


class Relation(val attrs: List[String]) {
  override def toString(): String = {
    attrs.toString()
  }
}

object QueryCompiler extends App {

  def loadingCode(relations : Map[String, Relation]) : String = {
    val data_file = "\"data/facebook_bidirectional.txt\""
    var result = ""
    for ((rel_name, rel) <- relations) {
      result += s"TrieNode* $rel_name = TrieNode::FromFile($data_file, {${rel.attrs.map(a => "\"" + a + "\"").mkString(",")}});\n"
    }
    result
  }

  def generateCode(attrs : List[String], relations : Map[String, Relation]): String = {
    val Preamble : String =
      """
      |#include <chrono>
      |#include "relation.h"
      |#include "yannakakis.h"
      |#include "generic_join.h"
      |
      |using namespace std;
      |
      |int main(int argc, char* argv[]) {
      |
      |vector<const TrieNode*> relations;
      |
      |cout << "loading files..." << endl;
      |""".stripMargin

    val PerformJoin =
      """
      |typedef chrono::high_resolution_clock Clock;
      |typedef chrono::milliseconds ms;
      |
      |cout << "performing join..." << endl;
      |
      |Clock::time_point start_join = Clock::now();
      |int result = GenericJoinCount(relations);
      |Clock::time_point end_join = Clock::now();
      |
      |cout << "done performing join" << endl;
      |ms join_time = chrono::duration_cast<ms>(end_join - start_join);
      |
      |cout << join_time.count() << "ms\n" << endl;
      |cout << "size: " << result << endl;
      |""".stripMargin

    Preamble + loadingCode(relations) + PerformJoin + "}"
  }

  override def main (args: Array[String]) {
    val relations = Map(
      "r1" -> new Relation(List("a", "b")),
      "r2" -> new Relation(List("b", "c")),
      "r3" -> new Relation(List("a", "c"))
    )

    val attrs = List("a", "b", "c")

    val writer = new PrintWriter(args(0))
    writer.write(generateCode(attrs, relations))
    writer.close()
    println("done generating code")
  }
}

package com.ides_lang

/**
 * Created by sedwards on 7/6/14.
 */
object Idesc {
  def main(args: Array[String]) {
    println(syntax.Parser.parseFile("public trait Test[X, Y](a: Arg) : Blah { }"))
  }
}

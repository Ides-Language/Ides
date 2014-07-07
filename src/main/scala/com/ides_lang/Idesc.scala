package com.ides_lang

import com.ides_lang.syntax.{Scanner, Parser}

import scala.io.Source

/**
 * Created by sedwards on 7/6/14.
 */
object Idesc {
  def main(args: Array[String]) {
    val result = Parser.file(new Parser.lexical.Scanner(Source.fromFile("stdlib/test.ides").getLines().mkString("\n")))
    println(result)
  }
}

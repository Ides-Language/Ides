package com.ides_lang.test

import com.ides_lang.syntax._
import org.scalatest._
import scala.util.parsing.combinator.syntactical._

/**
 * Created by sedwards on 8/31/14.
 */
abstract class IdesSpec extends FunSpec {
  def assertFileSuccess(source: String, expr: Expr) {
    expr match {
      case expr: ExprList => assertParseSuccess(Parser.parse(Parser.file, source), expr)
      case expr: Expr => assertParseSuccess(Parser.parse(Parser.file, source), ExprList(expr))
    }
  }

  def assertParseSuccess[T](parser: Parser.Parser[T], source: String, expr: T) {
    assertParseSuccess[T](Parser.parse[T](parser, source), expr)
  }

  def assertParseSuccess[T](result : Parser.ParseResult[T], expr: T) {
    result match {
      case s : Parser.Success[_] => assert(s.get == expr, s.toString())
      case s : Parser.NoSuccess => fail(s.toString)
      case s => fail("Unknown result: " + s.toString)
    }
  }

  def assertParseError[T](parser: Parser.Parser[T], source: String, msg: String) {
    assertParseError[T](Parser.parse[T](parser, source), msg)
  }

  def assertParseError[T](result : Parser.ParseResult[T], msg: String) {
    result match {
      case s : Parser.Error => assert(s.msg == msg)
      case s : Parser.Failure => fail("Failure, not error: " + s.toString())
      case s : Parser.Success[_] => fail("Parse succeeded: " + s.toString)
      case s => fail("Unknown result: " + s.toString)
    }
  }

  def assertParseFailure[T](parser: Parser.Parser[T], source: String, msg: String) {
    assertParseFailure[T](Parser.parse[T](parser, source), msg)
  }

  def assertParseFailure[T](result : Parser.ParseResult[T], msg: String) {
    result match {
      case s : Parser.Failure => assert(s.msg == msg)
      case s : Parser.Error => fail("Error, not failure: " + s.toString())
      case s : Parser.Success[_] => fail("Parse succeeded: " + s.toString)
      case s => fail("Unknown result: " + s.toString)
    }
  }

  def parse[T](parser: Parser.Parser[T], source: String) : T = {
    Parser.parse[T](parser, source) match {
      case s : Parser.Success[_] => s.get.asInstanceOf[T]
      case s : Parser.NoSuccess => fail(s.toString)
      case s => fail("Unknown result: " + s.toString)
    }
  }
}

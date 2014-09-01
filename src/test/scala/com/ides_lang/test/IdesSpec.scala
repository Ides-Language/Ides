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
    assertParseSuccess[T](Parser.parse[T](parser.! <~ Parser.EOI, source), expr)
  }

  def assertParseSuccess[T](result : Parser.ParseResult[T], expr: T) {
    result match {
      case s : Parser.Success[Expr] => assert(s.get == expr, s.toString())
      case s : Parser.Error => fail(s.toString())
      case s : Parser.Failure => fail(s.toString())
      case s => fail("Unknown result: " + s.toString)
    }
  }

  def assertError(result : Parser.ParseResult[Expr], msg: String) {
    result match {
      case s : Parser.Error => assert(s.msg == msg)
      case s : Parser.Failure => fail("Failure, not error: " + s.toString())
      case s : Parser.Success[Expr] => fail("Parse succeeded: " + s.toString)
      case s => fail("Unknown result: " + s.toString)
    }
  }

  def assertFailure(result : Parser.ParseResult[Expr], msg: String) {
    result match {
      case s : Parser.Failure => assert(s.msg == msg)
      case s : Parser.Error => fail("Error, not failure: " + s.toString())
      case s : Parser.Success[Expr] => fail("Parse succeeded: " + s.toString)
      case s => fail("Unknown result: " + s.toString)
    }
  }
}

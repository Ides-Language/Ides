package com.ides_lang.test

import com.ides_lang.syntax._
import org.scalatest._

/**
 * Created by sedwards on 8/31/14.
 */
abstract class IdesSpec extends FunSpec {
  def assertParseResult(source: String, expr: Expr) {
    expr match {
      case expr: ExprList => assertParseResult(Parser.parseFile(source), expr)
      case expr: Expr => assertParseResult(Parser.parseFile(source), ExprList(expr))
    }
  }

  def assertParseResult(result : Parser.ParseResult[Expr], expr: Expr) {
    result match {
      case s : Parser.Success[Expr] => assert(s.get == expr, s.toString())
      case s : Parser.Error => fail(s.toString())
      case s : Parser.Failure => fail(s.toString())
      case s => fail("Unknown result: " + s.toString)
    }
  }
}

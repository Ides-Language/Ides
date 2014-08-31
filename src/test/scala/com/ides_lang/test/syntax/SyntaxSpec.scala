package com.ides_lang.test.syntax

import com.ides_lang.syntax._
import com.ides_lang.test.IdesSpec
import org.scalatest._

/**
 * Created by sedwards on 8/31/14.
 */
class SyntaxSpec extends IdesSpec {
  describe("The parser") {
    val quals = Seq("public", "private", "protected", "internal")
    val qualVals = Map("public" -> QualExpr.Public, "private" -> QualExpr.Private, "protected" -> QualExpr.Protected, "internal" -> QualExpr.Internal)

    val varTypes = Seq("val", "var")
    val vtr = Map("val" -> ValDecl, "var" -> VarDecl)

    describe("when parsing variables") {

      varTypes.map { vt =>
        describe(s"${vt}s") {
          it("should parse type inference") {
            assertParseResult(s"$vt x = y", vtr(vt)(QualExpr.None, Name("x"), None, Some(Ident("y"))))
          }

          it("should parse type specification") {
            assertParseResult(s"$vt x : y", vtr(vt)(QualExpr.None, Name("x"), Some(Name("y")), None))
          }

          it("should parse type+initializer specification") {
            assertParseResult(s"$vt x : y = z", vtr(vt)(QualExpr.None, Name("x"), Some(Name("y")), Some(Ident("z"))))
          }

          it("should fail parsing on syntax error") {
            val source = Parser.parseFile(s"$vt {}")

            assert(source match {
              case s: Parser.Error => true
              case _ => false
            })
          }

          describe("qualifier parsers") {
            quals.map { qual =>
              it(s"should parse $qual qualifier") {
                assertParseResult(s"$qual $vt x", vtr(vt)(qualVals(qual), Name("x"), None, None))
              }
            }
          }
        }
      }
    }

    describe ("when parsing functions") {
      it("should parse a basic function") {
        assertParseResult("def f = x", FnDecl(QualExpr.None, Name("f"), ExprList(), None, Some(Ident("x"))))
      }

      it ("should parse a function with an argument") {
        assertParseResult("def f(x: Y) = z", FnDecl(QualExpr.None, Name("f"), ExprList(
          ArgDecl(QualExpr.None, Ident("x"), Some(Name("Y")), None)
        ), None, Some(Ident("z"))))
      }

      it("should parse a function with a return type") {
        assertParseResult("def f : X = y", FnDecl(QualExpr.None, Name("f"), ExprList(), Some(Name("X")), Some(Ident("y"))))
      }

      describe("qualifier parsers") {
        quals.map { qual =>
          it(s"should parse a $qual function") {
            assertParseResult(s"$qual def f = x", FnDecl(qualVals(qual), Name("f"), ExprList(), None, Some(Ident("x"))))
          }
        }
      }
    }
  }
}

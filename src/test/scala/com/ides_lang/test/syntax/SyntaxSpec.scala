package com.ides_lang.test.syntax

import com.ides_lang.syntax._
import com.ides_lang.syntax.Implicits._
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

    describe("when parsing constants") {
      it("should parse empty strings") {
        assertParseSuccess(Parser.constant, "\"\"", ConstantString(""))
      }

      it("should parse \"foobar\" as string") {
        assertParseSuccess(Parser.constant, "\"foobar\"", ConstantString("foobar"))
      }

      it("should parse escape-quoted string") {
        assertParseSuccess(Parser.constant, "\"foo\\\"bar\"", ConstantString("foo\\\"bar"))
      }

      it("should parse 'x' as char") {
        assertParseSuccess(Parser.constant, "'x'", ConstantChar('x'))
      }

      it("should parse escape-quoted char") {
        assertParseSuccess(Parser.constant, "'\\''", ConstantChar('\''))
      }

      it("should parse 0 as int") {
        assertParseSuccess(Parser.constant, "0", ConstantInt(0))
      }

      it("should parse 0.0 as decimal") {
        assertParseSuccess(Parser.constant, "0.0", ConstantDec(0.0))
      }

      it("should parse 0xFF as hex") {
        assertParseSuccess(Parser.constant, "0xFF", ConstantInt(255))
      }

      it("should parse 0b11 as binary") {
        assertParseSuccess(Parser.constant, "0b11", ConstantInt(3))
      }

      it("should parse 010 as octal") {
        assertParseSuccess(Parser.constant, "010", ConstantInt(8))
      }

      it("should parse true") {
        assertParseSuccess(Parser.constant, "true", ConstantBool(v = true))
      }

      it("should parse false") {
        assertParseSuccess(Parser.constant, "false", ConstantBool(v = false))
      }

      it("should parse placeholders") {
        assertParseSuccess(Parser.placeholder, ":1", PlaceholderExpr(1))
      }

      it("should catch unclosed strings") {
        assertParseError(Parser.constant, "\"", "unclosed string literal")
      }

      it("should catch unclosed chars") {
        assertParseError(Parser.constant, "'", "unclosed char literal")
      }
    }

    describe("when parsing names") {
      it("should parse empty type args") {
        assertParseSuccess(Parser.name, "T[]", Name("T", ExprList()))
      }

      it("should parse 1 type arg") {
        assertParseSuccess(Parser.name, "T[I]", Name("T", ExprList("I")))
      }

      it("should parse 2 type args") {
        assertParseSuccess(Parser.name, "T[I, J]", Name("T", ExprList("I", "J")))
      }

      it("should parse 3 type args") {
        assertParseSuccess(Parser.name, "T[I, J, K]", Name("T", ExprList("I", "J", "K")))
      }

      it("should parse exprs in type args") {
        assertParseSuccess(Parser.name, "T[I < J]", Name("T", ExprList(InfixExpr("<", "I", "J"))))
      }
    }

    describe("when parsing variable delcarations") {
      varTypes.map { vt =>
        describe(s"${vt}s") {
          it("should parse type inference") {
            assertFileSuccess(s"$vt x = y", vtr(vt)(QualExpr.None, Name("x"), None, Some("y")))
          }

          it("should parse type specification") {
            assertFileSuccess(s"$vt x : y", vtr(vt)(QualExpr.None, Name("x"), Some(Name("y")), None))
          }

          it("should parse type+initializer specification") {
            assertFileSuccess(s"$vt x : y = z", vtr(vt)(QualExpr.None, Name("x"), Some(Name("y")), Some("z")))
          }

          it("should fail parsing on syntax error") {
            assertParseError(Parser.parseFile(s"$vt {}"), "identifier expected")
          }

          describe("qualifier parsers") {
            quals.map { qual =>
              it(s"should parse $qual qualifier") {
                assertFileSuccess(s"$qual $vt x", vtr(vt)(qualVals(qual), Name("x"), None, None))
              }
            }
          }
        }
      }
    }

    describe ("when parsing function declarations") {
      it("should parse a basic function") {
        assertFileSuccess("def f = x", FnDecl(QualExpr.None, Name("f"), ExprList(), None, Some("x")))
      }

      it ("should parse a function with an argument") {
        assertFileSuccess("def f(x: Y) = z", FnDecl(QualExpr.None, Name("f"), ExprList(
          ArgDecl(QualExpr.None, "x", Some(Name("Y")), None)
        ), None, Some("z")))
      }

      it("should parse a function with a return type") {
        assertFileSuccess("def f : X = y", FnDecl(QualExpr.None, Name("f"), ExprList(), Some(Name("X")), Some("y")))
      }

      it("should require a type or body parse a function") {
        assertParseError(Parser.fn_decl, "def f()", "end of input")
      }

      describe("qualifier parsers") {
        quals.map { qual =>
          it(s"should parse a $qual function") {
            assertFileSuccess(s"$qual def f = x", FnDecl(qualVals(qual), Name("f"), ExprList(), None, Some("x")))
          }
        }
      }
    }

    describe ("when parsing module declarations") {
      it("should parse an empty module") {
        assertFileSuccess("mod x { }", ModDecl(QualExpr.None, "x", ExprList()))
      }

      it("should parse a module with a basic function") {
        assertFileSuccess("mod x { def f = y }", ModDecl(QualExpr.None, "x", ExprList(FnDecl(QualExpr.None, Name("f"), ExprList(), None, Some("y")))))
      }
    }

    describe ("when parsing infix expressions") {
      describe("order of operations") {
        it("* is left-associative") {
          assertParseSuccess(Parser.expr, "x * y * z", InfixExpr("*", InfixExpr("*", "x", "y"), "z"))
        }

        it("= is right-associative") {
          assertParseSuccess(Parser.expr, "x = y = z", InfixExpr("=", "x", InfixExpr("=", "y", "z")))
        }

        it("* is higher than +") {
          assertParseSuccess(Parser.expr, "x + y * z", InfixExpr("+", "x", InfixExpr("*", "y", "z")))
        }
      }
    }
  }
}

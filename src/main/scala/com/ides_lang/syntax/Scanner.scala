package com.ides_lang.syntax

import scala.util.parsing.combinator.RegexParsers
import scala.util.parsing.combinator.lexical.{Lexical, StdLexical}
import scala.util.parsing.combinator.token.Tokens
import scala.util.parsing.input.CharArrayReader._
import scala.util.parsing.input.Positional

/**
 * Created by sedwards on 7/6/14.
 */

class Scanner extends StdLexical with RegexParsers {

  val oct = "(0[0-9]+)".r
  val dec = "([0-9]+\\.[0-9]+)".r
  val int = "(0|[1-9][0-9]*)".r
  val hex = "(0x[0-9A-F]+)".r
  val bin = "(0b[0-9A-F]+)".r

  val op_start = "!#%\\^&*\\-+/\\\\<>\\|?~"
  val op_any   = "=:!#%\\^&*\\-+/\\\\<>\\|?~"
  val op = s"[$op_start][$op_any]*"
  val id = "[A-Za-z_][A-Za-z0-9]*"

  val placeholder = ":[0-9]+"

  this.reserved ++= Seq(
    "true", "false",
    "def", "fn", "function", "var", "val", "struct", "class", "trait", "mod",
    "null", "namespace", "case",
    "if", "else",
    "public", "protected", "internal", "private", "extern", "const", "abstract", "unsafe", "intrinsic", "implicit", "locked",
    "throw", "new", "return", "match", "as",
    "...", "..", "=>"
  )

  override type Elem = Char

  def escapeStr = '\\' ~ chrExcept('\n', EofCh) ^^ { case esc ~ char => "\\" + char}
  def escapeChr = '\\' ~> chrExcept('\n', EofCh)

  override def token: Parser[Token] =
    ( dec                                                                ^^ DoubleTok
    | (oct | hex | bin | int)                                            ^^ IntegerTok
    | ("`[^`]+`".r | s"${id}(_${op})?".r)                                ^^ processIdent
    | ".."                                                               ^^ OpTok
    | op.r                                                               ^^ OpTok
    | s"[=:][$op_any]+".r                                                ^^ OpTok
    | placeholder.r                                                      ^^ PlaceholderTok
    | '\'' ~> (escapeChr | chrExcept('\'', '\n', EofCh)) <~ '\''         ^^ { case char => CharTok(char) }
    | '\"' ~> rep( escapeStr | chrExcept('\"', '\n', EofCh) ) <~ '\"'    ^^ { case chars => StringLit(chars mkString "") }
    | EofCh                                                              ^^^ EOF
    | '\'' ~> failure("unclosed string literal")
    | '\"' ~> failure("unclosed string literal")
    | ".".r                                                              ^^ Keyword
    )

  def intFromString(chars: String) = chars match {
    case oct(n) => Integer.parseInt(n.drop(1), 8)
    case int(n) => Integer.parseInt(n, 10)
    case hex(n) => Integer.parseInt(n.drop(2), 16)
    case bin(n) => Integer.parseInt(n.drop(2), 2)
  }

  case class OpTok(chars: String) extends Token {
    override def toString = s"operator $chars"
  }

  case class IntegerTok(chars: String) extends Token {
    val num = intFromString(chars)
    override def toString = chars
  }

  case class DoubleTok(chars: String) extends Token {
    val num = chars.toDouble
    override def toString = chars
  }

  case class PlaceholderTok(chars: String) extends Token {
    val num = chars.drop(1).toInt
    override def toString = chars
  }

  case class CharTok(char: Char) extends Token {
    override def chars = toString
    override def toString = char.toString
  }

}

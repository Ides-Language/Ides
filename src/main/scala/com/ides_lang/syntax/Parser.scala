package com.ides_lang.syntax

import scala.util.parsing.combinator._
import scala.util.parsing.combinator.lexical._
import scala.util.parsing.combinator.syntactical._
import scala.util.parsing.combinator.token._
import scala.util.parsing.input.CharArrayReader._
import scala.util.parsing.input.Positional

/**
 * Created by sedwards on 7/6/14.
 */

object Parser extends StdTokenParsers  {

  class Scanner extends StdLexical with RegexParsers {
    override type Elem = Char

    val oct = "(0[0-9]+)".r
    val dec = "([0-9]+\\.[0-9]+)".r
    val int = "(0|[1-9][0-9]*)".r
    val hex = "(0x[0-9A-F]+)".r
    val bin = "(0b[0-9A-F]+)".r
    val placeholder = ":[0-9]+"

    val op_start = "!#%\\^&*\\-+/\\\\<>\\|?~"
    val op_any   = "=:!#%\\^&*\\-+/\\\\<>\\|?~"
    val op = s"[$op_start][$op_any]*"
    val id = "[A-Za-z_][A-Za-z0-9]*"

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
      override def toString = s"'${char.toString}'"
    }

    case class StringTok(chars: String) extends Token {
      override def toString = "\""+chars+"\""
    }

    case class ErrorTok(msg: String) extends Token {
      def chars = msg
    }

    def escapeStr = '\\' ~ chrExcept('\n', EofCh) ^^ { case esc ~ char => "\\" + char}
    def escapeChr = '\\' ~> chrExcept('\n', EofCh)

    override def token: Parser[Token] =
      ( dec                                                                ^^ DoubleTok
      | (oct | hex | bin | int)                                            ^^ IntegerTok
      | ("`[^`]+`".r | s"${id}(_${op})?".r)                                ^^ processIdent
      | acceptSeq("..")                                                    ^^^ OpTok("..")
      | op.r                                                               ^^ OpTok
      | s"[=][$op_any]+".r                                                 ^^ OpTok
      | placeholder.r                                                      ^^ PlaceholderTok
      | '\'' ~> (escapeChr | chrExcept('\'', '\n', EofCh)) <~ '\''         ^^ { case char => CharTok(char) }
      | '\"' ~> rep(escapeStr | chrExcept('\"', '\n', EofCh)) <~ '\"'      ^^ { case chars => StringTok(chars mkString "") }
      | EofCh                                                              ^^^ EOF
      | '\'' ~> err("unclosed char literal")
      | '\"' ~> err("unclosed string literal")
      | s"[:][$op_any]+".r                                                 ^^ Keyword
      | ".".r                                                              ^^ Keyword
      )

    override def errorToken(msg: String) : Token =
      ErrorTok(msg)
  }

  def dbg[T](t: T) = { println(t); t }

  override type Tokens = Scanner
  override val lexical = new Tokens
  override type Elem = lexical.Token

  this.lexical.reserved += (
    "true", "false",
    "def", "fn", "function", "var", "val", "struct", "class", "trait", "mod",
    "null", "namespace", "case",
    "if", "else", "unless",
    "public", "protected", "internal", "private", "extern", "const", "abstract", "unsafe", "intrinsic", "implicit", "locked",
    "throw", "new", "return", "match", "as",
    "...", "..", ":="
  )

  def errTok : Parser[Nothing] =
    (elem("error", _.isInstanceOf[lexical.ErrorTok]) ^^ { tok => tok.chars }) >> err

  def dbl : Parser[ConstantDec] =
    elem("double", _.isInstanceOf[lexical.DoubleTok]) ^^
      { i => ConstantDec(i.asInstanceOf[lexical.DoubleTok].num) }

  def int : Parser[ConstantInt] =
    elem("integer", _.isInstanceOf[lexical.IntegerTok]) ^^
      { i => ConstantInt(i.asInstanceOf[lexical.IntegerTok].num) }

  def str : Parser[ConstantString] =
    elem("string", _.isInstanceOf[lexical.StringTok]) ^^
      { i => ConstantString(i.asInstanceOf[lexical.StringTok].chars) }

  def chr : Parser[ConstantChar] =
    elem("char", _.isInstanceOf[lexical.CharTok]) ^^
      { i => ConstantChar(i.asInstanceOf[lexical.CharTok].char) }

  def placeholder : Parser[PlaceholderExpr] =
    elem("placeholder", _.isInstanceOf[lexical.PlaceholderTok]) ^^
      { i => PlaceholderExpr(i.asInstanceOf[lexical.PlaceholderTok].num) }


  def op : Parser[String] =
    elem("operator", _.isInstanceOf[lexical.OpTok]) ^^
      { i => i.asInstanceOf[lexical.OpTok].chars }

  def operator : Parser[Ident] =
    ( op
    | "="
    | "match"
    | "as"
    | "if"
    | "else"
    | "unless"
    ) ^^ Ident

  def tru = "true" ^^^ { ConstantBool(v = true) }
  def fals = "false" ^^^ { ConstantBool(v = false) }
  def bool = tru | fals

  def constant = chr | dbl | int | str | bool

  def stmt : Parser[Expr]=
    ( expr
    | fn_decl
    | trait_decl
    | mod_decl
    ) <~ ";".?

  def file = compound_expr

  def identifier : Parser[Ident] = super.ident ^^ Ident

  def name =
    identifier ~ ("[" ~> tuple_items <~ "]").? ^^ {
      case i ~ Some(s) => Name(i, s)
      case i ~ None => Name(i)
    }

  def compound_expr : Parser[ExprList] =
    stmt.* ^^ ExprList.apply

  def primary_expr : Parser[Expr] =
    ( constant
    | placeholder
    | val_decl
    | var_decl
    | ("(" ~> tuple_items.! <~ ")") ^^ TupleExpr
    | ("{" ~> partial_fn  <~ "}") ^^ PartialFunction.apply
    | ("{" ~> compound_expr.! <~ "}") ^^ CompoundExpr
    | identifier
    )

  def postfix_expr : Parser[Expr] = primary_expr ~
    rep( ("(" ~> tuple_items <~ ")") ^^ { args => (lhs: Expr) => CallExpr(lhs, args) }
       | ("[" ~> tuple_items <~ "]") ^^ { args => (lhs: Expr) => BracketExpr(lhs, args) }
       | ("{" ~> tuple_items <~ "}") ^^ { args => (lhs: Expr) => PrefixExpr(lhs, args) }
       | ("." ~> name.!)             ^^ { i => (lhs: Expr) => InfixExpr.create(Ident("."), lhs, i) }
       ) ^^ { case lhs ~ items => items.foldLeft(lhs)((lhs, f) => f(lhs))}

  def prefix_expr : Parser[Expr] =
    ( operator ~ prefix_expr ^^ { case fn ~ rhs => PrefixExpr(fn, rhs) }
    | postfix_expr
    )

  def infix_expr : Parser[Expr] =
    ( prefix_expr ~ operator ~ infix_expr.! ^^ { case lhs ~ fn ~ rhs => InfixExpr.create(fn, lhs, rhs) }
    | prefix_expr
    )

  def expr : Parser[Expr] = infix_expr

  def partial_fn : Parser[List[Case]] =
    ( ("case" ~> expr <~ ":=") ~ expr ^^ { case cond ~ res => Case(cond, res) } ).*

  def name_items = repsep(name, ",") ^^ ExprList.apply

  def tuple_items : Parser[ExprList] =
    repsep(expr, ",") ^^ ExprList.apply

  def arg_item : Parser[ArgDecl] =
    qual.? ~ identifier ~ (":" ~> name.!).? ~ ("=" ~> expr).? ^^ {
      case q ~ i ~ t ~ d => ArgDecl(q.getOrElse(QualExpr()), i, t, d)
    }

  def arg_items : Parser[ExprList] =
    repsep(arg_item, ",") ^^ ExprList.apply

  def qual = vis

  def vis =
    ( "public" ^^ { q => QualExpr(pub = true) }
    | "protected" ^^ { q => QualExpr(prot = true) }
    | "internal" ^^ { q => QualExpr(internal = true) }
    | "private" ^^ { q => QualExpr(priv = true) }
    )

  def val_decl : Parser[ValDecl] =
    (qual.? <~ "val") ~! name ~! (":" ~> name).? ~ ("=" ~> expr).? ^^ {
      case q ~ n ~ t ~ e => ValDecl(q.getOrElse(QualExpr()), n, t, e)
    }

  def var_decl : Parser[VarDecl] =
    (qual.? <~ "var") ~! name ~! (":" ~> name).? ~ ("=" ~> expr).? ^^ {
      case q ~ n ~ t ~ e => VarDecl(q.getOrElse(QualExpr()), n, t, e)
    }

  def fn_decl : Parser[FnDecl] =
    (qual.? <~ "def") ~!
    name ~
    ("(" ~> arg_items.! <~ ")").? ~
    (":" ~> name.!).? ~
    ("=".? ~> expr.!).? ^^ {
      case q ~ n ~ a ~ t ~ e => FnDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList()), t, e)
    }

  def trait_decl : Parser[TraitDecl] =
    (qual.? <~ "trait") ~
    name.! ~
    ("(" ~> arg_items.! <~ ")").? ~
    (":" ~> repsep(name, ",").! ^^ ExprList.apply).? ~
    ("{" ~> compound_expr.! <~ "}") ^^ {
      case q ~ n ~ a ~ t ~ b => TraitDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList()), t.getOrElse(ExprList()), b)
    }

  def struct_decl : Parser[StructDecl] =
    (qual.? <~ "struct") ~!
    name.! ~
    ("(" ~> arg_items.! <~ ")").? ~
    (":" ~> repsep(name, ",").! ^^ ExprList.apply).? ~
    ("{" ~> compound_expr <~ "}").! ^^ {
      case q ~ n ~ a ~ t ~ b => StructDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList()), t.getOrElse(ExprList()), b)
    }

  def class_decl : Parser[ClassDecl] =
    (qual.? <~ "class") ~
    name.! ~
    ("(" ~> arg_items.! <~ ")").? ~
    (":" ~> repsep(name, ",").! ^^ ExprList.apply).? ~
    ("{" ~> compound_expr.! <~ "}") ^^ {
      case q ~ n ~ a ~ t ~ b => ClassDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList()), t.getOrElse(ExprList()), b)
    }

  def mod_decl : Parser[ModDecl] =
    (qual.? <~ "mod") ~
    identifier.! ~
    ("{" ~> compound_expr.! <~ "}") ^^ {
      case q ~ n ~ a => ModDecl(q.getOrElse(QualExpr()), n, a)
    }


  def parseFile(input: String) : ParseResult[Expr] = parse(file, input)
  def parseExpr(input: String) : ParseResult[Expr] = parse(expr, input)

  def parse[T](p : Parser[T], input: String) = phrase(p)(new lexical.Scanner(input))

  //val mod_decl = "mod" ~> name




  def EOI: Parser[Any] = new Parser[Any] {
    def apply(in: Input) = {
      if (in.atEnd) new Success( "EOI", in )
      else Failure("End of Input expected", in)
    }
  }


  //an implicit keyword function that gives a warning when a given word is not in the reserved/delimiters list
  override implicit def keyword(chars : String): Parser[String] =
    if(chars.length == 1 || lexical.reserved.contains(chars) || lexical.delimiters.contains(chars)) super.keyword(chars)
    else failure(s"You are trying to parse `$chars', but it is neither contained in the delimiters list, nor in the reserved keyword list of your lexical object")



  class ParserExt[T](p : Parser[T]) {
    def ! = commit(p)
  }
  implicit def parserToParserExt[T](p: Parser[T]) : ParserExt[T] = new ParserExt[T](p)
}

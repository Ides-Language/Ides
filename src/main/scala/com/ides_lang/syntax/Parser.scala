package com.ides_lang.syntax

import scala.util.parsing.combinator.syntactical.StdTokenParsers

/**
 * Created by sedwards on 7/6/14.
 */
object Parser extends StdTokenParsers  {
  def dbg[T](t: T) = { println(t); t }

  type Tokens = Scanner
  override type Elem = lexical.Token

  val lexical = new Tokens

  def dbl : Parser[ConstantDec] =
    elem("double", _.isInstanceOf[lexical.DoubleTok]) ^^
      { i => ConstantDec(i.asInstanceOf[lexical.DoubleTok].num) }

  def int : Parser[ConstantInt] =
    elem("integer", _.isInstanceOf[lexical.IntegerTok]) ^^
      { i => ConstantInt(i.asInstanceOf[lexical.IntegerTok].num) }

  def str : Parser[ConstantString] =
    elem("string", _.isInstanceOf[lexical.StringLit]) ^^
      { i => ConstantString(i.asInstanceOf[lexical.StringLit].chars) }

  def placeholder : Parser[PlaceholderExpr] =
    elem("placeholder", _.isInstanceOf[lexical.PlaceholderTok]) ^^
      { i => PlaceholderExpr(i.asInstanceOf[lexical.PlaceholderTok].num) }

  def operator : Parser[Ident] =
    ( elem("operator", _.isInstanceOf[lexical.OpTok]) ^^ { i => i.asInstanceOf[lexical.OpTok].chars }
    | "="
    | "match"
    | "as"
    ) ^^ Ident

  def tru = "true" ^^^ { ConstantBool(v = true) }
  def fals = "false" ^^^ { ConstantBool(v = false) }
  def bool = tru | fals

  def constant = dbl | int | str | bool

  def stmt : Parser[Expr]=
    ( expr
    | fn_decl
    | trait_decl
    | mod_decl
    ) <~ ";".?

  def file = compound_expr <~ EOI

  def identifier : Parser[Ident] = super.ident ^^ Ident

  def name =
    identifier ~ ("[" ~> tuple_items <~ "]").? ^^ {
      case i ~ Some(s) => Name(i, s)
      case i ~ None => Name(i)
    }

  def compound_expr : Parser[Expr] =
    stmt.* ^^ ExprList.apply

  def primary_expr : Parser[Expr] =
    ( constant
    | placeholder
    | val_decl
    | var_decl
    | ("(" ~> tuple_items.! <~ ")")
    | ("{" ~> compound_expr.! <~ "}")
    | identifier
    )

  def postfix_expr : Parser[Expr] = primary_expr ~
    rep( ("(" ~> tuple_items <~ ")") ^^ { args => (lhs: Expr) => CallExpr(lhs, args) }
       | ("[" ~> tuple_items <~ "]") ^^ { args => (lhs: Expr) => BracketExpr(lhs, args) }
       | ("{" ~> tuple_items <~ "}") ^^ { args => (lhs: Expr) => PrefixExpr(lhs, args) }
       | ("." ~> name.!)       ^^ { i => (lhs: Expr) => InfixExpr.create(Ident("."), lhs, i) }
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

  def parse[T](p : Parser[T], input: String) = p(new lexical.Scanner(input))

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

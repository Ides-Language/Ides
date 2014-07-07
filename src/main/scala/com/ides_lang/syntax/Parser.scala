package com.ides_lang.syntax

import scala.util.parsing.combinator._
import scala.util.parsing.combinator.lexical._
import scala.util.parsing.combinator.syntactical.{StdTokenParsers, StandardTokenParsers}

/**
 * Created by sedwards on 7/6/14.
 */
object Parser extends StdTokenParsers  {
  def dbg[T](t: T) = { println(t); t }

  type Tokens = Scanner

  val lexical = new Scanner

  def dbl : Parser[ConstantDec] =
    elem("double", _.isInstanceOf[lexical.DoubleTok]) ^^
      { i => ConstantDec(i.asInstanceOf[lexical.DoubleTok].num) }

  def int : Parser[ConstantInt] =
    elem("integer", _.isInstanceOf[lexical.IntegerTok]) ^^
      { i => ConstantInt(i.asInstanceOf[lexical.IntegerTok].num) }

  def placeholder : Parser[PlaceholderExpr] =
    elem("placeholder", _.isInstanceOf[lexical.PlaceholderTok]) ^^
      { i => PlaceholderExpr(i.asInstanceOf[lexical.PlaceholderTok].num) }

  def operator : Parser[Ident] =
    elem("operator", _.isInstanceOf[lexical.OpTok]) ^^
      { i => Ident(i.asInstanceOf[lexical.OpTok].chars) }

  def tru = "true" ^^^ { ConstantBool(v = true) }
  def fals = "false" ^^^ { ConstantBool(v = false) }

  def constant = dbl | int | tru | fals

  def stmt : Parser[Expr]=
    ( expr <~ ";"
    | fn_decl <~ ";".?
    | trait_decl <~ ";".?
    )

  def file = compound_expr

  def identifier : Parser[Ident] = super.ident ^^ Ident

  def name =
    identifier ~ ("[" ~> tuple_items <~ "]").? ^^ {
      case i ~ Some(s) => Name(i, s)
      case i ~ None => Name(i)
    }

  def compound_expr : Parser[Expr] =
    stmt.* ^^ ExprList

  def primary_expr : Parser[Expr] =
    ( constant
    | placeholder
    | val_decl
    | var_decl
    | ("(" ~> commit(tuple_items) <~ ")")
    | ("{" ~> commit(compound_expr) <~ "}")
    | identifier
    )

  def postfix_expr = primary_expr

  def prefix_expr =
    ( operator ~ primary_expr ^^ { case fn ~ rhs => UnaryExpr(fn, rhs) }
    | primary_expr
    )

  def infix_expr : Parser[Expr] =
    ( prefix_expr ~ operator ~ commit(infix_expr) ^^ { case lhs ~ fn ~ rhs => BinaryExpr(fn, lhs, rhs) }
    | prefix_expr
    )

  def expr : Parser[Expr] = infix_expr

  def name_items = repsep(name, ",") ^^ ExprList

  def tuple_items : Parser[ExprList] =
    repsep(expr, ",") ^^ ExprList

  def arg_item : Parser[ArgDecl] =
    qual.? ~ identifier ~ (":" ~> name).? ~ ("=" ~> expr).? ^^ {
      case q ~ i ~ t ~ d => ArgDecl(q.getOrElse(QualExpr()), i, t, d)
    }

  def arg_items : Parser[ExprList] =
    repsep(arg_item, ",") ^^ ExprList

  def qual = vis

  def vis =
    ( "public" ^^ { q => QualExpr(pub = true) }
    | "protected" ^^ { q => QualExpr(prot = true) }
    | "internal" ^^ { q => QualExpr(internal = true) }
    | "private" ^^ { q => QualExpr(priv = true) }
    )

  def val_decl : Parser[ValDecl] =
    (qual.? <~ "val") ~ name ~ (":" ~> name).? ~ ("=" ~> expr).? ^^ {
      case q ~ n ~ t ~ e => ValDecl(q.getOrElse(QualExpr()), n, t, e)
    }

  def var_decl : Parser[VarDecl] =
    ((qual.? <~ "var") ~ name ~ (":" ~> name).? ~ ("=" ~> expr).?) ^^ {
      case q ~ n ~ t ~ e => VarDecl(q.getOrElse(QualExpr()), n, t, e)
    }

  def fn_decl : Parser[FnDecl] =
    (qual.? <~ "def") ~ name ~ ("(" ~> arg_items <~ ")") ~ (":" ~> name).? ~ ("=" ~> expr).? ^^ {
      case q ~ n ~ a ~ t ~ e => FnDecl(q.getOrElse(QualExpr()), n, a, t, e)
    }

  def trait_decl : Parser[TraitDecl] =
    (qual.? <~ "trait") ~
    commit(name) ~
    ("(" ~> commit(arg_items) <~ ")").? ~
    (":" ~> commit(repsep(name, ",")) ^^ ExprList).? ~
    ("{" ~> commit(compound_expr) <~ "}") ^^ {
      case q ~ n ~ a ~ t ~ b => TraitDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList(Nil)), t.getOrElse(ExprList(Nil)), b)
    }

  def struct_decl : Parser[StructDecl] =
    (qual.? <~ "struct") ~
    commit(name) ~
    ("(" ~> commit(arg_items) <~ ")").? ~
    (":" ~> commit(repsep(name, ",")) ^^ ExprList).? ~
    ("{" ~> commit(compound_expr) <~ "}") ^^ {
      case q ~ n ~ a ~ t ~ b => StructDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList(Nil)), t.getOrElse(ExprList(Nil)), b)
    }

  def class_decl : Parser[ClassDecl] =
    (qual.? <~ "class") ~
    commit(name) ~
    ("(" ~> commit(arg_items) <~ ")").? ~
    (":" ~> commit(repsep(name, ",")) ^^ ExprList).? ~
    ("{" ~> commit(compound_expr) <~ "}") ^^ {
      case q ~ n ~ a ~ t ~ b => ClassDecl(q.getOrElse(QualExpr()), n, a.getOrElse(ExprList(Nil)), t.getOrElse(ExprList(Nil)), b)
    }

  def mod_decl : Parser[ModDecl] =
    (qual.? <~ "mod") ~
    commit(identifier) ~
    ("{" ~> commit(compound_expr) <~ "}") ^^ {
      case q ~ n ~ a => ModDecl(q.getOrElse(QualExpr()), n, a)
    }



  def parseFile(input: String) = commit(file)(new lexical.Scanner(input))

  //val mod_decl = "mod" ~> name





  //an implicit keyword function that gives a warning when a given word is not in the reserved/delimiters list
  override implicit def keyword(chars : String): Parser[String] =
    if(chars.length == 1 || lexical.reserved.contains(chars) || lexical.delimiters.contains(chars)) super.keyword(chars)
    else failure(s"You are trying to parse `$chars', but it is neither contained in the delimiters list, nor in the reserved keyword list of your lexical object")
}

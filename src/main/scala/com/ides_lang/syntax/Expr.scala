package com.ides_lang.syntax

import scala.util.parsing.input.Positional

/**
 * Created by sedwards on 7/6/14.
 */
abstract trait Expr extends Positional

case class NullExpr() extends Expr

case class Ident(name: String) extends Expr

case class Name(ident: Ident, genericArgs: ExprList = ExprList()) extends Expr

object Name {
  def apply(i: String) : Name = Name(Ident(i))
}

case class QualExpr(priv: Boolean = false, prot: Boolean = false, internal: Boolean = false, pub: Boolean = false,
                     extern: Boolean = false, const: Boolean = false, abst: Boolean = false, unsafe: Boolean = false,
                     intrinsic: Boolean = false, impl: Boolean = false, locked: Boolean = false) extends Expr {
  def | (rhs: QualExpr) = QualExpr(
    priv || rhs.priv,
    prot || rhs.prot,
    internal || rhs.internal,
    pub || rhs.pub,
    extern || rhs.extern,
    const || rhs.const,
    abst || rhs.abst,
    unsafe || rhs.unsafe,
    intrinsic || rhs.intrinsic,
    impl || rhs.impl,
    locked || rhs.locked
  )
}

object QualExpr {
  val None = QualExpr()
  val Public = QualExpr(pub = true)
  val Private = QualExpr(priv = true)
  val Protected = QualExpr(prot = true)
  val Internal = QualExpr(internal = true)
}

case class ExprList(items: Expr*) extends Expr

object ExprList {
  def apply(l: List[Expr]) : ExprList = ExprList(l : _*)
}

case class ConstantInt(v: Long) extends Expr
case class ConstantDec(v: Double) extends Expr
case class ConstantBool(v: Boolean) extends Expr
case class ConstantString(v: String) extends Expr
case class ConstantChar(v: Char) extends Expr

case class PlaceholderExpr(v: Integer) extends Expr

case class CallExpr(lhs: Expr, args: ExprList) extends Expr
case class BracketExpr(lhs: Expr, args: ExprList) extends Expr

case class InfixExpr(fn: Ident, lhs: Expr, rhs: Expr) extends Expr
case class PrefixExpr(fn: Expr, arg: Expr) extends Expr

case class PartialFunction(patterns: List[Case]) extends Expr
case class Case(pattern: Expr, result: Expr) extends Expr

case class ValDecl(qual: QualExpr, name: Name, ty: Option[Expr], init: Option[Expr]) extends Expr
case class VarDecl(qual: QualExpr, name: Name, ty: Option[Expr], init: Option[Expr]) extends Expr

case class FnDecl(qual: QualExpr, name: Name, args: ExprList, ty: Option[Expr], body: Option[Expr]) extends Expr

case class ArgDecl(qual: QualExpr, name: Ident, ty: Option[Expr], default: Option[Expr]) extends Expr

case class ClassDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr
case class StructDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr
case class TraitDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr

case class ModDecl(qual: QualExpr, name: Ident, body: Expr) extends Expr



object InfixExpr {
  object Associativity extends Enumeration {
    type Associativity = Value
    val Left, Right, Nonassoc = Value
  }

  import Associativity._

  val precedence = Seq(
    //("if", 0, Nonassoc), ("else", -200, Right),
    ("&&", 1, Left), ("||", 1, Left),
    ("|", 2, Left), ("^", 5, Right), ("&", 6, Left),
    ("or", 7, Left),
    ("==", 8, Left), ("!=", 8, Left),
    (">", 9, Left),("<", 9, Left),(">=", 9, Left),("<=", 9, Left),
    ("+", 10, Left),("-", 10, Left),
    ("*", 11, Left),("/", 11, Left),("%", 11, Left),
    ("://", 12, Nonassoc),
    ("as", 50, Left)
  )

  def getPrecedence(op: String) =
    precedence
      .find(c => op == c._1)
      .getOrElse(if (op.startsWith("=")) (op, -100, Right) else (op, 0, Left))

  def create(fn: Ident, lhs: Expr, rhs: Expr) : InfixExpr = {
    val prec = getPrecedence(fn.name)
    rhs match {
      case InfixExpr(f, l, r) => {
        val otherP = getPrecedence(f.name)
        if ((prec._2 > otherP._2) || (prec._2 == otherP._2 && prec._3 == Left)) {
          new InfixExpr(f, create(fn, lhs, l), r)
        } else {
          new InfixExpr(fn, lhs, rhs)
        }
      }
      case _ =>
        new InfixExpr(fn, lhs, rhs)
    }
  }
}
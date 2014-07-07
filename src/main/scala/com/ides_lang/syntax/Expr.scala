package com.ides_lang.syntax

import scala.util.parsing.input.Positional

/**
 * Created by sedwards on 7/6/14.
 */
abstract class Expr() extends Positional

case class NullExpr() extends Expr

case class Ident(name: String) extends Expr

case class Name(ident: Ident, genericArgs: ExprList = ExprList(Nil)) extends Expr

case class QualExpr(priv: Boolean = false, prot: Boolean = false, internal: Boolean = false, pub: Boolean = false,
                     extern: Boolean = false, const: Boolean = false, abst: Boolean = false, unsafe: Boolean = false,
                     intrinsic: Boolean = false, impl: Boolean = false, locked: Boolean = false) extends Expr

case class ExprList(items: List[Expr]) extends Expr

case class ConstantInt(v: Long) extends Expr
case class ConstantDec(v: Double) extends Expr
case class ConstantBool(v: Boolean) extends Expr
case class ConstantString(v: String) extends Expr
case class ConstantChar(v: Char) extends Expr

case class PlaceholderExpr(v: Integer) extends Expr

case class CallExpr(lhs: Expr, args: ExprList) extends Expr
case class BracketExpr(lhs: Expr, args: ExprList) extends Expr

case class BinaryExpr(fn: Ident, lhs: Expr, rhs: Expr) extends Expr
case class UnaryExpr(fn: Expr, arg: Expr) extends Expr

case class PartialFunction(patterns: Map[Expr, Expr]) extends Expr

case class ValDecl(qual: QualExpr, name: Name, ty: Option[Expr], init: Option[Expr]) extends Expr

case class VarDecl(qual: QualExpr, name: Name, ty: Option[Expr], init: Option[Expr]) extends Expr

case class FnDecl(qual: QualExpr, name: Name, args: ExprList, ty: Option[Expr], body: Option[Expr]) extends Expr

case class ArgDecl(qual: QualExpr, name: Ident, ty: Option[Expr], default: Option[Expr]) extends Expr

case class ClassDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr
case class StructDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr
case class TraitDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr

case class ModDecl(qual: QualExpr, name: Ident, body: Expr) extends Expr



object BinaryExpr {
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
    ("as", 50, Left)
  )

  def getPrecedence(op: String) =
    precedence
      .find(c => op == c._1)
      .getOrElse(if (op.startsWith("=")) (op, -100, Right) else (op, 0, Left))

  def create(fn: Ident, lhs: Expr, rhs: Expr) = {
    val prec = getPrecedence(fn.name)
    rhs match {
      case BinaryExpr(f, l, r) => {
        val otherP = getPrecedence(f.name)
        if ((prec._2 > otherP._2) || (prec._2 == otherP._2 && prec._3 == Left)) {
          new BinaryExpr(f, new BinaryExpr(fn, lhs, l), r)
        } else {
          new BinaryExpr(fn, lhs, rhs)
        }
      }
      case _ =>
        new BinaryExpr(fn, lhs, rhs)
    }
  }
}
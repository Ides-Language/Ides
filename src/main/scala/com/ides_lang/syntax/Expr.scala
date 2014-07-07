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
case class UnaryExpr(fn: Ident, arg: Expr) extends Expr

case class PartialFunction(patterns: Map[Expr, Expr]) extends Expr

case class ValDecl(qual: QualExpr, name: Name, ty: Option[Expr], init: Option[Expr]) extends Expr

case class VarDecl(qual: QualExpr, name: Name, ty: Option[Expr], init: Option[Expr]) extends Expr

case class FnDecl(qual: QualExpr, name: Name, args: ExprList, ty: Option[Expr], body: Option[Expr]) extends Expr

case class ArgDecl(qual: QualExpr, name: Ident, ty: Option[Expr], default: Option[Expr]) extends Expr

case class ClassDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr
case class StructDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr
case class TraitDecl(qual: QualExpr, name: Name, args: ExprList, supers: ExprList, body: Expr) extends Expr

case class ModDecl(qual: QualExpr, name: Ident, body: Expr) extends Expr

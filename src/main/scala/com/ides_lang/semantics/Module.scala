package com.ides_lang.semantics

import com.ides_lang.syntax._

/**
 * Created by sedwards on 8/31/14.
 */
case class Module(ast: ModDecl, parent: Option[Module] = None) extends Sema with Scope {
  val name = ast.name.name
  val qual = ast.qual

  lazy val mangle : String = parent match {
    case Some(p) => s"${p.mangle}:$name"
    case None => "$ides:"+name
  }

  val items = ast.body.items.collect {
    case expr : ModDecl => expr.name.name -> Module(expr, Some(this))
    case expr : FnDecl => expr.name.ident.name -> Fn(expr, Some(this))
  }.toMap

  override def lookup(name: String): Option[Sema] =
    items.get(name)
}

object Module {
  lazy val llvm = Module(Parser.parse(Parser.mod_decl,
    """
      |mod LLVM {
      | intrinsic def ret[T](v: T) : Unit
      |
      | intrinsic def add[T](lhs: T, rhs: T) : T
      | intrinsic def sub[T](lhs: T, rhs: T) : T
      | intrinsic def mul[T](lhs: T, rhs: T) : T
      | intrinsic def div[T](lhs: T, rhs: T) : T
      | intrinsic def rem[T](lhs: T, rhs: T) : T
      |
      | intrinsic def shl[T](lhs: T, rhs: T) : T
      | intrinsic def lshr[T](lhs: T, rhs: T) : T
      | intrinsic def ashr[T](lhs: T, rhs: T) : T
      |
      | intrinsic def and(lhs: Bool, rhs: Bool) : Bool
      | intrinsic def or(lhs: Bool, rhs: Bool) : Bool
      | intrinsic def xor(lhs: Bool, rhs: Bool) : Bool
      |
      | intrinsic def cmp[T](lhs: T, rhs: T) : Bool
      |}
    """.stripMargin).get)

}

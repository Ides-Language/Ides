package com.ides_lang.semantics

import com.ides_lang.syntax._

/**
 * Created by sedwards on 9/1/14.
 */
case class Fn(ast: FnDecl, parent: Option[Module] = None) extends Sema with Scope {
  val name = ast.name.ident.name

  lazy val mangle = parent match {
    case Some(p) => s"${p.mangle}:$name"
    case None => "$ides:"+name
  }

  override def lookup(name: String): Option[Sema] = None
}

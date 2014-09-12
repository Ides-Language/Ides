package com.ides_lang.semantics

/**
 * Created by sedwards on 9/1/14.
 */
trait Scope {
  def lookup(name: String) : Option[Sema]
}

package com.ides_lang.util

import com.ides_lang.syntax.QualExpr

import scala.collection.mutable
import scala.util.{Failure, Success, Try}

/**
 * Created by sedwards on 9/1/14.
 */
trait SymbolTable {
  trait Symbol {
    val qual : QualExpr
    val name : String
  }

  case class DuplicateSymbolException(msg: String) extends RuntimeException

  def add(sym : Symbol) : Try[Symbol] = {
    if (symbols.contains(sym.name)) {
      Failure(DuplicateSymbolException(s"duplicate symbol `${sym.name}`"))
    } else {
      symbols += sym.name -> sym
      Success(sym)
    }
  }

  private val symbols = new mutable.HashMap[String, Symbol]()
}

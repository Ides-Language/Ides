package com.ides_lang.test.semantics

import com.ides_lang.semantics._
import com.ides_lang.syntax._
import com.ides_lang.test.IdesSpec

/**
 * Created by sedwards on 8/31/14.
 */
class SemanticsSpec extends IdesSpec {
  describe("modules") {
    it("should analyze a module") {
      val mod = Module(parse(Parser.mod_decl, "mod test { }"))
      assert(mod.items.isEmpty)
      assert(mod.name == "test")
      assert(mod.qual == QualExpr.None)
      assert(mod.mangle == "$ides:test")
    }

    it("should understand submodules") {
      val mod = Module(parse(Parser.mod_decl, "mod test { mod inner0 { } mod inner1 { } }"))
      assert(mod.items.size == 2)
      assert(mod.name == "test")
      assert(mod.qual == QualExpr.None)
      assert(mod.lookup("inner0").isDefined)
      assert(mod.lookup("inner1").get.mangle == "$ides:test:inner1")
    }

    it("can contain functions") {
      val mod = Module(parse(Parser.mod_decl, "mod test { def f = x }"))
      assert(mod.lookup("f").isInstanceOf[Some[Fn]])
      assert(mod.lookup("f").get.mangle == "$ides:test:f")
    }
  }
}

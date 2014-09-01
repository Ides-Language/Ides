package com.ides_lang

import java.io.File

import com.ides_lang.syntax.{Expr, ExprList, Parser}
import com.ides_lang.info.BuildInfo

import scala.io.Source
import scala.util.{Try, Success, Failure}
import scala.util.parsing.input.StreamReader

/**
 * Created by sedwards on 7/6/14.
 */
object Idesc {
  case class Config(outFile: File = new File("./Module.ilib"), files: Seq[File] = Seq())

  val parser = new scopt.OptionParser[Config](BuildInfo.name) {
    head(BuildInfo.name, BuildInfo.version)

    help("help").abbr("h").text("prints this usage text")

    opt[File]('o', "out").optional().valueName("<file>").action { (x, c) =>
      c.copy(outFile = x)
    } text "output file"

    arg[File]("<file>...").unbounded().action { (x, c) =>
      c.copy(files = c.files :+ x)
    } text "input file(s)"

  }

  def main(args: Array[String]) {
    parser.parse(args, Config()) map { config =>
      config.files.map { f =>
        Parser.parse(Parser.file, StreamReader(Source.fromFile(f).bufferedReader()).toString)
      }.foldLeft[Try[List[Expr]]](Success(Nil)) { (result, item) =>
        result match {
          case Success(f) => item match {
              case Parser.Success(res, next) => Success(f :+ res)
              case Parser.Failure(msg, next) => println(s"Failed to parse source: $msg"); Failure(new Exception(msg))
              case Parser.Error(msg, next) => println(s"Parse error: $msg"); Failure(new Exception(msg))
            }
          case fail => fail
        }
      } map { ast =>
          println(ast)
      }
    }
  }
}

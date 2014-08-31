import scoverage.ScoverageSbtPlugin.instrumentSettings
import org.scoverage.coveralls.CoverallsPlugin.coverallsSettings
import CoverallsPlugin.CoverallsKeys._

name := "Ides"

version := "1.0-SNAPSHOT"

scalaVersion := "2.11.2"

libraryDependencies ++= Seq(
  "org.scala-lang" % "scala-parser-combinators" % "2.11.0-M4",
  "com.github.scopt" %% "scopt" % "3.2.0",
  "org.scalatest" %% "scalatest" % "2.2.1" % "test"
)

resolvers += Resolver.sonatypeRepo("public")

buildInfoSettings

instrumentSettings

coverallsSettings

coverallsTokenFile := ".coveralls"

sourceGenerators in Compile <+= buildInfo

buildInfoKeys := Seq[BuildInfoKey](name, version, scalaVersion, sbtVersion)

buildInfoPackage := "com.ides_lang.info"

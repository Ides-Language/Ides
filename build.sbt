name := "Ides"

version := "1.0-SNAPSHOT"

scalaVersion := "2.11.0"

libraryDependencies ++= Seq(
  "org.scala-lang" % "scala-parser-combinators" % "2.11.0-M4",
  "com.github.scopt" %% "scopt" % "3.2.0"
)

resolvers += Resolver.sonatypeRepo("public")


buildInfoSettings

sourceGenerators in Compile <+= buildInfo

buildInfoKeys := Seq[BuildInfoKey](name, version, scalaVersion, sbtVersion)

buildInfoPackage := "com.ides_lang.info"

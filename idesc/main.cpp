// Cplus.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/PrettyStackTrace.h>
#include "llvm/Support/Signals.h"
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Linker.h>

#include <ides/common.h>
#include <ides/Parsing/Parser.h>
#include <ides/Compiling/Compiler.h>
#include <ides/Source/SourcePackage.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

po::variables_map args;
std::string current_file;

int main(int argc, const char* argv[])
{
    llvm::sys::PrintStackTraceOnErrorSignal();
    llvm::PrettyStackTraceProgram X(argc, argv);
    
    
	po::options_description genericdesc("Options");
	genericdesc.add_options()
		("help,h", "Show help message")
		("interactive,i", "Run in interactive mode")
        ("parse-only", "Parse the input file(s) and print the AST (YAML)")
		;

	po::options_description hiddendesc("Hidden Options");
	hiddendesc.add_options()
        ("input-file", po::value<fs::path>(), "input file")
        ("debug-mode", "debug mode")
        ("verbose-mode", "verbose mode")
        ("trace-mode", "trace mode")
		;

	po::options_description visibledesc("Allowed Options");
	visibledesc.add(genericdesc);

	po::options_description alldesc("All Options");
	alldesc.add(visibledesc).add(hiddendesc);

	po::positional_options_description p;
	p.add("input-file", -1);
	
	try {
		po::store(po::command_line_parser(argc, argv).options(alldesc).positional(p).run(), args);
		po::notify(args);
	}
	catch (const std::exception& ex) {
        MSG(F_BADARGS) % ex.what();
	}

	if (args.count("help")) {
		std::cout << visibledesc << std::endl;
		return 0;
	}

    if (args.count("debug-mode")) {
        Ides::MessageBuilder::min_print = Ides::DEBUG;
    }
    if (args.count("verbose-mode")) {
        Ides::MessageBuilder::min_print = Ides::INFO;
    }
    if (args.count("trace-mode")) {
        Ides::MessageBuilder::min_print = Ides::TRACE;
    }

	if (args.count("input-file") == 0) {
		MSG(F_NOINPUTS);
	}
    auto file = args["input-file"].as<fs::path>();

    DBG("Driver startup complete. Beginning compile phase.");

    if (args.count("parse-only")) {
        Ides::SourceFile source(NULL, file.string());
        source.Open();
        Ides::Parser parser;
        Ides::AstPtr ast = parser.Parse(source);
        YAML::Emitter out;
        ast->Emit(out);
        std::cout << out.c_str() << std::endl;
    }
    else {
        std::string filestr = file.string();
        try {
            Ides::SourcePackage package(file);
            Ides::Compiler compiler;
            Ides::AstPtr ast = compiler.Parse(package);
            std::cout << *ast << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            return 1;
        }
    }

	return 0;
}


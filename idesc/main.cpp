// Cplus.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>
#include <chrono>

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
    auto start_time = std::chrono::high_resolution_clock::now();

    llvm::sys::PrintStackTraceOnErrorSignal();
    llvm::PrettyStackTraceProgram X(argc, argv);
    
    
	po::options_description genericdesc("Options");
	genericdesc.add_options()
		("help,h", "Show help message")
		("interactive,i", "Run in interactive mode")
        ("output-file,o", po::value<fs::path>(), "Output file for compiler results. (Defaults to stdout.)")
        ("parse-ast", "Parse an AST from YAML provided through stdin.")
        ("print-ast", "Print the resulting AST as YAML and stop.")
        ("print-src", "Print formatted source code from the AST and stop.")
		;

	po::options_description hiddendesc("Hidden Options");
	hiddendesc.add_options()
        ("input-file", po::value<fs::path>()->default_value("."), "input file")
        ("debug", po::value<int>()->default_value(0), "debug mode")
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

    Ides::MessageBuilder::min_print = (Ides::Severity)(Ides::DBG_DEFAULT - args["debug"].as<int>());

	if (args.count("help")) { SETTRACE("--help")
		std::cout << visibledesc << std::endl;
		return 0;
	}

    auto file = args["input-file"].as<fs::path>();

    std::ostream* outstream = &std::cout;

    Ides::AstPtr ast;
    Ides::Parser parser;

    DBG("Driver startup complete. Beginning compile phase.");
    if (args.count("parse-ast")) { SETTRACE("--parse-ast")
        YAML::Node doc;
        if (args.count("input-file")) {
            std::fstream fs(file.string());
            doc = YAML::Load(fs);
            fs.close();
        } else {
            doc = YAML::Load(std::cin);
        }
        ast = Ides::AstPtr(Ides::Ast::Read(doc));
    }
    else {
        try {
            Ides::SourcePackage package(file);
            ast = parser.Parse(package);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    std::fstream outfile;
    outfile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    if (args.count("output-file")) { SETTRACE("--output-file")
        fs::path outputfile = args["output-file"].as<fs::path>();
        try {
            outfile.open(outputfile.string(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        }
        catch (const std::ofstream::failure& e) {
            MSG(F_BADOUTFILE) % outputfile;
        }
        outstream = &outfile;
        assert(outstream->good());
    }

    if (args.count("print-ast")) { SETTRACE("--print-ast")
        YAML::Emitter out(*outstream);
        ast->Emit(out);

        *outstream << std::endl;
    }
    else if (args.count("print-src")) { SETTRACE("--print-src")
        *outstream << *ast << std::endl;
    }
    else {
        throw std::runtime_error("Codegen not yet implemented.");
    }

    if (outfile.is_open())
        outfile.close();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span =
        std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

    MSG(D_RUNTIME) % time_span.count();


	return 0;
}


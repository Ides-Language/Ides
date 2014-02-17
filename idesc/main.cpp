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
        ("print-visitors", po::value<std::vector<std::string>>()->multitoken(), "Print visitor template.")
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
    else if (args.count("print-visitors")) {
        auto list = args["print-visitors"].as<std::vector<std::string>>();
        std::string name = list[0];
        list.erase(list.begin());
        std::string rettype = list.empty() ? "void" : list[0];
        if (!list.empty()) list.erase(list.begin());

        std::stringstream argtypes;
        std::stringstream argnames;

        while (list.size() >= 2) {
            argtypes << ", " << list[0];
            argnames << ", " << list[0] << " " << list[1];
            list.erase(list.begin(), list.begin() + 2);
        }

#define PRINT_IDES_VISITOR(r, data, elem) \
        std::cout << "template<> " << rettype << " " << name << \
        "(const " << BOOST_PP_STRINGIZE(elem) << "& ast" << argnames.str() << ") { " << \
        (list.empty() ? "" : list[0]) << " }" << std::endl;

        std::cout << "DECL_AST_VISITOR(" << name << ", " << rettype << argtypes.str() << ");" << std::endl;
        BOOST_PP_SEQ_FOR_EACH(PRINT_IDES_VISITOR, _, AST_TYPES)

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

    DBG("Parse complete. Beginning output phase.");

    std::fstream outfile;
    outfile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    if (args.count("output-file")) { SETTRACE("--output-file")
        fs::path outputfile = args["output-file"].as<fs::path>();
        DBG("Output file set to" << outputfile);
        try {
            outfile.open(outputfile.string(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        }
        catch (const std::ofstream::failure& e) {
            MSG(F_BADOUTFILE) % outputfile;
        }
        outstream = &outfile;
        assert(outstream->good());
    }
    else {
        DBG("No output file selected. Will output to STDOUT.");
    }

    if (args.count("print-ast")) { SETTRACE("--print-ast")
        DBG("Printing YAML from AST.");

        YAML::Emitter out(*outstream);
        ast->Emit(out);

        *outstream << std::endl;
    }
    else if (args.count("print-src")) { SETTRACE("--print-src")
        DBG("Printing formatted source code from AST.");
        *outstream << *ast << std::endl;
    }
    else {
        DBG("No output forms selected. Beginning compilation.");
        try {
            Ides::Compiler compiler;
            compiler.Compile(*ast);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    DBG("Compilation complete.");

    if (outfile.is_open())
        outfile.close();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span =
        std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

    MSG(D_RUNTIME) % time_span.count();


	return 0;
}


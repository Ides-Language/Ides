// Cplus.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <ides/Parsing/Parser.h>
#include <ides/AST/AST.h>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Linker.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

po::variables_map args;
std::string current_file;

int main(int argc, const char* argv[])
{
	po::options_description genericdesc("Options");
	genericdesc.add_options()
		("help,h", "Show help message")
		("output-file,o", po::value<fs::path>(), "Output file")
		("interactive,i", "Run in interactive mode")
        ("name", po::value<std::string>()->default_value("Ides Module"), "Module name")
		;

	po::options_description compilerdesc("Compiler Options");
	compilerdesc.add_options()
		("include-path,I", po::value<std::vector<std::string> >()->
			//default_value(std::vector<fs::path>())->
			composing(), "Additional include paths")
		("preprocess,E", "Preprocess, but don't compile, assemble, or link (outputs .pp.ic file")
		("compile-only,S", "Preprocess and compile, but don't assemble or link (outputs LLVM .ll file)")
		("compile,c", "Preprocess, compile, and assemble, but don't link (outputs LLVM .bc file)")
		("optimization-level,O", po::value<unsigned int>(0), "Optimization level.")
		//("define,D", po::value<std::map<std::string, std::string> >()->composing(), "Add preprocessor definition")
		("no-gc", "Disable garbage collection support")
		;

	po::options_description linkerdesc("Linker Options");
	compilerdesc.add_options()
        ("library-path,L", po::value<std::vector<std::string> >()->composing(), "Additional library paths")
        ("lib,l", po::value<std::vector<std::string> >()->composing(), "Additional libraries to link")
		;

	po::options_description rundesc("Runtime Options");
	rundesc.add_options()
		("no-extlib", "Disable extended standard library (disables garbage collection and class support)")
		("use-gc", po::value<std::string>()->default_value("shadow-stack"), "The GC algorithm to use")
		;
	po::options_description devdesc("Developer Options");
	devdesc.add_options()
        ("show-lex", "Dump the token sequence from the lexer.")
		("show-ast", "Print the full AST of the compiled source.")
		("show-mod", "Dump the full LLVM module assembly code.")
		;

	po::options_description hiddendesc("Hidden Options");
	hiddendesc.add_options()
		("input-file", po::value<std::vector<fs::path> >(), "input file")
		;

	po::options_description visibledesc("Allowed Options");
	visibledesc.add(genericdesc).add(compilerdesc).add(linkerdesc);

	po::options_description alldesc("All Options");
	alldesc.add(visibledesc).add(hiddendesc).add(devdesc);

	po::positional_options_description p;
	p.add("input-file", -1);
	
	try {
		po::store(po::command_line_parser(argc, argv).options(alldesc).positional(p).run(), args);
		po::notify(args);
	}
	catch (const std::exception& ex) {
		std::cerr << "error " << ex.what() << std::endl;
		return 1;
	}

	if (args.count("help")) {
		std::cout << visibledesc << std::endl;
		return 0;
	}

	if (args.count("input-file") == 0) {
		std::cerr << "No input files specified." << std::endl;
		return 1;
	}
    std::string output_name = args["name"].as<std::string>();

	std::string output_file = output_name + ".ll";
	if (args.count("output-file")) {
		output_file = args["output-file"].as<std::string>();
	}

	if (boost::filesystem::exists(output_file)) {
		if (boost::filesystem::is_regular_file(output_file)) {
			boost::filesystem::remove(output_file);
		}
		else {
			std::cerr << "Output file " << output_file << " already exists and is not a regular file. Can not continue." << std::endl;
			return 1;
		}
	}
    
    std::list<llvm::Module*> modules;
    
    Ides::Parsing::Parser parser;
	const std::vector<fs::path> files = args["input-file"].as<std::vector<fs::path> >();
	for (std::vector<fs::path>::const_iterator i = files.begin(); i != files.end(); ++i) {
		if (!fs::is_regular_file(*i)) {
			std::cerr << "Could not open " << *i << "." << std::endl;
			continue;
		}

		current_file = i->string();

		fs::ifstream srcfile(*i);
        fs::path outpath = *i;
        outpath.replace_extension(".s");
        
        
        Ides::Parsing::Parser::ParseTree t = parser.Parse(srcfile, current_file);
        if (t.get() == NULL) return 1;
        
        llvm::Module* mod = parser.Compile(t);
        if (mod == NULL) return 1;
        
        modules.push_back(mod);
	}
    
    llvm::Linker linker(output_name, output_name, llvm::getGlobalContext());
    
    linker.addSystemPaths();
    if (args.count("library-path"))
        linker.addPaths(args["library-path"].as<std::vector<std::string> >());
    
    if (args.count("library")) {
        if (linker.LinkInLibraries(args["library"].as<std::vector<std::string> >())) {
            std::cerr << linker.getLastError() << std::endl;
        }
    }
    bool isnative;
    if (linker.LinkInLibrary("c", isnative)) {
        std::cerr << linker.getLastError() << std::endl;
    }
    
    for (auto modi = modules.begin(); modi != modules.end(); ++modi) {
        if (linker.LinkInModule(*modi)) {
            std::cerr << linker.getLastError() << std::endl;
        }
    }
    
    linker.getModule()->dump();
    
    fs::ofstream outfile(output_file);
    llvm::raw_os_ostream llvm_outfile(outfile);
    llvm::WriteBitcodeToFile(linker.getModule(), llvm_outfile);

	return 0;
}


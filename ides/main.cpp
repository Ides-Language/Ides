// Cplus.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

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
		("library-path,L", po::value<std::vector<fs::path> >()->composing(), "Additional library paths")
		("lib,l", po::value<std::vector<std::string> >()->composing(), "Additional libraries to link")
		;

	po::options_description rundesc("Runtime Options");
	rundesc.add_options()
		("no-extlib", "Disable extended standard library (disables garbage collection and class support)")
		("use-gc", po::value<std::string>()->default_value("shadow-stack"), "The GC algorithm to use")
		;
#ifdef _DEBUG
	po::options_description devdesc("Developer Options");
	devdesc.add_options()
		("show-ast", "Print the full AST of the compiled source.")
		("show-mod", "Dump the full LLVM module assembly code.")
		;
#endif

	po::options_description hiddendesc("Hidden Options");
	hiddendesc.add_options()
		("input-file", po::value<std::vector<fs::path> >()->required(), "input file")
		;

	po::options_description visibledesc("Allowed Options");
	visibledesc.add(genericdesc).add(compilerdesc).add(linkerdesc)
#ifdef _DEBUG
		.add(devdesc)
#endif
		;

	po::options_description alldesc("All Options");
	alldesc.add(visibledesc).add(hiddendesc);

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

	std::string output_file = args["name"].as<std::string>() + ".ll";
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

	const std::vector<fs::path> files = args["input-file"].as<std::vector<fs::path> >();
	for (std::vector<fs::path>::const_iterator i = files.begin(); i != files.end(); ++i) {
		if (!fs::is_regular_file(*i)) {
			std::cerr << "Could not open " << *i << "." << std::endl;
			continue;
		}

		current_file = i->string();

		fs::ifstream srcfile(*i);
		std::string data(
			std::istreambuf_iterator<char>(srcfile.rdbuf()),
			std::istreambuf_iterator<char>());
		
		srcfile.close();

#ifdef _DEBUG
		if (args.count("show-ast")) {
			std::cout << "AST: " << std::endl;
			std::cout << std::endl;
		}
#endif
		try {
#ifdef _DEBUG
			if (args.count("show-mod")) {
				std::cout << "Module: " << std::endl;
				std::cout << std::endl;
			}
#endif
			
			std::string errinfo;
		}
//		catch (const compiler_error&) {
//			return 1;
//		}
		catch (const std::exception& ex) {
			std::cerr << "Unhandled exception during compilation: " << ex.what() << std::endl;
		}

	}

	return 0;
}


// Cplus.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <ides/Project/Project.h>
#include <ides/AST/AST.h>

#include <ides/CodeGen/CodeGen.h>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/PrettyStackTrace.h>
#include "llvm/Support/Signals.h"
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Linker.h>

#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/LangOptions.h>

#include <ides/Diagnostics/Diagnostics.h>

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
    ("output,o", po::value<std::string>(), "Output path")
		("interactive,i", "Run in interactive mode")
        ("name", po::value<std::string>()->default_value("Ides Module"), "Module name")
		;

	po::options_description compilerdesc("Compiler Options");
	compilerdesc.add_options()
		("include-path,I", po::value<std::vector<std::string> >()->
			//default_value(std::vector<fs::path>())->
			composing(), "Additional include paths")
		("compile,c", "Compile source file, but don't link.")
		("optimization-level,O", po::value<unsigned int>(0), "Optimization level.")
		//("define,D", po::value<std::map<std::string, std::string> >()->composing(), "Add preprocessor definition")
		//("no-gc", "Disable garbage collection support")
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

	po::options_description hiddendesc("Hidden Options");
	hiddendesc.add_options()
		("input-file", po::value<std::vector<fs::path> >(), "input file")
		;

	po::options_description visibledesc("Allowed Options");
	visibledesc.add(genericdesc).add(compilerdesc).add(linkerdesc);

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
    std::string output_name = args["name"].as<std::string>();

	std::string output_file = output_name + ".ilib";
	if (args.count("output")) {
		output_file = args["output"].as<std::string>();
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
    
    clang::DiagnosticOptions diagOpts;
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diagIDs= new clang::DiagnosticIDs();
    llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag = new clang::DiagnosticsEngine(diagIDs, &diagOpts, new clang::TextDiagnosticPrinter(llvm::errs(), &diagOpts), false);
    
    Ides::AST::ASTContext actx(diag);
    
    Ides::Diagnostics::InitAllDiagnostics(*diag);
    
    Ides::Project::Project proj(diag, actx);
	const std::vector<fs::path> files = args["input-file"].as<std::vector<fs::path> >();
	for (std::vector<fs::path>::const_iterator i = files.begin(); i != files.end(); ++i) {
		if (!fs::is_regular_file(*i)) {
			std::cerr << "Could not open " << *i << "." << std::endl;
			continue;
		}
        
        clang::LangOptions langopts;
        diag->getClient()->BeginSourceFile(langopts);

		current_file = i->string();

		fs::ifstream srcfile(*i);
        
        Ides::AST::AST* ast = NULL;
        try {
            ast = proj.ParseFile(current_file);
            modules.push_back(proj.Compile((Ides::AST::CompilationUnit*)ast));
        } catch (const std::exception&) {
            return 1;
        }
        
        diag->getClient()->EndSourceFile();
    }
    
    llvm::Module* linkermod = new llvm::Module(output_name, llvm::getGlobalContext());
    llvm::Linker linker(linkermod);
    
    for (auto modi = modules.begin(); modi != modules.end(); ++modi) {
        std::string err;
        if (linker.linkInModule(*modi, &err)) {
            std::cerr << err << std::endl;
        }
    }
    
    linkermod->dump();
    
    fs::ofstream outfile(output_file);
    llvm::raw_os_ostream llvm_outfile(outfile);
    llvm::WriteBitcodeToFile(linkermod, llvm_outfile);
    

	return 0;
}


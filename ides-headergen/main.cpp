#include <boost/unordered_map.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include <llvm/Support/PrettyStackTrace.h>
#include "llvm/Support/Signals.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseAST.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


class IdesASTPrinter : public clang::ASTConsumer {
public:
    IdesASTPrinter(std::ostream& os) : os(os), autoarg(0) {
    }
    
    virtual void Initialize (clang::ASTContext &Context) {
        ctx = &Context;
        ASTConsumer::Initialize(Context);
    }
    
    void HandleQualType(const clang::QualType& ty) {
        clang::QualType t = ty.getDesugaredType(*this->ctx);
        if (t->isFunctionPointerType()) {
            const clang::FunctionProtoType* ft = t->getPointeeType()->getAs<clang::FunctionProtoType>();
            assert(ft);
            os << "fn(";
            auto i = ft->arg_type_begin();
            if (i != ft->arg_type_end()) {
                HandleQualType(*i);
            }
            for (; i != ft->arg_type_end(); ++i) {
                os << ", ";
                HandleQualType(*i);
            }
            os << ")";
            os << ": ";
            HandleQualType(ft->getResultType());
        } else if (t->isPointerType()) {
            HandleQualType(t->getPointeeType());
            os << "*";
        } else if (t->isRecordType()) {
            const clang::RecordType* st = t->getAs<clang::RecordType>();
            os << st->getDecl()->getNameAsString();
        } else if (t->isBuiltinType()) {
            const clang::BuiltinType* bt = t->getAs<clang::BuiltinType>();
            switch (bt->getKind()) {
                case clang::BuiltinType::Void: os << "void"; break;
                case clang::BuiltinType::Bool: os << "bool"; break;
                    
                case clang::BuiltinType::SChar:
                case clang::BuiltinType::Char_S: os << "int8"; break;
                    
                case clang::BuiltinType::UChar:
                case clang::BuiltinType::Char_U: os << "uint8"; break;
                    
                case clang::BuiltinType::Short: os << "int16"; break;
                    
                case clang::BuiltinType::Char16:
                case clang::BuiltinType::UShort: os << "uint16"; break;
                    
                case clang::BuiltinType::WChar_S:
                case clang::BuiltinType::Int: os << "int32"; break;
                    
                case clang::BuiltinType::WChar_U:
                case clang::BuiltinType::Char32:
                case clang::BuiltinType::UInt: os << "uint32"; break;
                    
                case clang::BuiltinType::LongLong:
                case clang::BuiltinType::Long: os << "int64"; break;
                    
                case clang::BuiltinType::ULongLong:
                case clang::BuiltinType::ULong: os << "uint64"; break;
                    
                case clang::BuiltinType::Half: os << "float16"; break;
                case clang::BuiltinType::Float: os << "float32"; break;
                case clang::BuiltinType::Double: os << "float64"; break;
                case clang::BuiltinType::LongDouble: os << "float128"; break;
                    
                case clang::BuiltinType::Int128: os << "int128"; break;
                case clang::BuiltinType::UInt128: os << "uint128"; break;
                    
                case clang::BuiltinType::NullPtr:
                case clang::BuiltinType::ObjCClass:
                case clang::BuiltinType::ObjCId:
                case clang::BuiltinType::ObjCSel:
                case clang::BuiltinType::Dependent:
                case clang::BuiltinType::Overload:
                case clang::BuiltinType::BoundMember:
                case clang::BuiltinType::PseudoObject:
                case clang::BuiltinType::UnknownAny:
                case clang::BuiltinType::ARCUnbridgedCast:
                    assert(0);
                    os << "???";
                    break;
            }
        } else if (t->isArrayType()) {
            const clang::ArrayType* at = clang::dyn_cast<clang::ArrayType>(t);
            os << " /* " << t.getAsString() << " */ " << std::flush;
            
            HandleQualType(at->getElementType());
            os << "*";
            
        } else if (t->isEnumeralType()) {
            os << t.getAsString();
        }
        else {
            os << "void*  " << "/* " << t.getAsString() << " -> " << t.getTypePtr()->getTypeClassName() << " */" << std::flush;
        }
    }
    
    void HandleArgument(clang::ParmVarDecl* d) {
        std::string argname = d->getNameAsString();
        if (argname.empty()) {
            std::stringstream str;
            str << "arg" << ++this->autoarg;
            argname = str.str();
        }
        os << "val " << argname << ": ";
        HandleQualType(d->getType());
    }
    
    void HandleDecl(clang::Decl* d) {
        if (clang::FunctionDecl* fdecl = llvm::dyn_cast<clang::FunctionDecl>(d)) {
            os << "extern def " << fdecl->getNameAsString() << "(";
            auto i = fdecl->param_begin();
            if (i != fdecl->param_end()) {
                HandleArgument(*i);
                ++i;
            }
            for (; i != fdecl->param_end(); ++i) {
                os << ", ";
                HandleArgument(*i);
            }
            os << ") : ";
            HandleQualType(fdecl->getResultType());
            os << ";" << std::endl;
        }
        else if (clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(d)) {
            if( vd->isFileVarDecl() && !vd->hasExternalStorage() )
            {
                std::cerr << "Read top-level variable decl: '";
                std::cerr << vd->getDeclName().getAsString() ;
                std::cerr << std::endl;
            }
        }
        else if (clang::RecordDecl* rd = llvm::dyn_cast<clang::RecordDecl>(d)) {
            std::string name = rd->getNameAsString();
            os << "struct " << name << " {" << std::endl;
            
            for (auto i = rd->field_begin(); i != rd->field_end(); ++i) {
                HandleDecl(*i);
            }
            
            os << "}" << std::endl;
            
        }
        else if (clang::FieldDecl* fd = llvm::dyn_cast<clang::FieldDecl>(d)) {
            os << "    var " << fd->getNameAsString() << ": ";
            HandleQualType(fd->getType());
            os << ";" << std::endl;
        }
        autoarg = 0;
    }
    
    bool HandleTopLevelDecl(clang::DeclGroupRef d) {
        static int count = 0;
        clang::DeclGroupRef::iterator it;
        for( it = d.begin(); it != d.end(); it++)
        {
            HandleDecl(*it);
            count++;
        }
        return true;
    }
    
    clang::ASTContext* ctx;
    std::ostream& os;
    int autoarg;
};


po::variables_map args;

int main(int argc, const char** argv) {
    llvm::sys::PrintStackTraceOnErrorSignal();
    llvm::PrettyStackTraceProgram X(argc, argv);
    
    po::options_description genericdesc("Options");
	genericdesc.add_options()
    ("help,h", "Show help message")
    ("output,o", po::value<std::string>(), "Output path")
    ("include-path,I", po::value<std::vector<std::string> >()->composing(), "Additional include paths")
    ;
    
	po::options_description hiddendesc("Hidden Options");
	hiddendesc.add_options()
    ("input-file", po::value<std::string>(), "input file")
    ;
    
	po::positional_options_description p;
	p.add("input-file", -1);
    
	po::options_description alldesc("All Options");
	alldesc.add(genericdesc).add(hiddendesc);
    
    
	try {
		po::store(po::command_line_parser(argc, argv).options(alldesc).positional(p).run(), args);
		po::notify(args);
	}
	catch (const std::exception& ex) {
		std::cerr << "error " << ex.what() << std::endl;
		return 1;
	}
    
	if (args.count("help")) {
		std::cout << genericdesc << std::endl;
		return 0;
	}
    
    clang::CompilerInstance ci;
    ci.createDiagnostics(argc, argv);
    
    const std::vector<std::string>& include_dirs = args["include-path"].as<std::vector<std::string> >();
    for (auto i = include_dirs.begin(); i != include_dirs.end(); ++i) {
        ci.getHeaderSearchOpts().AddPath(*i, clang::frontend::Angled, true, false, false);
    }
    
    clang::TargetOptions opts;
    opts.Triple = llvm::sys::getDefaultTargetTriple();
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), opts);
    ci.setTarget(pti);
    
    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
    ci.createPreprocessor();
    ci.getPreprocessorOpts().UsePredefines = false;
    
    std::fstream os;
    IdesASTPrinter *astConsumer = NULL;
    if (args.count("output")) {
        std::string output_file = args["output"].as<std::string>();
        os.open(output_file.c_str(), std::ios_base::out | std::ios_base::trunc);
        
        astConsumer = new IdesASTPrinter(os);
        ci.setASTConsumer(astConsumer);
        
    }
    else {
        astConsumer = new IdesASTPrinter(std::cout);
        ci.setASTConsumer(astConsumer);
    }
    
    ci.createASTContext();
    
    
    
    const clang::FileEntry *pFile = ci.getFileManager().getFile(args["input-file"].as<std::string>());
    ci.getSourceManager().createMainFileID(pFile);
    ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(),
                                             &ci.getPreprocessor());
    clang::ParseAST(ci.getPreprocessor(), astConsumer, ci.getASTContext());
    ci.getDiagnosticClient().EndSourceFile();
    
    if (os.is_open()) os.close();
    
    return 0;
}
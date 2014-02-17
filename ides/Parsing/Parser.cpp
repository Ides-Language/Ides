//
//  Parser.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#define BOOST_SPIRIT_DEBUG

#include "Parser.h"

int yyparse (Ides::Parser* context, Ides::AstBase** program);

Ides::Parser::Parser() : currentFile(NULL) {
    yylex_init(&scanner);
}

Ides::Parser::~Parser() {
    yylex_destroy(scanner);
}

Ides::AstPtr Ides::Parser::Parse(const Ides::SourcePackage& pkg) { SETTRACE("Parse(Parser::SourcePackage)")
    Ides::AstPtr contents = Parse(pkg.GetRoot());
    return Ides::AstPtr(new Ides::ModuleDecl(NULL,
                                             new Ides::Name(pkg.GetProperty<Ides::String>("name")),
                                             (Ides::ExprList*)contents.release()));
}

Ides::AstPtr Ides::Parser::Parse(const Ides::SourceDirectory& dir) { SETTRACE("Parse(Parser::SourceDirectory)")
    DBG("Parsing dir:  " << dir.GetPath());
    Ides::ExprList* modules = new Ides::ExprList();

    try {
        for (auto& i : dir.GetDirs()) {
            modules->Add(new Ides::ModuleDecl(NULL, new Ides::Name(i->dirname), (Ides::ExprList*)Parse(*i).release()));
        }

        for (auto& i : dir.GetFiles()) {
            Ides::AstPtr fileMembers = Parse(*i);
            for (auto& fi : static_cast<Ides::ExprList*>(fileMembers.get())->items) {
                modules->Add(fi.release());
            }
            fileMembers.reset();
        }
    } catch (...) {
        delete modules;
        throw;
    }

    return Ides::AstPtr(modules);
}

Ides::AstPtr Ides::Parser::Parse(const Ides::SourceFile& file) { SETTRACE("Parse(Parser::SourceFile)")
    DBG("Parsing file: " << file.GetPath());
    this->currentFile = &file;
    Ides::AstPtr ret = Parse(file.GetBuffer());
    this->currentFile = NULL;
    return ret;
}

Ides::AstPtr Ides::Parser::Parse(const llvm::MemoryBuffer& buf) { SETTRACE("Parse(llvm::MemoryBuffer)")
    return Parse(llvm::StringRef(buf.getBufferStart(), buf.getBufferSize()));
}

Ides::AstPtr Ides::Parser::Parse(llvm::StringRef source) { SETTRACE("Parse(llvm::StringRef)")
    readOffset = 0;

    auto bufferState = yy_scan_bytes(source.data(), source.size(), scanner);
    yyset_debug(1, scanner);
    yyset_lineno(1, scanner);

    Ides::Ast* ret = NULL;
    try {
        yyparse(this, &ret);
    }
    catch (...) {
        yy_delete_buffer(bufferState, scanner);
        throw;
    }

    yy_delete_buffer(bufferState, scanner);

    return Ides::AstPtr(ret);
}

//
//  Parser.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#define BOOST_SPIRIT_DEBUG

#include "Parser.h"

size_t Ides::Ast::count = 0;

int yyparse (Ides::Parser* context, Ides::Ast** program);

Ides::Parser::Parser() {
    yylex_init(&scanner);
}

Ides::Parser::~Parser() {
    yylex_destroy(scanner);
}


Ides::AstPtr Ides::Parser::Parse(const Ides::SourceFile& file) {
    return Parse(file.GetBuffer());
}

Ides::AstPtr Ides::Parser::Parse(const llvm::MemoryBuffer& buf) {
    return Parse(llvm::StringRef(buf.getBufferStart(), buf.getBufferSize()));
}

Ides::AstPtr Ides::Parser::Parse(llvm::StringRef source) {
    readOffset = 0;

    auto bufferState = yy_scan_bytes(source.data(), source.size(), scanner);
    yyset_debug(1, scanner);

    Ides::Ast* ret = NULL;
    try {
        yyparse(this, &ret);
    }
    catch (const std::exception& ex) {
        yy_delete_buffer(bufferState, scanner);
        throw ex;
    }

    yy_delete_buffer(bufferState, scanner);

    return Ides::AstPtr(ret);
}

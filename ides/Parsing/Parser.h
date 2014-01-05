//
//  Parser.h
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#ifndef __ides__Parser__
#define __ides__Parser__

#include <ides/common.h>
#include <ides/Source/SourceLocation.h>
#include <ides/Source/SourcePackage.h>
#include <ides/Parsing/AST.h>

#include <ides/lexer.hpp>
#include <ides/parser.hpp>


namespace Ides {
    typedef std::unique_ptr<Ides::Ast> AstPtr;

    class Parser {
    public:
        Parser();

        ~Parser();

        AstPtr Parse(const Ides::SourcePackage& pkg);
        AstPtr Parse(const Ides::SourceDirectory& dir);
        AstPtr Parse(const Ides::SourceFile& file);
        AstPtr Parse(llvm::StringRef source);
        AstPtr Parse(const llvm::MemoryBuffer& buf);

        yyscan_t GetScanner() const { return scanner; }

    private:
        size_t readOffset;
        yyscan_t scanner;
    };
}

#endif /* defined(__ides__Parser__) */

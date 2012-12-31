//
//  DeclarationContext.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__DeclarationContext__
#define __ides__DeclarationContext__


#include <ides/AST/AST.h>

namespace Ides {
namespace AST {
    class Declaration;
    
    class DeclarationContext : public std::list<boost::shared_ptr<Declaration> > {
    public:
    };
    
    class CompilationUnit : public AST, public DeclarationContext {
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
    };
    
}
}

#endif /* defined(__ides__DeclarationContext__) */

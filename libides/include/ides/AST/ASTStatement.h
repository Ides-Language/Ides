//
//  ASTStatement.h
//  ides
//
//  Created by Sean Edwards on 12/14/12.
//
//

#ifndef ides_ASTStatement_h
#define ides_ASTStatement_h

#include <ides/AST/AST.h>

namespace Ides {
namespace AST {

    class ASTStatement : public AST {
        
    };
    
    class ASTCompoundStatement : public ASTList {
    public:
        ASTCompoundStatement() { }
        virtual ~ASTCompoundStatement() { }
        
        virtual Ides::String GetDOT() const;
    };
    
    class ASTIfStatement : public ASTStatement {
        
    };
    
    
}
}

#endif

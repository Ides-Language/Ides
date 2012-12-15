//
//  ASTConstantExpression.h
//  ides
//
//  Created by Sean Edwards on 12/14/12.
//
//

#ifndef ides_ASTConstantExpression_h
#define ides_ASTConstantExpression_h

#include <ides/AST/ASTExpression.h>

namespace Ides {
namespace AST {

    class ASTConstantExpression : public AST {
    public:
    };
    
    
    
    /* Integral constant expression types */
    
    class ASTConstantIntExpression : public ASTConstantExpression {
    public:
        ASTConstantIntExpression(int64_t v) : val(v) { }

        virtual Ides::String GetDOT() const;
    private:
        int64_t val;
    };

    
    
    /* String constant expression types */
    
    class ASTConstantStringExpression : public ASTConstantExpression {
        
    };
    
    class ASTConstantCStringExpression : public ASTConstantStringExpression {
        
    };
    
    class ASTConstantWCStringExpression : public ASTConstantStringExpression {
        
    };
    
    class ASTConstantLCStringExpression : public ASTConstantStringExpression {
        
    };
    
    
    
}; // namespace AST
}; // namespace Ides

#endif

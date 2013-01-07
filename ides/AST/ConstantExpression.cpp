//
//  ConstantExpression.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "ConstantExpression.h"
#include <ides/ASTVisitor/ASTVisitor.h>

namespace Ides {
namespace AST {
    
    
    template<> void ConstantBuiltinTypeExpression<Ides::Types::Integer1Type, bool>::Accept(Visitor* v) { v->Visit(this); }
    void ConstantIntExpression::Accept(Visitor* v) { v->Visit(this); }
    template<> void ConstantBuiltinTypeExpression<Ides::Types::Float64Type, double>::Accept(Visitor* v) { v->Visit(this); }
    
    void ConstantStringExpression::Accept(Visitor* v) { v->Visit(this); }
    void ConstantCStringExpression::Accept(Visitor* v) { v->Visit(this); }
    void ConstantWCStringExpression::Accept(Visitor* v) { v->Visit(this); }
    void ConstantLCStringExpression::Accept(Visitor* v) { v->Visit(this); }
    
}
}
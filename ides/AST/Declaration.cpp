//
//  Declaration.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Declaration.h"
#include <ides/ASTVisitor/ASTVisitor.h>

namespace Ides {
namespace AST {
    
    const Ides::Types::Type* FunctionDeclaration::GetReturnType(ASTContext &ctx) {
        if (val) return val->GetType(ctx);
        else if (returntype) return returntype->GetType(ctx);
        return Ides::Types::VoidType::GetSingletonPtr();
    }
    
    const Ides::Types::Type* FunctionDeclaration::GetType(ASTContext& ctx)    {
        class RecursiveTypeEvalException {
        public:
            RecursiveTypeEvalException(clang::SourceRange loc) : loc(loc) {}
            virtual ~RecursiveTypeEvalException() throw() {}
            
            const clang::SourceRange loc;
        };
        
        // No need to re-calculate the type.
        if (functype != NULL) return functype;
        
        if (evaluatingtype) {
            throw RecursiveTypeEvalException(this->val->exprloc);
        }
        evaluatingtype = true;
        
        std::vector<const Ides::Types::Type*> argTypes;
        for (auto i = this->args.begin(); i != this->args.end(); ++i) {
            argTypes.push_back((*i)->GetType(ctx));
        }
        
        const Ides::Types::Type* ret = NULL;
        
        if (this->returntype == NULL) {
            if (val == NULL) ret = Ides::Types::VoidType::GetSingletonPtr();
            else {
                try {
                    ret = val->GetType(ctx);
                } catch (const RecursiveTypeEvalException& ex) {
                    this->evaluatingtype = false;
                    throw RecursiveTypeEvalException(this->val->exprloc);
                }
            }
        } else {
            ret = this->returntype->GetType(ctx);
        }
        
        evaluatingtype = false;
        assert(ret != NULL);
        this->functype = Ides::Types::FunctionType::Get(ret, argTypes, this->isVarArgs);
        return this->functype;
    }
    
    Ides::String FunctionDeclaration::GetMangledName() const {
        if (Attribute* nomangle = this->GetAttribute("NoMangle")) {
            return this->GetName();
        }
        if (Attribute* mangle = this->GetAttribute("Mangle")) {
            if (mangle->GetArgs().size() == 1) {
                if (ConstantStringExpression* expr = dynamic_cast<ConstantStringExpression*>(mangle->GetArgs()[0])) {
                    return expr->GetString();
                }
            }
        }
        return "Ides$" + this->GetName();
    }
    
    void Attribute::Accept(Visitor* v) { v->Visit(this); }
    void ValueDeclaration::Accept(Visitor* v) { v->Visit(this); }
    void VariableDeclaration::Accept(Visitor* v) { v->Visit(this); }
    void GlobalVariableDeclaration::Accept(Visitor* v) { v->Visit(this); }
    void ArgumentDeclaration::Accept(Visitor* v) { v->Visit(this); }
    void FunctionDeclaration::Accept(Visitor* v) { v->Visit(this); }
    void OverloadedFunction::Accept(Visitor* v) { v->Visit(this); }
    void StructDeclaration::Accept(Visitor* v) { v->Visit(this); }
    void FieldDeclaration::Accept(Visitor* v) { v->Visit(this); }
}
}
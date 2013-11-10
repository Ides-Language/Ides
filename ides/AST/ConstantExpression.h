//
//  ConstantExpression.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__ConstantExpression__
#define __ides__ConstantExpression__

#include <ides/AST/Expression.h>
#include <ides/AST/AST.h>

namespace Ides {
namespace AST {
    
    class ConstantExpression : public Expression {
    public:
        virtual ~ConstantExpression() { }
    };
    
    template<typename Tt, typename Vt>
    class ConstantBuiltinTypeExpression : public ConstantExpression {
    public:
        ConstantBuiltinTypeExpression(Vt v) : val(v) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Tt::GetSingletonPtr();
        }
        
        Vt GetValue() const { return val; }
    private:
        Vt val;
    };
    
    typedef ConstantBuiltinTypeExpression<Ides::Types::Integer1Type, bool> ConstantBoolExpression;
    typedef ConstantBuiltinTypeExpression<Ides::Types::Float64Type, double> ConstantFloatExpression;
    typedef ConstantBuiltinTypeExpression<Ides::Types::Integer32Type, uint64_t> ConstantIntExpression;

    
    class ConstantStringExpression : public ConstantExpression {
    public:
        virtual void Accept(Visitor* v);
        
        Ides::String GetString() { return buf.str(); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            throw Ides::AST::TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, this->exprloc);
        }
        
        std::stringstream& GetBuffer() { return buf; }
    protected:
        std::stringstream buf;
    };
    
    class ConstantCStringExpression : public ConstantStringExpression {
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::PointerType::Get(Ides::Types::Integer8Type::GetSingletonPtr());
        }
    };
    
    class ConstantWCStringExpression : public ConstantStringExpression {
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::PointerType::Get(Ides::Types::Integer16Type::GetSingletonPtr());
        }
    };
    
    class ConstantLCStringExpression : public ConstantStringExpression {
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::PointerType::Get(Ides::Types::Integer32Type::GetSingletonPtr());
        }
    };

}
}

#endif /* defined(__ides__ConstantExpression__) */

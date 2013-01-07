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
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Tt::GetSingletonPtr();
        }
        
        Vt GetValue() const { return val; }
    private:
        Vt val;
    };
    
    typedef ConstantBuiltinTypeExpression<Ides::Types::Integer1Type, bool> ConstantBoolExpression;
    typedef ConstantBuiltinTypeExpression<Ides::Types::FloatLiteralType, double> ConstantFloatExpression;
    
    class ConstantIntExpression : public ConstantExpression {
    public:
        ConstantIntExpression(uint64_t v) : val(v) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            if (val <= INT8_MAX) return Ides::Types::IntegerLiteralType<8>::GetSingletonPtr();
            else if (val <= INT16_MAX) return Ides::Types::IntegerLiteralType<16>::GetSingletonPtr();
            else if (val <= INT32_MAX) return Ides::Types::IntegerLiteralType<32>::GetSingletonPtr();
            else if (val <= INT64_MAX) return Ides::Types::IntegerLiteralType<64>::GetSingletonPtr();
            return Ides::Types::UInteger64Type::GetSingletonPtr();
        }
        
        uint64_t GetValue() const { return val; }
    private:
        uint64_t val;
    };

    
    class ConstantStringExpression : public ConstantExpression {
    public:
        virtual void Accept(Visitor* v);
        
        Ides::String GetString() { return buf.str(); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return NULL;
        }
        
        std::stringstream& GetBuffer() { return buf; }
    protected:
        std::stringstream buf;
    };
    
    class ConstantCStringExpression : public ConstantStringExpression {
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::PointerType::Get(Ides::Types::Integer8Type::GetSingletonPtr());
        }
    };
    
    class ConstantWCStringExpression : public ConstantStringExpression {
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::PointerType::Get(Ides::Types::Integer16Type::GetSingletonPtr());
        }
    };
    
    class ConstantLCStringExpression : public ConstantStringExpression {
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::PointerType::Get(Ides::Types::Integer32Type::GetSingletonPtr());
        }
    };

}
}

#endif /* defined(__ides__ConstantExpression__) */

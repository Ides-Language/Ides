//
//  ASTConstantExpression.h
//  ides
//
//  Created by Sean Edwards on 12/14/12.
//
//

#ifndef ides_ASTConstantExpression_h
#define ides_ASTConstantExpression_h

#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <ides/AST/ASTExpression.h>

namespace Ides {
namespace AST {

    class ASTConstantExpression : public AST {
    public:
        virtual ~ASTConstantExpression() { }
    };
    
    
    
    /* Integral constant expression types.
     *
     * Note: constant ints store an unsigned value, because negative constants
     * are handled automatically by the unary - operator.
     */
    
    class ASTConstantIntExpression : public ASTConstantExpression {
    public:
        ASTConstantIntExpression(uint64_t v) : val(v) { }
        virtual ~ASTConstantIntExpression() { }
        
        virtual Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::Integer32Type::GetSingletonPtr();
        }
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            return llvm::ConstantInt::get(this->GetType(ctx)->GetLLVMType(ctx), val);
        }
    private:
        uint64_t val;
    };
    
    class ASTConstantFloatExpression : public ASTConstantExpression {
    public:
        ASTConstantFloatExpression(double v) : val(v) { }
        virtual ~ASTConstantFloatExpression() { }
        
        virtual Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::Float64Type::GetSingletonPtr();
        }
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            return llvm::ConstantFP::get(this->GetType(ctx)->GetLLVMType(ctx), val);
        }
    private:
        double val;
    };
    
    class ASTConstantCharExpression : public ASTConstantExpression {
    public:
        ASTConstantCharExpression(uint8_t v) : val(v) { }
        virtual ~ASTConstantCharExpression() { }
        
        virtual Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::Integer8Type::GetSingletonPtr();
        }
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            return llvm::ConstantInt::get(this->GetType(ctx)->GetLLVMType(ctx), val);
        }
    private:
        uint8_t val;
    };

    
    
    /* String constant expression types */
    
    class ASTConstantStringExpression : public ASTConstantExpression {
    public:
        ASTConstantStringExpression(const Ides::String& str) : val(str) { }
        virtual ~ASTConstantStringExpression() { }
        
    protected:
        Ides::String val;
    };
    
    class ASTConstantCStringExpression : public ASTConstantStringExpression {
    public:
        ASTConstantCStringExpression(const Ides::String& str) : ASTConstantStringExpression(str) { }
        virtual ~ASTConstantCStringExpression() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            throw Ides::Diagnostics::CompileError("Cstrings not yet implemented.", this->exprloc);
            /*static llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx.GetIRBuilder()->getContext()), 0);
            static llvm::Value *Args[] = { zero, zero };
            
            llvm::Value* str = llvm::ConstantDataArray::getString(ctx.GetIRBuilder()->getContext(), val, true);
            return ctx.GetIRBuilder()->CreateInBoundsGEP(str, Args);*/
        }
        
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::PointerType::Get(Ides::Types::Integer8Type::GetSingletonPtr());
        }
    };
    
    class ASTConstantWCStringExpression : public ASTConstantStringExpression {
    public:
        ASTConstantWCStringExpression(const Ides::String& str) : ASTConstantStringExpression(str) { }
        virtual ~ASTConstantWCStringExpression() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            throw Ides::Diagnostics::CompileError("WCstrings not yet implemented.", this->exprloc);
        }
        
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::PointerType::Get(Ides::Types::Integer16Type::GetSingletonPtr());
        }
    };
    
    class ASTConstantLCStringExpression : public ASTConstantStringExpression {
    public:
        ASTConstantLCStringExpression(const Ides::String& str) : ASTConstantStringExpression(str) { }
        virtual ~ASTConstantLCStringExpression() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            throw Ides::Diagnostics::CompileError("LCstrings not yet implemented.", this->exprloc);
        }
        
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::PointerType::Get(Ides::Types::Integer32Type::GetSingletonPtr());
        }
    };
    
    
    
}; // namespace AST
}; // namespace Ides

#endif

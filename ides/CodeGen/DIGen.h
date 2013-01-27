//
//  DIGen.h
//  ides
//
//  Created by Sean Edwards on 1/12/13.
//
//

#ifndef __ides__DIGen__
#define __ides__DIGen__

#include <ides/common.h>
#include <ides/ASTVisitor/ASTVisitor.h>
#include <ides/Types/TypeVisitor.h>

#include <llvm/DebugInfo.h>

namespace Ides {
namespace CodeGen {
    
    class DIGenerator : public llvm::DIBuilder, public Ides::Types::TypeVisitor {
    public:
        DIGenerator(llvm::Module& mod) :
            llvm::DIBuilder(mod) { }
        
        virtual ~DIGenerator() { }
        
        const llvm::MDNode* GetLast() const { return last; }
        
        llvm::DIType GetType(const Ides::Types::Type* t) {
            auto i = typemd.find(t);
            if (i != typemd.end()) last = i->second;
            else t->Accept(this);
            llvm::DIType dit(last);
            assert(dit.Verify());
            return dit;
        }
        
        const llvm::MDNode* GetCurrentScope() { return scopeStack.top(); }
        
        class DebugScope {
        public:
            DebugScope(DIGenerator* gen, const llvm::MDNode* node) : gen(NULL) {
                SetScope(gen, node);
            }
            
            DebugScope() : gen(NULL) { }
            
            void SetScope(DIGenerator* gen, const llvm::MDNode* node) {
                if (this->gen != NULL) gen->scopeStack.pop();
                assert(llvm::DIScope(node).Verify());
                this->gen = gen;
                if (gen != NULL) gen->scopeStack.push(node);
            }
            
            ~DebugScope() {
                if (gen != NULL)
                    gen->scopeStack.pop();
            }
        private:
            DIGenerator* gen;
        };
    public:
        
        virtual void Visit(const Ides::Types::VoidType* ty) { last = NULL; }
        virtual void Visit(const Ides::Types::UnitType* ty) { last = NULL; }
        
        virtual void Visit(const Ides::Types::PointerType* ty) {
            last = createPointerType(GetType(ty->GetTargetType()), 64, 64, ty->ToString());
        }
        virtual void Visit(const Ides::Types::FunctionType* ty);
        virtual void Visit(const Ides::Types::OverloadedFunctionType* ty) { }
        
        virtual void Visit(const Ides::Types::StructType* ty);
        virtual void Visit(const Ides::Types::ClassType* ty) { }
        virtual void Visit(const Ides::Types::ReferenceType* ty) { }
        
        virtual void Visit(const Ides::Types::Integer1Type* ty) {
            last = createBasicType(ty->ToString(), 1, 1, llvm::dwarf::DW_ATE_boolean);
        }
        
        virtual void Visit(const Ides::Types::Integer8Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::UInteger8Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_unsigned);
        }
        virtual void Visit(const Ides::Types::Integer16Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::UInteger16Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_unsigned);
        }
        virtual void Visit(const Ides::Types::Integer32Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::UInteger32Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_unsigned);
        }
        virtual void Visit(const Ides::Types::Integer64Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::UInteger64Type* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_unsigned);
        }
        
        virtual void Visit(const Ides::Types::Float32Type* ty) { }
        virtual void Visit(const Ides::Types::Float64Type* ty) { }
        
        virtual void Visit(const Ides::Types::IntegerLiteralType<8>* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::IntegerLiteralType<16>* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::IntegerLiteralType<32>* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        virtual void Visit(const Ides::Types::IntegerLiteralType<64>* ty) {
            last = createBasicType(ty->ToString(), ty->GetSize(), ty->GetAlignment(), llvm::dwarf::DW_ATE_signed);
        }
        
        
    private:
        boost::unordered_map<const Ides::Types::Type*, llvm::MDNode*> typemd;
        const llvm::MDNode* last;
        
        std::stack<const llvm::MDNode*> scopeStack;
    };
    
}
}

#endif /* defined(__ides__DIGen__) */

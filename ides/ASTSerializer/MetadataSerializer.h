//
//  MetadataSerializer.h
//  ides
//
//  Created by Sean Edwards on 1/8/13.
//
//

#ifndef __ides__MetadataSerializer__
#define __ides__MetadataSerializer__

#include <ides/ASTVisitor/ASTVisitor.h>
#include <ides/Types/TypeVisitor.h>

namespace Ides {
namespace AST {
    
    class MetadataSerializer : public Visitor, public Ides::Types::TypeVisitor {
    public:
        
        MetadataSerializer(Ides::AST::ASTContext& actx, llvm::LLVMContext& lctx) : actx(actx), lctx(lctx) { }
        virtual ~MetadataSerializer() { }
        
        llvm::MDNode* GetMDValue(AST* ast);
        llvm::MDNode* GetMDValue(const Ides::Types::Type* type);
    private:
        class NodeHelper {
        public:
            NodeHelper(MetadataSerializer* md) : md(md) { }

            operator llvm::MDNode* () const { return llvm::MDNode::get(md->lctx, args); }

            NodeHelper& Add(llvm::StringRef str) {
                args.push_back(llvm::MDString::get(md->lctx, str));
                return *this;
            }

            NodeHelper& Add(llvm::MDNode* node) {
                args.push_back(node);
                return *this;
            }

            NodeHelper& Add(const Ides::Types::Type* ty) {
                return Add(md->GetMDValue(ty));
            }

            NodeHelper& Add(Ides::AST::AST* ast) {
                return Add(md->GetMDValue(ast));
            }

            template<typename T>
            NodeHelper& Add(const std::vector<T>& ast) {
                NodeHelper n(md);
                for (auto i = ast.begin(); i != ast.end(); ++i) {
                    n.Add(*i);
                }
                return Add(n);
            }

            template<typename T1, typename T2>
            NodeHelper& Add(const std::pair<T1, T2>& pair) {
                return Add(
                           md->node()
                           .Add(pair.first)
                           .Add(pair.second)
                           );
            }
        private:
            std::vector<llvm::Value*> args;
            MetadataSerializer* md;

        };

        NodeHelper node() { return NodeHelper(this); }

        llvm::MDNode* last;
        
        boost::unordered_map<AST*, llvm::MDNode*> mdnodes;
        boost::unordered_map<const Ides::Types::Type*, llvm::MDNode*> mdtypes;

        Ides::AST::ASTContext& actx;
        llvm::LLVMContext& lctx;


    public:
        using Visitor::Visit;
        using Ides::Types::TypeVisitor::Visit;
        
        virtual void Visit(Ides::AST::CompilationUnit* ast) { }

        virtual void Visit(Ides::AST::Attribute* ast) { }
        virtual void Visit(Ides::AST::ValueDeclaration* ast) { }
        virtual void Visit(Ides::AST::VariableDeclaration* ast) { }
        virtual void Visit(Ides::AST::GlobalVariableDeclaration* ast) { }
        virtual void Visit(Ides::AST::ArgumentDeclaration* ast) { }
        virtual void Visit(Ides::AST::StructDeclaration* ast);
        virtual void Visit(Ides::AST::FieldDeclaration* ast) { }
        virtual void Visit(Ides::AST::FunctionDeclaration* ast);
        virtual void Visit(Ides::AST::OverloadedFunction* ast);
        


        
        virtual void Visit(const Ides::Types::VoidType* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::UnitType* ty) { last = node().Add(ty->ToString()); }

        virtual void Visit(const Ides::Types::PointerType* ty) { last = node().Add("ptr").Add(GetMDValue(ty->GetTargetType())); }
        virtual void Visit(const Ides::Types::FunctionType* ty);

        virtual void Visit(const Ides::Types::StructType* ty);
        virtual void Visit(const Ides::Types::ClassType* ty) { }
        virtual void Visit(const Ides::Types::ReferenceType* ty) { }

        virtual void Visit(const Ides::Types::Integer1Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::Integer8Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::UInteger8Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::Integer16Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::UInteger16Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::Integer32Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::UInteger32Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::Integer64Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::UInteger64Type* ty) { last = node().Add(ty->ToString()); }

        virtual void Visit(const Ides::Types::Float32Type* ty) { last = node().Add(ty->ToString()); }
        virtual void Visit(const Ides::Types::Float64Type* ty) { last = node().Add(ty->ToString()); }
    };
    
}
}

#endif /* defined(__ides__MetadataSerializer__) */

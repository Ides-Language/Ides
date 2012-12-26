#ifndef _IDES_AST_NODE_H_
#define _IDES_AST_NODE_H_

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/unordered_map.hpp>

#include <ides/common.h>
#include <ides/Diagnostics/SourceLocation.h>
#include <ides/Types/Type.h>

#include <ides/Parsing/Parser.h>

namespace Ides {
namespace AST {
    class AST;
    class ASTIdentifier;
    
    typedef Ides::Parsing::ParseContext ParseContext;
    
    class UnitValueException { };
    
    enum Specifier {
        // Low order bits = visibility
        PUBLIC = 0,
        PROTECTED = 1,
        INTERNAL = 2,
        PRIVATE = 3,
        
        MASK_VISIBILITY = 7,
        
        EXTERN = 8,
        CONST = 16,
    };
    
    class AST {
    public:
        
        AST();
        virtual ~AST() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) { assert(0); }
        virtual llvm::Value* GetValue(ParseContext& ctx, const Ides::Types::Type* to) {
            return this->GetType(ctx)->Convert(ctx, this->GetValue(ctx), to);
        }
        virtual llvm::Value* GetPointerValue(ParseContext& ctx) {
            throw Ides::Diagnostics::CompileError("expression is not a pointer", this->exprloc);
        }
        llvm::Value* GetMDNode(ParseContext& ctx) { return mdnode ? mdnode : (mdnode = CreateMDNode(ctx)); }
        virtual llvm::Value* CreateMDNode(ParseContext& ctx) { assert(0); }
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { assert(0); }
        
        Diagnostics::SourceLocation exprloc;
    private:
        llvm::Value* mdnode;
    };
    
    class ASTList : public AST, public std::list<AST*> {
    public:
        ~ASTList();
        
        virtual llvm::Value* CreateMDNode(ParseContext& ctx);
    };
    
    class ASTIdentifier : public AST {
    public:
        ASTIdentifier (const Ides::String& name) : name(name) {}
        virtual ~ASTIdentifier() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) { return GetDeclaration(ctx)->GetValue(ctx); }
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return GetDeclaration(ctx)->GetType(ctx); }
        virtual llvm::Value* GetValue(ParseContext& ctx, const Ides::Types::Type* to) { return GetDeclaration(ctx)->GetValue(ctx, to); }
        virtual llvm::Value* GetPointerValue(ParseContext& ctx) { return GetDeclaration(ctx)->GetPointerValue(ctx); }
        
        AST* GetDeclaration(ParseContext& ctx);
        
        const Ides::String name;
    };
    
    class ASTType : public AST {
    public:
        ASTType() : isConst(false) { }
        virtual ~ASTType() { }
        
        void SetConst(bool constness) { isConst = constness; }
        bool isConst;
    };
    
    class ASTVoidType : public ASTType {
    public:
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return Ides::Types::VoidType::GetSingletonPtr(); }
    };
    
    class ASTUnitType : public ASTType {
    public:
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return Ides::Types::UnitType::GetSingletonPtr(); }
    };
    
#define ASTINTTYPE(size) class ASTInteger##size##Type : public ASTType { \
    public: \
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return Ides::Types::Integer##size##Type::GetSingletonPtr(); } \
    }
#define ASTUINTTYPE(size) class ASTUInteger##size##Type : public ASTType { \
    public: \
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return Ides::Types::UInteger##size##Type::GetSingletonPtr(); } \
    }
    
    ASTINTTYPE(1);
    ASTINTTYPE(8);
    ASTUINTTYPE(8);
    ASTINTTYPE(16);
    ASTUINTTYPE(16);
    ASTINTTYPE(32);
    ASTUINTTYPE(32);
    ASTINTTYPE(64);
    ASTUINTTYPE(64);
    
    class ASTFloat32Type : public ASTType {
    public:
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return Ides::Types::Float32Type::GetSingletonPtr(); }
    };
    
    class ASTFloat64Type : public ASTType {
    public:
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) { return Ides::Types::Float64Type::GetSingletonPtr(); }
    };
    
    class ASTPtrType : public ASTType {
    public:
        ASTPtrType(ASTType* type) : basetype(type) { }
        ~ASTPtrType() { delete basetype; }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            return NULL;
        }
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) {
            return Ides::Types::PointerType::Get(basetype->GetType(ctx));
        }
        
        ASTType* basetype;
    };
    
    class ASTFunctionType : public ASTType {
    public:
        ASTFunctionType(ASTList* argtypes, ASTType* rettype) : rettype(rettype), argtypes(argtypes) { }
        virtual ~ASTFunctionType() { delete rettype; delete argtypes; }
        
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTType* rettype;
        ASTList* argtypes;
    };
    
    class ASTTypeName : public ASTType {
    public:
        ASTTypeName (ASTIdentifier* name) : name(name) { }
        virtual ~ASTTypeName() { delete name; }
        
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTIdentifier* name;
        
    };
    
    
    class ASTCompilationUnit : public ASTList {
    public:
        ASTCompilationUnit();
        virtual ~ASTCompilationUnit() { }
        
        void Compile(ParseContext& ctx);
    };
    
} // namespace AST
} // namespace Ides


#endif // _IDES_AST_NODE_H_


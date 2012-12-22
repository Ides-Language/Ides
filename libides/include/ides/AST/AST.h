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
    
    class UnitValueException : public std::exception { };
    
    class AST {
    public:
        
        AST();
        virtual ~AST() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx) {
            throw Ides::Diagnostics::CompileError("AST node not yet implemented.", this->exprloc);
        }
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) {
            throw Ides::Diagnostics::CompileError("AST node not yet implemented.", this->exprloc);
        }
        
        const boost::uuids::uuid& GetUUID() const { return this->uuid; }
        
        Diagnostics::SourceLocation exprloc;
    private:
        const boost::uuids::uuid uuid;
    };
    
    class ASTList : public AST, public std::list<AST*> {
    public:
        ~ASTList();
    };
    
    
    
    class ASTIdentifier : public AST {
    public:
        ASTIdentifier (const Ides::String& name) : name(name) {}
        virtual ~ASTIdentifier() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        const Ides::String name;
    };
    
    class ASTType : public AST {
    public:
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


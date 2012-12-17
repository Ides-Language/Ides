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


namespace Ides {
namespace AST {
    class AST;
    class ASTIdentifier;
    
    typedef boost::unordered_map<Ides::String, AST* > SymbolMap;
    
    class IDES_EXPORTS SymbolTable : public SymbolMap {
    public:
        AST* LookupRecursive(const Ides::String& symbol) const;
        AST* Lookup(const Ides::String& symbol) const;
        
        SymbolTable* GetParentScope() { return parentScope; }
        void SetParentScope(SymbolTable* scope) { parentScope = scope; }
    private:
        SymbolTable* parentScope;
    }; // class SymbolTable
    
    
    
    class AST : public Ides::Util::Graph {
    public:
        
        AST();
        virtual ~AST() { }
        
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) { assert(0); }
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) { assert(0); }
        
        virtual Ides::String GetCHeader() const { return ""; }
        const boost::uuids::uuid& GetUUID() const { return this->uuid; }
        
        
        SymbolTable symbols;
        
        Diagnostics::SourceLocation exprloc;
    private:
        const boost::uuids::uuid uuid;
    };
    
    
    
    class ASTIdentifier : public AST {
    public:
        ASTIdentifier (const Ides::String& name) : name(name) {}
        virtual ~ASTIdentifier() { }
        
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope);
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope);
        
        virtual Ides::String GetDOT() const;
        virtual Ides::String GetCHeader() const { return name; }
        
        const Ides::String name;
    };
    
    class ASTType : public AST {
    public:
    };
    
#define ASTINTTYPE(size) class ASTInteger##size##Type : public ASTType { \
    public: \
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) { return NULL; } \
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) { return Ides::Types::Integer##size##Type::GetSingletonPtr(); } \
        virtual Ides::String GetDOT() const; \
        virtual Ides::String GetCHeader() const { return "int" #size "_t"; } \
    }
    
    ASTINTTYPE(8);
    ASTINTTYPE(16);
    ASTINTTYPE(32);
    ASTINTTYPE(64);
    
    class ASTPtrType : public ASTType {
    public:
        ASTPtrType(ASTType* type) : basetype(type) { }
        ~ASTPtrType() { delete basetype; }
        
        virtual Ides::String GetDOT() const;
        virtual Ides::String GetCHeader() const { return basetype->GetCHeader() + "*"; }
        
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return Ides::Types::PointerType::Get(basetype->GetType(builder, scope));
        }
        
        ASTType* basetype;
    };
    
    class ASTTypeName : public ASTType {
    public:
        ASTTypeName (ASTIdentifier* name) : name(name) { }
        virtual ~ASTTypeName() { delete name; }
        
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope);
        
        virtual Ides::String GetDOT() const;
        virtual Ides::String GetCHeader() const { return name->GetCHeader(); }
        
        ASTIdentifier* name;
        
    };
    
    class ASTList : public AST, public std::list<AST*> {
    public:
        ~ASTList();
        
        virtual Ides::String GetDOT() const;
    };
    
    
    class ASTCompilationUnit : public ASTList {
    public:
        ASTCompilationUnit();
        virtual ~ASTCompilationUnit() { }
        
        void Compile(llvm::Module& mod);
        
        virtual Ides::String GetDOT() const;
        virtual Ides::String GetCHeader() const;
    };
    
} // namespace AST
} // namespace Ides


#endif // _IDES_AST_NODE_H_


//
//  DeclarationContext.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__DeclarationContext__
#define __ides__DeclarationContext__


//#include <ides/AST/AST.h>
#include <boost/enable_shared_from_this.hpp>
#include <ides/AST/ASTContext.h>
#include <map>

namespace Ides {
namespace AST {
    class Declaration;
    class NamedDeclaration;
    
    typedef std::map<Ides::String, Declaration*> SymbolTable;
    
    class DeclarationContext {
    public:
        virtual Declaration* GetMember(ASTContext& ctx, Ides::StringRef name) const = 0;
        virtual void AddMember(Ides::StringRef name, Declaration* decl) = 0;
    };
    
    class ConcreteDeclarationContext : public DeclarationContext {
    public:
        virtual Declaration* GetMember(ASTContext& ctx, Ides::StringRef name) const;
        virtual void AddMember(Ides::StringRef name, Declaration* decl);
        
        SymbolTable::const_iterator begin() const { return this->members.begin(); }
        SymbolTable::const_iterator end() const { return this->members.end(); }
        
    private:
        SymbolTable members;
        
    };
    
    class HierarchicalConcreteDeclarationContext : public ConcreteDeclarationContext {
    public:
        HierarchicalConcreteDeclarationContext() : parentContext(NULL) { }
        
        virtual Declaration* GetMember(ASTContext& ctx, Ides::StringRef name) const;
        DeclarationContext* GetParent() const { return this->parentContext; }
        void SetParent(DeclarationContext* ctx) { parentContext = ctx; }
    private:
        DeclarationContext* parentContext;
    };
    
}
}

#endif /* defined(__ides__DeclarationContext__) */

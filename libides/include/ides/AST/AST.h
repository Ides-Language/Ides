#ifndef _IDES_AST_NODE_H_
#define _IDES_AST_NODE_H_

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/uuid/uuid.hpp>
#include <llvm/Value.h>

#include <ides/common.h>

namespace Ides {
namespace AST {
    class AST;
    
    
    class IDES_EXPORTS SymbolTable : public std::map<Ides::String, boost::shared_ptr<AST> > {
    public:
        
    private:
    }; // class SymbolTable
    
    
    
    class AST {
    public:
        
        AST();
        
        template<typename T>
        T* As() {
            T* ret = dynamic_cast<T>(this);
            return ret;
        }
        
        virtual Ides::String GetDOT() const = 0;
        const boost::uuids::uuid& GetUUID() const { return this->uuid; }
    protected:
        SymbolTable symbols;
    private:
        const boost::uuids::uuid uuid;
    };
    
    class ASTIdentifier : public AST {
    public:
        ASTIdentifier (const Ides::String& name) : name(name) {}
        
        virtual Ides::String GetDOT() const;
        
        const Ides::String name;
    };
    
    class ASTList : public AST, public std::list<AST*> {
    public:
        virtual Ides::String GetDOT() const;
    };
    
    
    class ASTCompilationUnit {
    public:
        ASTCompilationUnit(ASTList* symbols) : symbols(symbols) {}
        
        virtual Ides::String GetDOT() const;
        const ASTList* symbols;
    };
    
} // namespace AST
} // namespace Ides


#endif // _IDES_AST_NODE_H_


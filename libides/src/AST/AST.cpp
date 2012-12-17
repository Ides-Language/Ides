#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <sstream>

namespace Ides {
namespace AST {
    
#define ADD_INTTYPE(size) this->symbols.insert(std::make_pair("int" #size, \
    new Ides::AST::ASTInteger##size##Type))
    
    ASTCompilationUnit::ASTCompilationUnit() {
        ADD_INTTYPE(8);
        ADD_INTTYPE(16);
        ADD_INTTYPE(32);
        ADD_INTTYPE(64);
    }
    
    
    AST* SymbolTable::LookupRecursive(const Ides::String& symbol) const {
        AST* ret = this->Lookup(symbol);
        if (ret == NULL && this->parentScope != NULL) {
            ret = parentScope->LookupRecursive(symbol);
        }
        return ret;
    }
    
    AST* SymbolTable::Lookup(const Ides::String& symbol) const {
        SymbolMap::const_iterator i = this->find(symbol);
        return (i != this->end()) ? i->second : NULL;
    }
    
    AST::AST() : uuid(boost::uuids::random_generator()()) {}
    
    ASTList::~ASTList() {
        while (!this->empty()) {
            delete *this->begin();
            this->pop_front();
        }
    }
    
    ASTDeclaration::~ASTDeclaration() {
        delete name;
        if (this->type) delete this->type;
        if (this->initval) delete this->initval;
    }
    
    ASTFunction::~ASTFunction() {
        delete name;
        if (val) delete val;
        if(body) delete body;
        if (rettype) delete rettype;
        if (args) delete args;
    }
    
    ASTBinaryExpression::~ASTBinaryExpression() {
        delete lhs;
        delete rhs;
    }
    
}
}
#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <sstream>

namespace Ides {
namespace AST {
    

    ASTCompilationUnit::ASTCompilationUnit() {
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
    
}
}
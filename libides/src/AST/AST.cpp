#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <sstream>

namespace Ides {
namespace AST {
        
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
    
    ASTFunctionDecl::~ASTFunctionDecl() {
        delete name;
        delete rettype;
        delete args;
    }
    
    ASTBinaryExpression::~ASTBinaryExpression() {
        delete lhs;
        delete rhs;
    }
    
}
}
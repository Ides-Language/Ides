#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators

#include <sstream>
#include <cstdio>

namespace Ides {
namespace AST {

    Ides::String ASTCompilationUnit::GetCHeader() const {
        std::stringstream buf;
        buf << "#ifndef __GUARD__" << std::endl <<
               "#define __GUARD__" << std::endl <<
               "#include \"stdint.h\"" << std::endl;
        
        for (auto i = this->begin(); i != this->end(); ++i) {
            buf << (*i)->GetCHeader();
        }
        
        buf << "#endif // __GUARD__";
        return buf.str();
    }
    
    Ides::String ASTFunction::GetCHeader() const {
        std::stringstream buf;
        if (this->val == NULL && this->body == NULL) return "";
        
        if (this->rettype != NULL) buf << this->rettype->GetCHeader();
        else buf << "void";
        
        buf << " " << this->name->GetCHeader() << '(';
        
        if (this->args) {
            bool firstarg = true;
            for (auto i = this->args->begin(); i != this->args->end(); ++i) {
                if (!firstarg) {
                    buf << ',';
                }
                firstarg = false;
                buf << (*i)->GetCHeader();
            }
        }
        buf << ')' << ';' << std::endl;
        return buf.str();
    }
    
    Ides::String ASTDeclaration::GetCHeader() const {
        std::stringstream buf;
        buf << this->type->GetCHeader() << " " << this->name->GetCHeader();
        return buf.str();
    }
    
}
}
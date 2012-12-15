#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators

#include <sstream>
#include <cstdio>

#define THIS_UUID UUIDFormat(this->GetUUID())

namespace {
    Ides::String UUIDFormat(const boost::uuids::uuid& uuid) {
        char buf[34];
        buf[0] = 'N';
        int idx = 0;
        for (auto i = uuid.begin(); i != uuid.end(); ++i) {
            assert(sprintf(&buf[idx++ * 2 + 1], "%02X", *i));
        }
        return std::string(buf);
    }
    
    Ides::String Link(Ides::AST::AST* parent, Ides::AST::AST* child, const std::string& label = "") {
        std::stringstream buf;
        buf << UUIDFormat(parent->GetUUID()) << " -> " << UUIDFormat(child->GetUUID()) << " [label=\"" << label << "\"];" << std::endl;
        return buf.str();
    }
}


namespace Ides {
namespace AST {
    
    Ides::String ASTIdentifier::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"" << this->name << "\"];" << std::endl;
        return buf.str();
    }
    
    Ides::String ASTList::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"ASTList\"]" << std::endl;
        for (auto i = this->begin(); i != this->end(); ++i) {
            buf << (*i)->GetDOT();
            buf << Link((AST*)this, (AST*)*i);
        }
        return buf.str();
    }
    
    Ides::String ASTDeclaration::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"" << ((this->vartype == ASTDeclaration::DECL_VAL) ? "val " : "var ") << this->name->name << "\" shape=box]" << std::endl;
        
        if (this->type != NULL) {
            buf << this->type->GetDOT();
            buf << Link((AST*)this, (AST*)this->type, "type");
        }
        
        if (this->initval != NULL) {
            buf << this->initval->GetDOT();
            buf << Link((AST*)this, (AST*)this->initval, "val");
        }
                                                                          
        return buf.str();
    }
    
    Ides::String ASTFunction::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"def " << this->name->name << " ...\"];" << std::endl;
        
        if (this->args != NULL) {
            buf << this->args->GetDOT();
            buf << UUIDFormat(this->GetUUID()) << " -> " << UUIDFormat(this->args->GetUUID()) << " [label=\"args\"];" << std::endl;
        }
        
        if (this->val != NULL) {
            buf << this->val->GetDOT();
            buf << UUIDFormat(this->GetUUID()) << " -> " << UUIDFormat(this->val->GetUUID()) << " [label=\"expr\"];" << std::endl;
        }
        
        if (this->rettype != NULL) {
            buf << this->rettype->GetDOT();
            buf << UUIDFormat(this->GetUUID()) << " -> " << UUIDFormat(this->rettype->GetUUID()) << " [label=\"ret\"];" << std::endl;
        }
        
        return buf.str();
    }
    
    Ides::String ASTConstantIntExpression::GetDOT() const
    {
        std::stringstream buf;
        buf << UUIDFormat(this->GetUUID()) << " [shape=box label=\"" << this->val << "\"];" << std::endl;
        return buf.str();
    }
    
    
    Ides::String ASTCompilationUnit::GetDOT() const
    {
        std::stringstream buf;
        buf << "digraph compilation_unit {" << std::endl;
        buf << "node [shape=record];" << std::endl;
        buf << "graph [ranksep=0];" << std::endl;
        buf << "compilation_unit;" << std::endl;
        
        buf << this->symbols->GetDOT();
        buf << "compilation_unit -> " << UUIDFormat(this->symbols->GetUUID()) << ";" << std::endl;
        
        buf << "}" << std::endl;

        return buf.str();
    }
    
#define BINARY_GETDOT(n, s) \
    Ides::String AST##n##Expression::GetDOT() const { \
        std::stringstream buf; \
        buf << THIS_UUID << " [label=\"" s "\" shape=plaintext];" << std::endl; \
        buf << lhs->GetDOT(); \
        buf << rhs->GetDOT(); \
        buf << Link((AST*)this, (AST*)lhs, "lhs"); \
        buf << Link((AST*)this, (AST*)rhs, "rhs"); \
        return buf.str(); \
    }
    
#define UNARY_GETDOT(n, s) \
    Ides::String AST##n##Expression::GetDOT() const { \
        std::stringstream buf; \
        buf << THIS_UUID << " [label=\"" s "\" shape=plaintext];" << std::endl; \
        buf << arg->GetDOT(); \
        buf << Link((AST*)this, (AST*)arg); \
        return buf.str(); \
    }
    
    UNARY_GETDOT(Deref, "*");
    UNARY_GETDOT(Not, "!");
    UNARY_GETDOT(Bnot, "~");
    UNARY_GETDOT(Neg, "-");
    UNARY_GETDOT(PreInc, "pre ++");
    UNARY_GETDOT(PreDec, "pre --");
    
    BINARY_GETDOT(Eq, "=")
    BINARY_GETDOT(Add, "+")
    BINARY_GETDOT(Sub, "-")
    BINARY_GETDOT(Mul, "*")
    BINARY_GETDOT(Div, "/")
    BINARY_GETDOT(Mod, "%")
    BINARY_GETDOT(Coalesce, "or")
    BINARY_GETDOT(Band, "&")
    BINARY_GETDOT(Bor, "|")
    BINARY_GETDOT(Bxor, "^")
    
}
}
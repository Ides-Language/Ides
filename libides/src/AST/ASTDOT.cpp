#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators

#include <sstream>
#include <cstdio>

#define THIS_UUID UUIDFormat(this->GetUUID())
#define OUT(x) do { std::stringstream buf; buf << x; return buf.str(); } while(0)

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
    
    Ides::String EscapeString(const Ides::String& str) {
        std::stringstream buf;
        for (auto i = str.begin(); i != str.end(); ++i) {
            switch (*i) {
                case '\a': buf << "\\\\a"; break;
                case '\b': buf << "\\\\b"; break;
                case '\f': buf << "\\\\f"; break;
                case '\n': buf << "\\\\n"; break;
                case '\r': buf << "\\\\r"; break;
                case '\t': buf << "\\\\t"; break;
                case '\v': buf << "\\\\v"; break;
                case '\'': buf << "\\\\'"; break;
                case '\"': buf << "\\\\\\\""; break;
                default:
                    buf << *i;
            }
        }
        return buf.str();
    }
    
    Ides::String Link(Ides::AST::AST* parent, Ides::AST::AST* child, const std::string& label = "") { OUT(UUIDFormat(parent->GetUUID()) << " -> " << UUIDFormat(child->GetUUID()) << " [label=\"" << label << "\"];" << std::endl); }
}


namespace Ides {
namespace AST {
        
        
    Ides::String ASTCompilationUnit::GetDOT() const
    {
        std::stringstream buf;
        
        
        buf << "digraph compilation_unit {" << std::endl;
        buf << "node [shape=plaintext];" << std::endl;
        //buf << "graph [ranksep=0];" << std::endl;
        
        buf << THIS_UUID << " [label=\"compilation_unit\" shape=tripleoctagon rank=2];" << std::endl;
        for (auto i = this->begin(); i != this->end(); ++i) {
            buf << (*i)->GetDOT();
            buf << Link((AST*)this, (AST*)*i);
        }
        
        buf << "{ key [label=\"key:\" shape=plaintext]; literal [shape=ellipse]; identifier [shape=diamond]; declaration [shape=box]; type [shape=doubleoctagon]; }" << std::endl;
        
        buf << "}" << std::endl;
        
        return buf.str();
    }
    
    Ides::String ASTList::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"ASTList\" shape=plaintext];" << std::endl;
        for (auto i = this->begin(); i != this->end(); ++i) {
            buf << (*i)->GetDOT();
            buf << Link((AST*)this, (AST*)*i);
        }
        return buf.str();
    }
    
    Ides::String ASTPtrType::GetDOT() const { OUT(THIS_UUID << " [label=\"ptr*\" shape=doubleoctagon];" << std::endl << this->basetype->GetDOT() << Link((AST*)this, (AST*)basetype) << std::endl); }
    Ides::String ASTTypeName::GetDOT() const { OUT(THIS_UUID << " [label=\"" << this->name->name << "\" shape=doubleoctagon];" << std::endl); }
    
    Ides::String ASTDeclaration::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"{" << ((this->vartype == ASTDeclaration::DECL_VAL) ? "val " : "var ") << "|" << this->name->name << "}\" shape=record];" << std::endl;
        
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
    
    
    Ides::String ASTFunctionDecl::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"def " << this->name->name << " ...\" shape=record];" << std::endl;
        
        if (this->args != NULL) {
            buf << this->args->GetDOT();
            buf << UUIDFormat(this->GetUUID()) << " -> " << UUIDFormat(this->args->GetUUID()) << " [label=\"args\"];" << std::endl;
        }
        
        if (this->rettype != NULL) {
            buf << this->rettype->GetDOT();
            buf << UUIDFormat(this->GetUUID()) << " -> " << UUIDFormat(this->rettype->GetUUID()) << " [label=\"ret\"];" << std::endl;
        }
        
        return buf.str();
    }
    
    Ides::String ASTFunction::GetDOT() const
    {
        std::stringstream buf;
        buf << ASTFunctionDecl::GetDOT();
        
        if (this->val != NULL) {
            buf << this->val->GetDOT() << Link((AST*)this, (AST*)val, "expr") << std::endl;
        }
        
        if (this->body != NULL) {
            buf << this->body->GetDOT() << Link((AST*)this, (AST*)this->body, "body");
        }
        
        return buf.str();
    }
    
    Ides::String ASTFunctionCall::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"operator()\"];" << std::endl;
        buf << this->fn->GetDOT() << Link((AST*)this, (AST*)fn, "lhs");
        buf << this->args->GetDOT() << Link((AST*)this, (AST*)args, "(args)");
        return buf.str();
    }
    
    Ides::String ASTBracketCall::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"operator[]\"];" << std::endl;
        buf << this->fn->GetDOT() << Link((AST*)this, (AST*)fn, "lhs");
        buf << this->args->GetDOT() << Link((AST*)this, (AST*)args, "[args]");
        return buf.str();
    }
    
    Ides::String ASTCompoundStatement::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"\\{ block \\}\" shape=plaintext];" << std::endl;
        for (auto i = this->begin(); i != this->end(); ++i) {
            buf << (*i)->GetDOT();
            buf << Link((AST*)this, (AST*)*i);
        }
        return buf.str();
    }
    
    
    Ides::String ASTIdentifier::GetDOT() const { OUT(THIS_UUID << " [label=\"" << this->name << "\" shape=diamond];" << std::endl); }
    
    /*********************************************************
     **************** Operator Expressions *******************
     *********************************************************/
    
    Ides::String ASTBinaryExpression::GetDOT() const { OUT(lhs->GetDOT() << rhs->GetDOT() << Link((AST*)this, (AST*)lhs, "lhs") << Link((AST*)this, (AST*)rhs, "rhs")); }
    Ides::String ASTInfixExpression::GetDOT() const { OUT(THIS_UUID << "[label=\"" << this->func->name << "\" shape=diamond];" << std::endl << ASTBinaryExpression::GetDOT()); }
    
#define BINARY_GETDOT(n, s) Ides::String AST##n##Expression::GetDOT() const { OUT(THIS_UUID << " [label=\"" s "\" shape=plaintext];" << std::endl << ASTBinaryExpression::GetDOT()); }
#define UNARY_GETDOT(n, s) Ides::String AST##n##Expression::GetDOT() const { OUT(THIS_UUID << " [label=\"unary " s "\" shape=plaintext];" << std::endl << arg->GetDOT() << Link((AST*)this, (AST*)arg)); }
    
    /*********************************************************
     ******************* Unary Expressions *******************
     *********************************************************/
    UNARY_GETDOT(Deref, "*");
    UNARY_GETDOT(Not, "!");
    UNARY_GETDOT(Bnot, "~");
    UNARY_GETDOT(Neg, "-");
    UNARY_GETDOT(PreInc, "pre ++");
    UNARY_GETDOT(PreDec, "pre --");
    UNARY_GETDOT(PostInc, "post ++");
    UNARY_GETDOT(PostDec, "post --");
    
    /*********************************************************
     ****************** Binary Expressions *******************
     *********************************************************/
    BINARY_GETDOT(Eq, "=")
    BINARY_GETDOT(Add, "+")
    BINARY_GETDOT(Sub, "-")
    BINARY_GETDOT(Mul, "*")
    BINARY_GETDOT(Div, "/")
    BINARY_GETDOT(Mod, "%")
    BINARY_GETDOT(Band, "&")
    BINARY_GETDOT(Bor, "|")
    BINARY_GETDOT(Bxor, "^")
    
    BINARY_GETDOT(Coalesce, "or")
    BINARY_GETDOT(Cast, "as")
    
    BINARY_GETDOT(DictPair, "pair");
    
    /*********************************************************
     **************** Constant Expressions *******************
     *********************************************************/
    
    Ides::String ASTDictExpression::GetDOT() const
    {
        std::stringstream buf;
        buf << THIS_UUID << " [label=\"Dictionary\" shape=ellipse];" << std::endl;
        for (auto i = this->begin(); i != this->end(); ++i) {
            buf << (*i)->GetDOT();
            buf << Link((AST*)this, (AST*)*i);
        }
        return buf.str();
    }
    
    Ides::String ASTConstantIntExpression::GetDOT() const { OUT(UUIDFormat(this->GetUUID()) << " [shape=ellipse label=\"" << this->val << "\"];" << std::endl); }
    Ides::String ASTConstantCharExpression::GetDOT() const { OUT(UUIDFormat(this->GetUUID()) << " [shape=ellipse label=\"'" << this->val << "'\"];" << std::endl); }
    Ides::String ASTConstantStringExpression::GetDOT() const { OUT(UUIDFormat(this->GetUUID()) << " [shape=ellipse label=\"\\\"" << EscapeString(this->val) << "\\\"\"];" << std::endl); }
    Ides::String ASTConstantCStringExpression::GetDOT() const { OUT(UUIDFormat(this->GetUUID()) << " [shape=ellipse label=\"C\\\"" << EscapeString(this->val) << "\\\"\"];" << std::endl); }
    Ides::String ASTConstantWCStringExpression::GetDOT() const { OUT(UUIDFormat(this->GetUUID()) << " [shape=ellipse label=\"WC\\\"" << EscapeString(this->val) << "\\\"\"];" << std::endl); }
    Ides::String ASTConstantLCStringExpression::GetDOT() const { OUT(UUIDFormat(this->GetUUID()) << " [shape=ellipse label=\"LC\\\"" << EscapeString(this->val) << "\\\"\"];" << std::endl); }
    
    Ides::String ASTNullExpr::GetDOT() const { OUT(THIS_UUID << " [label=\"null\" shape=ellipse];" << std::endl); }
    
}
}
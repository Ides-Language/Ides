//
//  AST.cpp
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#include <stdio.h>
#include <ides/Parsing/AST.h>

namespace {

    std::string NL(size_t size) {
        std::stringstream buf;
        buf << std::endl;
        for (size_t i = 0; i < size; ++i)
            buf << " ";
        return buf.str();
    }

    void PrettyPrint(std::ostream& os, const Ides::Ast& data, size_t tab) {
        Match<void>()
            .on<const Ides::Ast>([&os, &tab](const Ides::Ast*){ os << "{AST?}"; })
            .on<const Ides::TupleExpr>([&os, &tab](const Ides::TupleExpr* expr) {
                for (auto& i : expr->items) {
                    PrettyPrint(os, *i, tab);
                    os << ";" << NL(tab);
                }
            })
            .on<const Ides::InfixExpr>([&os, &tab](const Ides::InfixExpr* expr){
                os << "(";
                PrettyPrint(os, *expr->lhs, tab);
                os << " " << expr->ident << " ";
                PrettyPrint(os, *expr->rhs, tab);
                os << ")";
            })
            .on<const Ides::CallExpr>([&os, &tab](const Ides::CallExpr* expr){
                PrettyPrint(os, *expr->lhs, tab);
                os << "(";
                bool first = true;
                for (auto& i : expr->args->items) {
                    if (!first) os << ", ";
                    PrettyPrint(os, *i, tab);
                    first = false;
                }
                os << ")";
            })
            .on<const Ides::IndexExpr>([&os, &tab](const Ides::IndexExpr* expr){
                PrettyPrint(os, *expr->lhs, tab);
                os << "[";
                bool first = true;
                for (auto& i : expr->args->items) {
                    if (!first) os << ", ";
                    PrettyPrint(os, *i, tab);
                    first = false;
                }
                os << "]";
            })
            .on<const Ides::DotExpr>([&os, &tab](const Ides::DotExpr* expr){
                PrettyPrint(os, *expr->lhs, tab);
                os << "." << expr->ident;
            })
            .on<const Ides::IdentifierExpr>([&os, &tab](const Ides::IdentifierExpr* expr){
                os << "`" << expr->ident << "`";
            })
            .on<const Ides::ConstantInt>([&os, &tab](const Ides::ConstantInt* expr){ os << expr->value; })
            .on<const Ides::ConstantDec>([&os, &tab](const Ides::ConstantDec* expr){ os << expr->value; })
            .on<const Ides::ConstantBool>([&os, &tab](const Ides::ConstantBool* expr){ os << (expr->value ? "true" : "false"); })

            .on<const Ides::DataStructureDecl>([&os, &tab](const Ides::DataStructureDecl* expr){
                os << " : ";
                bool first = true;
                for (auto& i : expr->superTypes->items) {
                    if (!first) os << ", ";
                    PrettyPrint(os, *i, tab);
                    first = false;
                }
                os << "{" << NL(tab+4);
                PrettyPrint(os, *expr->body, tab+4);
                os << NL(tab) << "}";
            })
            .on<const Ides::TraitDecl>([&os, &tab](const Ides::TraitDecl* expr){
                os << "trait " << expr->name;
                PrettyPrint(os, *expr->decl, tab);
            })

            .match(&data);
    }

    enum associativity {
        LEFT,
        RIGHT,
        NONASSOC
    };

    typedef struct precedence {
        const char* op;
        const int p;
        const associativity assoc;
    } precedence;

    const precedence operators[] = {
        {"&&", 1, LEFT},{"||", 1, LEFT},
        {"|", 2, LEFT},{"^", 5, RIGHT},{"&", 6, LEFT},
        {"or", 7, LEFT},
        {"==", 8, LEFT}, {"!=", 8, LEFT},
        {">", 9, LEFT},{"<", 9, LEFT},{">=", 9, LEFT},{"<=", 9, LEFT},
        {"+", 10, LEFT},{"-", 10, LEFT},
        {"*", 11, LEFT},{"/", 11, LEFT},{"%", 11, LEFT},
        {"as", 50, LEFT},
    };

#define PRECTABLE_LEN  sizeof(operators)/sizeof(operators[0])

    const precedence get_precedence(const char* op){
        // Operators ending in = are minimum-associativity.
        if (op[strlen(op)] - 1 == '=') {
            return {op, -10000, RIGHT};
        }

        for (int i=0; i<PRECTABLE_LEN; i++) {
            if (strcmp(operators[i].op, op) == 0) {
                return operators[i];
            }
        }
        return {op, 0, LEFT};
    }

    Ides::InfixExpr* RotateRhs(Ides::InfixExpr* self) {
        if (Ides::InfixExpr* irhs = self->rhs->As<Ides::InfixExpr>()) {
            precedence selfP = get_precedence(self->ident.c_str());
            precedence rhsP = get_precedence(irhs->ident.c_str());
            if ((selfP.p > rhsP.p) || (selfP.p == rhsP.p && selfP.assoc == LEFT)) {
                Ides::Expr* b = irhs->lhs.release();
                self->rhs.release();
                self->rhs.reset(b);
                irhs->lhs.reset(RotateRhs(self));
                return irhs;
            } else if (selfP.assoc == NONASSOC) {
                std::cerr << "Nonassoc!" << std::endl;
            }
        }
        return self;
    }

}

Ides::InfixExpr* Ides::InfixExpr::Create(IdentifierExpr* ident, Expr* lhs, Expr* rhs) {
    return RotateRhs(new Ides::InfixExpr(ident, lhs, rhs));
}

std::ostream& operator<<(std::ostream& os, const Ides::Ast& data) {
    PrettyPrint(os, data, 0);
    return os;
}

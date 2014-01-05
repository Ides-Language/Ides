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

    std::ostream& PrettyPrint(std::ostream& os, const Ides::Ast& data, size_t tab) {
        Match<void>()
            .on<const Ides::Ast>([&os, &tab](const Ides::Ast*){ os << "{AST?}"; })
            .on<const Ides::TupleExpr>([&os, &tab](const Ides::TupleExpr* expr) {
                os << "{";
                for (auto& i : expr->items) {
                    os << NL(tab+4);
                    PrettyPrint(os, *i, tab+4);
                    os << ";";
                }
                os << NL(tab) << "}";
            })
            .on<const Ides::InfixExpr>([&os, &tab](const Ides::InfixExpr* expr){
                os << "(";
                PrettyPrint(os, *expr->lhs, tab);
                os << " " << expr->ident << " ";
                PrettyPrint(os, *expr->rhs, tab);
                os << ")";
            })
            .on<const Ides::PrefixExpr>([&os, &tab](const Ides::PrefixExpr* expr){
                os << "(";
                os << expr->ident;
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
            .on<const Ides::VarArgsExpr>([&os, &tab](const Ides::VarArgsExpr* expr){
                PrettyPrint(os, *expr->lhs, tab);
                os << "...";
            })
            .on<const Ides::BlockExpr>([&os, &tab](const Ides::BlockExpr* expr){
                PrettyPrint(os, *expr->lhs, tab);
                PrettyPrint(os, *expr->body, tab);
            })
            .on<const Ides::DotExpr>([&os, &tab](const Ides::DotExpr* expr){
                PrettyPrint(os, *expr->lhs, tab);
                os << "." << expr->ident;
            })
            .on<const Ides::IfExpr>([&os, &tab](const Ides::IfExpr* expr){
                os << "if (";
                PrettyPrint(os, *expr->cond, tab);
                os << ") ";
                PrettyPrint(os, *expr->body, tab);
            })
            .on<const Ides::IdentifierExpr>([&os, &tab](const Ides::IdentifierExpr* expr){
                os << "`" << expr->ident << "`";
            })
            .on<const Ides::Name>([&os, &tab](const Ides::Name* expr){
                os << "`" << expr->ident << "`";
                if (!expr->genericArgs->items.empty()) {
                    os << "[";
                    bool first = true;
                    for (auto& i : expr->genericArgs->items) {
                        os << (first ? "" : ", ");
                        PrettyPrint(os, *i, tab);
                        first = false;
                    }
                    os << "]";
                }
            })
            .on<const Ides::ConstantInt>([&os, &tab](const Ides::ConstantInt* expr){ os << expr->value; })
            .on<const Ides::ConstantDec>([&os, &tab](const Ides::ConstantDec* expr){ os << expr->value; })
            .on<const Ides::ConstantBool>([&os, &tab](const Ides::ConstantBool* expr){ os << (expr->value ? "true" : "false"); })
            .on<const Ides::ConstantString>([&os, &tab](const Ides::ConstantString* expr){ os << "\"" << expr->value << "\""; })
            .on<const Ides::PlaceholderExpr>([&os, &tab](const Ides::PlaceholderExpr* expr){
                os << ':' << static_cast<int>(expr->value);
            })

            .on<const Ides::DataStructureDecl>([&os, &tab](const Ides::DataStructureDecl* expr){
                if (!expr->args->items.empty()) {
                    os << "(";
                    bool first = true;
                    for (auto& i : expr->args->items) {
                        os << (first ? "" : ", ");
                        PrettyPrint(os, *i, tab);
                        first = false;
                    }
                    os << ")";
                }
                bool first = true;
                for (auto& i : expr->superTypes->items) {
                    os << (first ? " : " : ", ");
                    PrettyPrint(os, *i, tab);
                    first = false;
                }
                os << " => ";
                PrettyPrint(os, *expr->body, tab);
            })
            .on<const Ides::TraitDecl>([&os, &tab](const Ides::TraitDecl* expr){
                os << "trait ";
                PrettyPrint(os, *expr->name, tab);
                PrettyPrint(os, *expr->decl, tab);
            })
            .on<const Ides::ClassDecl>([&os, &tab](const Ides::ClassDecl* expr){
                os << "class ";
                PrettyPrint(os, *expr->name, tab);
                PrettyPrint(os, *expr->decl, tab);
            })
            .on<const Ides::StructDecl>([&os, &tab](const Ides::StructDecl* expr){
                os << "struct ";
                PrettyPrint(os, *expr->name, tab);
                PrettyPrint(os, *expr->decl, tab);
            })
            .on<const Ides::ModuleDecl>([&os, &tab](const Ides::ModuleDecl* expr){
                os << "mod ";
                PrettyPrint(os, *expr->name, tab);
                PrettyPrint(os, *expr->decl, tab);
            })
            .on<const Ides::ArgDecl>([&os, &tab](const Ides::ArgDecl* expr){
                PrettyPrint(os, *expr->name, tab);
                os << " : ";
                PrettyPrint(os, *expr->decl, tab);
            })
            .on<const Ides::FnDecl>([&os, &tab](const Ides::FnDecl* expr){
                os << "def ";
                PrettyPrint(os, *expr->name, tab);
                os << "(";
                bool first = true;
                for (auto& i : expr->decl->args->items) {
                    os << (first ? "" : ", ");
                    PrettyPrint(os, *i, tab);
                    first = false;
                }
                os << ")";
                if (expr->decl->type) {
                    os << " : ";
                    PrettyPrint(os, *expr->decl->type, tab);
                }
                if (expr->decl->body) {
                    os << " => ";
                    PrettyPrint(os, *expr->decl->body, tab);
                }
            })
            .on<const Ides::PartialFunction>([&os, &tab](const Ides::PartialFunction* expr){
                os << "{";
                for (auto& i : expr->cases) {
                    os << NL(tab+4);
                    os << "case ";
                    PrettyPrint(os, *i->lhs, tab+8);
                    os << " => ";
                    os << NL(tab+8);
                    PrettyPrint(os, *i->rhs, tab+8);
                }
                os << NL(tab) << "}";
            })

            .match(&data);
        return os;
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

    Ides::InfixExpr* RotateLhs(Ides::InfixExpr* self) {
        if (Ides::InfixExpr* ilhs = self->lhs->As<Ides::InfixExpr>()) {
            precedence selfP = get_precedence(self->ident.c_str());
            precedence lhsP = get_precedence(ilhs->ident.c_str());
            if ((selfP.p > lhsP.p) || (selfP.p == lhsP.p && selfP.assoc == RIGHT)) {
                Ides::Expr* b = ilhs->rhs.release();
                self->lhs.release();
                self->lhs.reset(b);
                ilhs->rhs.reset(RotateLhs(self));
                return ilhs;
            } else if (selfP.assoc == NONASSOC) {
                MSG(E_NONASSOC) % self->ident;
            }
        }
        return self;
    }

}

namespace Ides {

    Ides::InfixExpr* Ides::InfixExpr::Create(IdentifierExpr* ident, Expr* lhs, Expr* rhs) {
        return RotateLhs(new Ides::InfixExpr(ident, lhs, rhs));
    }

    void Ast::Emit(YAML::Emitter &o) {
        o << YAML::BeginMap;
        o << YAML::Key << "type" << YAML::Value << type;
        o << YAML::Key << "loc" << YAML::Value << "null";
        o << YAML::Key << "data";
        o << YAML::Value;
        DoEmit(o);
        o << YAML::EndMap;
    }

    void IdentifierExpr::DoEmit(YAML::Emitter& o) {
        o << ident;
    }

    void Name::DoEmit(YAML::Emitter& o) {
        o << ident;
        o << YAML::Key << "typeargs" << YAML::Value;
        o << YAML::BeginSeq;
        for (auto& i : genericArgs->items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
    }

    void DotExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "ident" << YAML::Value << ident;
        o << YAML::EndMap;
    }

    void TupleExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginSeq;
        for (auto& i : items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
    }

    void CallExpr::DoEmit(YAML::Emitter& o)  {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "args" << YAML::Value;
        o << YAML::BeginSeq;
        for (auto& i : args->items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
        o << YAML::EndMap;
    }

    void IndexExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "args" << YAML::Value;
        o << YAML::BeginSeq;
        for (auto& i : args->items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
        o << YAML::EndMap;
    }

    void InfixExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "op" << YAML::Value << ident;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "rhs" << YAML::Value;
        rhs->Emit(o);
        o << YAML::EndMap;
    }

    void PrefixExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "op" << YAML::Value << ident;
        o << YAML::Key << "rhs" << YAML::Value;
        rhs->Emit(o);
        o << YAML::EndMap;
    }

    void IfExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "cond" << YAML::Value;
        cond->Emit(o);
        o << YAML::Key << "body" << YAML::Value;
        body->Emit(o);
        o << YAML::EndMap;
    }

    void BlockExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "body" << YAML::Value;
        body->Emit(o);
        o << YAML::EndMap;
    }

//    void NamedDecl::DoEmit(YAML::Emitter& o)

    void DataStructureDecl::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "args" << YAML::Value;
        o << YAML::BeginSeq;
        for (auto& i : args->items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
        o << YAML::Key << "supers" << YAML::Value;
        o << YAML::BeginSeq;
        for (auto& i : superTypes->items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
        o << YAML::Key << "body" << YAML::Value;
        body->Emit(o);
        o << YAML::EndMap;
    }

    void FunctionDecl::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "args" << YAML::Value;
        o << YAML::BeginSeq;
        for (auto& i : args->items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
        if (type) {
            o << YAML::Key << "ret" << YAML::Value;
            type->Emit(o);
        }
        if (body) {
            o << YAML::Key << "body" << YAML::Value;
            body->Emit(o);
        }
        o << YAML::EndMap;
    }

//    void ValueDecl::DoEmit(YAML::Emitter& o)

    void CasePair::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "rhs" << YAML::Value;
        rhs->Emit(o);
    }

    void PartialFunction::DoEmit(YAML::Emitter& o) {
    }


}

std::ostream& operator<<(std::ostream& os, const Ides::Ast& data) {
    return PrettyPrint(os, data, 0);
}

//
//  AST.cpp
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#include <ides/Parsing/AstTraits.h>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <regex>
#include <ides/Parsing/AST.h>

namespace Ides {
    struct AstBase;

    typedef Ides::AstBase* (YamlReader)(const YAML::Node&);

    std::unordered_map<std::string, YamlReader*> AstReaders;


    template<typename T>
    struct AstIOBuilder {
        static AstIOBuilder<T>* self;
        AstIOBuilder(YamlReader* reader) {
            AstReaders.insert(std::make_pair(AstTraits<T>::name, reader));
        }
    };

    template<typename T>
    Ides::AstBase* ReadNode(const YAML::Node& node);

    template<typename T>
    void WriteNode(YAML::Emitter& o, const T& ast);

#define AST_NAME(r, d, t) \
    struct t;\
    template<> AstBase* ReadNode<t>(const YAML::Node& n); \
    template<> AstIOBuilder<t>* AstIOBuilder<t>::self = new AstIOBuilder(&ReadNode<t>);


    BOOST_PP_SEQ_FOR_EACH(AST_NAME, , AST_TYPES);


}


namespace {

    std::string NL(size_t size) {
        std::stringstream buf;
        buf << std::endl;
        for (size_t i = 0; i < size; ++i)
            buf << " ";
        return buf.str();
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
        {"if", 0, NONASSOC}, {"else", -200, RIGHT},
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
            return {op, -100, RIGHT};
        }

        for (int i=0; i<PRECTABLE_LEN; i++) {
            if (strcmp(operators[i].op, op) == 0) {
                return operators[i];
            }
        }
        return {op, 0, LEFT};
    }

    Ides::BinaryExpr* RotateLhs(Ides::BinaryExpr* self) {
        Ides::IdentifierExpr* ident = self->As<Ides::IdentifierExpr>();
        Ides::BinaryExpr* ilhs = self->lhs->As<Ides::BinaryExpr>();
        if (ident != NULL && ilhs != NULL) {
            if (Ides::IdentifierExpr* lhsident = ilhs->fn->As<Ides::IdentifierExpr>()) {
                precedence selfP = get_precedence(ident->ident.c_str());
                precedence lhsP = get_precedence(lhsident->ident.c_str());
                if ((selfP.p > lhsP.p) || (selfP.p == lhsP.p && selfP.assoc == RIGHT)) {
                    Ides::Expr* b = ilhs->rhs.release();
                    self->lhs.release();
                    self->lhs.reset(b);
                    ilhs->rhs.reset(RotateLhs(self));
                    return ilhs;
                } else if (selfP.assoc == NONASSOC) {
                    MSG(E_NONASSOC) % ident->ident;
                }
            }
        }
        return self;
    }

}

namespace Ides {

    DECL_AST_VISITOR(PrintNode, void, std::ostream&, size_t);

    Ides::BinaryExpr* Ides::BinaryExpr::Create(IdentifierExpr* ident, Expr* lhs, Expr* rhs) {
        return RotateLhs(new Ides::BinaryExpr(ident, lhs, rhs));
    }

    void Ast::Emit(YAML::Emitter &o) {
        if (this == NULL) {
            o << YAML::Node();
            return;
        }
        o << YAML::BeginMap;
        o << YAML::Key << "type" << YAML::Value << getName();
        o << YAML::Key << "loc" << YAML::Value;
        o << YAML::BeginMap;
        if (this->source.begin.file)
            o << YAML::Key << "file" << YAML::Value << this->source.begin.file->GetPath().string();
        o << YAML::Key << "offset" << YAML::Value << this->source.begin.offset;
        o << YAML::Key << "length" << YAML::Value << this->source.length;
        o << YAML::EndMap;
        o << YAML::Key << "data" << YAML::Value;
        DoEmit(o);
        o << YAML::EndMap;
    }

    Ast* Ast::Read(const YAML::Node& n) {
        auto nt = n.Type();
        if (nt == YAML::NodeType::Null || nt == YAML::NodeType::Undefined) return NULL;
        std::string ntype = n["type"].Scalar();
        DBG("Using reader for " << ntype);
        YamlReader* reader = AstReaders.at(ntype);
        return (*reader)(n["data"]);
    }

    /** IdentifierExpr **/
    void IdentifierExpr::DoEmit(YAML::Emitter& o) {
        o << ident;
    }

    template<>
    Ast* ReadNode<IdentifierExpr>(const YAML::Node& n) {
        return new IdentifierExpr(n.Scalar());
    }


    template<>
    void PrintNode(const IdentifierExpr& expr, std::ostream& os, size_t tab) {
        static std::regex basic_ident("[A-Za-z_][A-Za-z0-9_]*");
        if (std::regex_match(expr.ident, basic_ident)) {
            os << expr.ident;
        }
        else {
            os << "`" << expr.ident << "`";
        }
    }

    /** Name **/
    void Name::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "ident" << YAML::Value;
        ident->Emit(o);
        if (!genericArgs->items.empty()) {
            o << YAML::Key << "typeargs" << YAML::Value;
            genericArgs->Emit(o);
        }
        o << YAML::EndMap;
    }

    template<>
    Ast* ReadNode<Name>(const YAML::Node& n) {
        return new Name(new IdentifierExpr(n["ident"].Scalar()), Ast::Read<ExprList>(n["typeargs"]));
    }

    template<>
    void PrintNode(const Name& expr, std::ostream& os, size_t tab) {
        DoPrintNode(*expr.ident, os, tab);
        if (!expr.genericArgs->items.empty()) {
            os << "[";
            bool first = true;
            for (auto& i : expr.genericArgs->items) {
                os << (first ? "" : ", ");
                DoPrintNode(*i, os, tab);
                first = false;
            }
            os << "]";
        }
    }

    /** ExprList **/
    void ExprList::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginSeq;
        for (auto& i : items) {
            i->Emit(o);
        }
        o << YAML::EndSeq;
    }

    template<>
    Ast* ReadNode<ExprList>(const YAML::Node& n) {
        ExprList* ret = new ExprList();
        for (auto i : n) {
            ret->Add(Ast::Read<Expr>(i));
        }
        return ret;
    }

    template<>
    void PrintNode(const ExprList& expr, std::ostream& os, size_t tab) {
        os << "{";
        for (auto& i : expr.items) {
            os << NL(tab+4);
            DoPrintNode(*i, os, tab+4);
            os << ";";
        }
        os << NL(tab) << "}";
    }

    /** CallExpr **/
    void CallExpr::DoEmit(YAML::Emitter& o)  {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "args" << YAML::Value;
        args->Emit(o);
        o << YAML::EndMap;
    }

    template<>
    Ast* ReadNode<CallExpr>(const YAML::Node& n) {
        return new CallExpr(Ast::Read<Expr>(n["lhs"]), Ast::Read<ExprList>(n["args"]));
    }

    template<>
    void PrintNode(const CallExpr& expr, std::ostream& os, size_t tab) {
        DoPrintNode(*expr.lhs, os, tab);
        os << "(";
        bool first = true;
        for (auto& i : expr.args->items) {
            if (!first) os << ", ";
            DoPrintNode(*i, os, tab);
            first = false;
        }
        os << ")";
    }

    /** IndexExpr **/
    void IndexExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "args" << YAML::Value;
        args->Emit(o);
        o << YAML::EndMap;
    }

    template<>
    Ast* ReadNode<IndexExpr>(const YAML::Node& n) {
        return new IndexExpr(Ast::Read<Expr>(n["lhs"]), Ast::Read<ExprList>(n["args"]));
    }

    template<>
    void PrintNode(const IndexExpr& expr, std::ostream& os, size_t tab) {
        DoPrintNode(*expr.lhs, os, tab);
        os << "[";
        bool first = true;
        for (auto& i : expr.args->items) {
            if (!first) os << ", ";
            DoPrintNode(*i, os, tab);
            first = false;
        }
        os << "]";
    }

    /** BinaryExpr **/
    void BinaryExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "fn" << YAML::Value;
        fn->Emit(o);
        o << YAML::Key << "lhs" << YAML::Value;
        lhs->Emit(o);
        o << YAML::Key << "rhs" << YAML::Value;
        rhs->Emit(o);
        o << YAML::EndMap;
    }

    template<>
    Ast* ReadNode<BinaryExpr>(const YAML::Node& n) {
        return new BinaryExpr(new IdentifierExpr(n["op"].Scalar()),
                              Ast::Read<Expr>(n["lhs"]),
                              Ast::Read<Expr>(n["rhs"]));
    }

    template<>
    void PrintNode(const BinaryExpr& expr, std::ostream& os, size_t tab) {
        if (expr.fn->ident == "if") {
            os << "if (";
            DoPrintNode(*expr.lhs, os, tab);
            os << ") ";
            DoPrintNode(*expr.rhs, os, tab);
            return;
        }
        os << "(";
        DoPrintNode(*expr.lhs, os, tab);
        os << ")";
        os << " ";
        DoPrintNode(*expr.fn, os, tab);
        os << " ";
        DoPrintNode(*expr.rhs, os, tab);
    }

    /** UnaryExpr **/
    void UnaryExpr::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        if (is_prefix) {
            o << YAML::Key << "op" << YAML::Value; fn->Emit(o);
            o << YAML::Key << "arg" << YAML::Value; arg->Emit(o);
        } else {
            o << YAML::Key << "arg" << YAML::Value; arg->Emit(o);
            o << YAML::Key << "op" << YAML::Value; fn->Emit(o);
        }
        o << YAML::EndMap;
    }

    template<>
    Ast* ReadNode<UnaryExpr>(const YAML::Node& n) {
        return new UnaryExpr(Ast::Read<Expr>(n["arg"]),
                             new IdentifierExpr(n["op"].Scalar()));
    }

    template<>
    void PrintNode(const UnaryExpr& expr, std::ostream& os, size_t tab) {
        DoPrintNode(*expr.fn, os, tab);
        DoPrintNode(*expr.arg, os, tab);
    }


    /** DataStructureDecl **/
    void DataStructureDecl::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "args" << YAML::Value;
        args->Emit(o);
        o << YAML::Key << "supers" << YAML::Value;
        superTypes->Emit(o);
        o << YAML::Key << "body" << YAML::Value;
        body->Emit(o);
        o << YAML::EndMap;
    }

    template<>
    Ast* ReadNode<DataStructureDecl>(const YAML::Node& n) {
        return new DataStructureDecl(Ast::Read<ExprList>(n["args"]),
                                     Ast::Read<ExprList>(n["supers"]),
                                     Ast::Read<ExprList>(n["body"]));
    }

    template<>
    void PrintNode(const DataStructureDecl& expr, std::ostream& os, size_t tab) {
        if (!expr.args->items.empty()) {
            os << "(";
            bool first = true;
            for (auto& i : expr.args->items) {
                os << (first ? "" : ", ");
                DoPrintNode(*i, os, tab);
                first = false;
            }
            os << ")";
        }
        bool first = true;
        for (auto& i : expr.superTypes->items) {
            os << (first ? " : " : ", ");
            DoPrintNode(*i, os, tab);
            first = false;
        }
        os << " => ";
        DoPrintNode(*expr.body, os, tab);
    }

    /* FnDataDecl */
    void FnDataDecl::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginMap;
        o << YAML::Key << "args" << YAML::Value;
        args->Emit(o);
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

    template<>
    Ast* ReadNode<FnDataDecl>(const YAML::Node& n) {
        return new FnDataDecl(Ast::Read<ExprList>(n["args"]),
                              Ast::Read<Expr>(n["ret"]),
                              Ast::Read<Expr>(n["body"]));
    }

    template<>
    void PrintNode(const FnDataDecl& expr, std::ostream& os, size_t tab) {
        assert(false);
    }

//    void ValueDecl::DoEmit(YAML::Emitter& o)


    /** PartialFunction **/
    void PartialFunction::DoEmit(YAML::Emitter& o) {
        o << YAML::BeginSeq;
        for (auto& i : *this) {
            o << YAML::BeginMap;
            o << YAML::Key << "lhs" << YAML::Value;
            if (i.first) {
                i.first->Emit(o);
            }
            else {
                o << "else";
            }
            o << YAML::Key << "rhs" << YAML::Value;
            i.second->Emit(o);
            o << YAML::EndMap;
        }
        o << YAML::EndSeq;
    }

    template<>
    Ast* ReadNode<PartialFunction>(const YAML::Node& n) {
        PartialFunction* ret = new PartialFunction();
        for (auto i : n) {
            if (i["lhs"].IsScalar())
                ret->Add(NULL, Ast::Read<Expr>(i["rhs"]));
            else
                ret->Add(Ast::Read<Expr>(i["lhs"]), Ast::Read<Expr>(i["rhs"]));
        }
        return ret;
    }

    template<>
    void PrintNode(const PartialFunction& expr, std::ostream& os, size_t tab) {
        os << "{";
        for (auto& i : expr) {
            os << NL(tab+4);
            os << "case ";
            if (i.first) {
                DoPrintNode(*i.first, os, tab+8);
            }
            else {
                os << "else";
            }
            os << " => ";
            os << NL(tab+8);
            DoPrintNode(*i.second, os, tab+8);
        }
        os << NL(tab) << "}";
    }



    template<>
    Ast* ReadNode<ConstantString>(const YAML::Node& n) {
        return new ConstantString(n.Scalar());
    }

    template<>
    void PrintNode(const ConstantString& expr, std::ostream& os, size_t tab) {
        os << "\"" << expr.value << "\"";
    }

    template<>
    Ast* ReadNode<ValueDecl>(const YAML::Node& n) {
        return new ValueDecl(Ast::Read<Expr>(n["type"]), Ast::Read<Expr>(n["init"]));
    }

    template<>
    void PrintNode(const ValueDecl& expr, std::ostream& os, size_t tab) {
        assert(false);
    }

    template<>
    Ast* ReadNode<TraitDecl>(const YAML::Node& n) {
        return new TraitDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<DataStructureDecl>(n["decl"]));
    }

    template<>
    void PrintNode(const TraitDecl& expr, std::ostream& os, size_t tab) {
        os << "trait ";
        DoPrintNode(*expr.name, os, tab);
        DoPrintNode(*expr.decl, os, tab);
    }

    template<>
    Ast* ReadNode<ClassDecl>(const YAML::Node& n) {
        return new ClassDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<DataStructureDecl>(n["decl"]));
    }

    template<>
    void PrintNode(const ClassDecl& expr, std::ostream& os, size_t tab) {
        os << "class ";
        DoPrintNode(*expr.name, os, tab);
        DoPrintNode(*expr.decl, os, tab);
    }

    template<>
    Ast* ReadNode<StructDecl>(const YAML::Node& n) {
        return new StructDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<DataStructureDecl>(n["decl"]));
    }

    template<>
    void PrintNode(const StructDecl& expr, std::ostream& os, size_t tab) {
        os << "struct ";
        DoPrintNode(*expr.name, os, tab);
        DoPrintNode(*expr.decl, os, tab);
    }

    template<>
    Ast* ReadNode<ValDecl>(const YAML::Node& n) {
        return new ValDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<ValueDecl>(n["decl"]));
    }

    template<>
    void PrintNode(const ValDecl& expr, std::ostream& os, size_t tab) {
        os << "val ";
        DoPrintNode(*expr.name, os, tab);
        os << " = ";
        DoPrintNode(*expr.decl->init, os, tab);
    }

    template<>
    Ast* ReadNode<VarDecl>(const YAML::Node& n) {
        return new VarDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<ValueDecl>(n["decl"]));
    }

    template<>
    void PrintNode(const VarDecl& expr, std::ostream& os, size_t tab) {
        os << "var ";
        DoPrintNode(*expr.name, os, tab);
        if (expr.decl->type) {
            os << " : ";
            DoPrintNode(*expr.decl->type, os, tab);
        }
        if (expr.decl->init && expr.decl->type) {
            os << " => ";
            DoPrintNode(*expr.decl->init, os, tab);
        } else if (expr.decl->init) {
            os << " = ";
            DoPrintNode(*expr.decl->init, os, tab);
        }
    }

    template<>
    Ast* ReadNode<FnDecl>(const YAML::Node& n) {
        return new FnDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<FnDataDecl>(n["decl"]));
    }

    template<>
    void PrintNode(const FnDecl& expr, std::ostream& os, size_t tab) {
        os << "def ";
        DoPrintNode(*expr.name, os, tab);
        os << "(";
        bool first = true;
        for (auto& i : expr.decl->args->items) {
            os << (first ? "" : ", ");
            DoPrintNode(*i, os, tab);
            first = false;
        }
        os << ")";
        if (expr.decl->type) {
            os << " : ";
            DoPrintNode(*expr.decl->type, os, tab);
        }
        if (expr.decl->body) {
            os << " => ";
            DoPrintNode(*expr.decl->body, os, tab);
        }
    }

    template<>
    Ast* ReadNode<ArgDecl>(const YAML::Node& n) {
        return new ArgDecl(V_DEFAULT, Ast::Read<IdentifierExpr>(n["name"]), Ast::Read<Expr>(n["decl"]));
    }

    template<>
    void PrintNode(const ArgDecl& expr, std::ostream& os, size_t tab) {
        DoPrintNode(*expr.name, os, tab);
        os << " : ";
        DoPrintNode(*expr.decl, os, tab);
    }

    template<>
    Ast* ReadNode<ModuleDecl>(const YAML::Node& n) {
        return new ModuleDecl(V_DEFAULT, Ast::Read<Name>(n["name"]), Ast::Read<ExprList>(n["decl"]));
    }

    template<>
    void PrintNode(const ModuleDecl& expr, std::ostream& os, size_t tab) {
        os << "mod ";
        DoPrintNode(*expr.name, os, tab);
        DoPrintNode(*expr.decl, os, tab);
    }
}

std::ostream& operator<<(std::ostream& os, const Ides::Ast& data) {
    Ides::DoPrintNode(data, os, 0);
    return os;
}

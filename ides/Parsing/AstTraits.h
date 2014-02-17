//
//  AstTraits.h
//  ides
//
//  Created by Sean Edwards on 1/6/14.
//
//

#ifndef ides_AstTraits_h
#define ides_AstTraits_h

#define BOOST_PP_VARIADICS 1

#include <ides/Util/Visitor.h>

#define AST_TYPES \
    (QualExpr)\
    (IdentifierExpr)\
    (ExprList)\
    (CallExpr)\
    (IndexExpr)\
    (ConstantExpr)\
    (BinaryExpr)\
    (UnaryExpr)\
    (Name)\
    (RecordDecl)\
    (ValueDecl)\
    (ValDecl)\
    (VarDecl)\
    (FnDataDecl)\
    (FnDecl)\
    (ArgDecl)\
    (ModuleDecl)\
    (PartialFunction)\
    (DataStructureDecl)

namespace Ides {
    DECL_TRAITS(AST_TYPES, Ast)
}

#define DECL_AST_VISITOR(name, ret, ...) DECL_VISITOR(AST_TYPES, AstBase, name, ret, __VA_ARGS__)

#endif

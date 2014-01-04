//
//  Tokens.h
//  ides
//
//  Created by Sean Edwards on 12/31/13.
//
//

#ifndef ides_Tokens_h
#define ides_Tokens_h

namespace Ides {
    enum Token {
        TOK_UNKNOWN = 1000,
        TOK_WHITESPACE,

        KW_VOID,
        KW_UNIT,
        KW_BOOL,
        KW_INT8,
        KW_UINT8,
        KW_INT16,
        KW_UINT16,
        KW_INT32,
        KW_UINT32,
        KW_INT64,
        KW_UINT64,
        KW_FLOAT32,
        KW_FLOAT64,

        KW_TRUE,
        KW_FALSE,

        // Specifiers
        KW_PUBLIC,
        KW_PROTECTED,
        KW_INTERNAL,
        KW_PRIVATE,
        KW_EXTERN,
        KW_CONST,

        // Keywords
        KW_DEF,
        KW_FN,
        KW_STRUCT,
        KW_CLASS,
        KW_TRAIT,
        KW_VAR,
        KW_VAL,
        KW_NULL,
        KW_NAMESPACE,

        KW_IF, // if
        KW_ELSE, // else
        KW_DO, // do
        KW_WHILE, // while
        KW_FOR, // for

        KW_TRIDOT, // ...

        // Keyword operators
        KW_THROW,
        KW_NEW,
        KW_RETURN,

        // Operators
        OP_INC,
        OP_DEC,
        OP_COALESCE,
        OP_CAST,

        OP_EQ,
        OP_NE,
        OP_LT,
        OP_LE,
        OP_GT,
        OP_GE,

        OP_AND,
        OP_OR,
        OP_NOT,

        OP_BAND,
        OP_BOR,
        OP_BXOR,
        OP_BNOT,
        OP_ASHL,
        OP_ASHR,
        OP_LSHL,
        OP_LSHR,

        OP_ASSIGN,
        OP_PLUS,
        OP_MINUS,
        OP_STAR,
        OP_SLASH,
        OP_MOD,

        OP_RARROW,
        OP_LARROW,
    };
}

#endif

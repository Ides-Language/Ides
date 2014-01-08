#ifndef _IDES_LANG_H_
#define _IDES_LANG_H_

namespace Ides {


    enum ValKind {
        VAL,
        VAR
    };

    enum DataKind {
        NONE,
        TRAIT,
        CLASS,
        STRUCT
    };

    enum Visibility {
        V_PRIVATE,
        V_PROTECTED,
        V_INTERNAL,
        V_PUBLIC,

        V_EXTERN = 1 << 10,
        V_CONST = 1 << 11,
        V_ABSTRACT = 1 << 12,

        V_DEFAULT = V_PRIVATE
    };

    
}


#endif // _IDES_LANG_H_

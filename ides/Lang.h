#ifndef _IDES_LANG_H_
#define _IDES_LANG_H_

namespace Ides {


    enum ValKind {
        VAL,
        VAR
    };

    enum RecordKind {
        STRUCT,
        CLASS,
        TRAIT,
        TYPE,
        IMPL
    };

    enum Qualifiers {
        QUAL_PRIVATE,
        QUAL_PROTECTED,
        QUAL_INTERNAL,
        QUAL_PUBLIC,

        QUAL_EXTERN,
        QUAL_CONST,
        QUAL_ABSTRACT,
        QUAL_UNSAFE,
        QUAL_INTRINSIC,
        QUAL_IMPLICIT,
        QUAL_LOCKED,

        QUAL_COUNT
    };

    
}


#endif // _IDES_LANG_H_

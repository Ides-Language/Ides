#ifndef _IDES_PARSER_IMPL_H_
#define _IDES_PARSER_IMPL_H_

//#define BOOST_SPIRIT_LEXERTL_DEBUG 1
#define BOOST_SPIRIT_QI_DEBUG 1

extern "C" {
#include <stdint.h>
}

#include <iostream>

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/support_utree.hpp>

#define LEX_STRING_REGEX "\\\"[^\\\"]*\\\""
#define LEX_CHAR_REGEX "'\\\\?.'"

#define LEX_DECNUM_REGEX "-?[0-9][1-9]*"
#define LEX_HEXNUM_REGEX "-?0x[0-9A-F]+"
#define LEX_OCTNUM_REGEX "-?0[0-7]+"
#define LEX_BINNUM_REGEX "-?0b[0|1]+"

#define NAME(x) x.name(#x)

namespace Ides {
namespace Parsing{
namespace detail {

    using namespace boost::spirit;
    using namespace boost;
    
    template<typename Lexer>
    class IdesLexer : public lex::lexer<Lexer>
    {
    public:
        IdesLexer() : lex::lexer<Lexer>(lex::match_flags::match_not_dot_newline), 
        identifier_("[A-Za-z_][A-Za-z0-9_]*"),
        
        char_(LEX_CHAR_REGEX),
        wchar_("W" LEX_CHAR_REGEX),
        lchar_("L" LEX_CHAR_REGEX),
        
        string_(LEX_STRING_REGEX),
        cstring_("C" LEX_STRING_REGEX),
        wcstring_("W" LEX_STRING_REGEX),
        lcstring_("L" LEX_STRING_REGEX),
        
        decnumber_(LEX_DECNUM_REGEX),
        hexnumber_(LEX_HEXNUM_REGEX),
        octnumber_(LEX_OCTNUM_REGEX),
        binnumber_(LEX_BINNUM_REGEX),
        
        udecnumber_(LEX_DECNUM_REGEX "u"),
        uhexnumber_(LEX_HEXNUM_REGEX "u"),
        uoctnumber_(LEX_OCTNUM_REGEX "u"),
        ubinnumber_(LEX_BINNUM_REGEX "u"),
        
        fltnumber_("-?[0-9+]\\.[0-9]*"),
        
        extern_("extern"), public_("public"), private_("private"), protected_("protected"), internal_("internal"),
        
        var_("var"), val_("val"), lazy_("lazy"), def_("def"),
        
        class_("class"), struct_("struct"),
        
        if_("if"), while_("while"), for_("for"), do_("do"),
        return_("return")
        
        {
            this->self = lex::token_def<>('=')
            | '+' | '-' | '*' | '/' | '%'
            | '^' | '&' | '|' | '~' | '!'
            | '<' | '>'
            | '@' | '$' | '#'
            
            | ',' | '(' | ')' | '{' | '}' | '[' | ']' | ':' | ';'
            
            | extern_ | public_ | private_ | protected_ | internal_
            
            | class_ | struct_
            
            | var_ | val_ | lazy_ | def_
            
            | if_ | while_ | for_ | do_ | return_
            
            | char_ | wchar_ | lchar_
            
            | string_ | cstring_ | wcstring_ | lcstring_
            
            | fltnumber_ 
            | ( udecnumber_ | uhexnumber_ | uoctnumber_ | ubinnumber_ )
            | ( decnumber_ | hexnumber_ | octnumber_ | binnumber_ )
            
            | identifier_
            
            | lex::string("[ \\t\\n]+") [ lex::_pass = lex::pass_flags::pass_ignore ]
            ;
        }
        
        lex::token_def<std::string>
        /* Identifiers */
            identifier_,
        
        /* String and character literals */
            char_, wchar_, lchar_,
            string_, cstring_, wcstring_, lcstring_;
        
        /* Signed integral literals */
        lex::token_def<int64_t>
            decnumber_, hexnumber_, octnumber_, binnumber_;
        
        /* Unsigned integral literals */
        lex::token_def<uint64_t>
            udecnumber_, uhexnumber_, uoctnumber_, ubinnumber_;
        
        /* Floating point literals */
        lex::token_def<double>
            fltnumber_;
        
        /* Keywords */
        lex::token_def<lex::omit>
            extern_, public_, private_, protected_, internal_,
        
            var_, val_, lazy_, def_,
        
            class_, struct_,
        
            if_, while_, for_, do_, return_
            ;
        
        
    }; // class IdesLexer
    
    template<typename Iterator, typename Lexer>
    class IdesParser : public qi::grammar<Iterator, utree()>
    {
    public:
        
        template<typename TokenDef>
        IdesParser(const TokenDef& tok) : IdesParser::base_type(compilation_unit)
        {
            compilation_unit
                = *function_def
                ;
            
            qualifier = tok.extern_ | tok.private_ | tok.public_ | tok.protected_ | tok.internal_;
            
            var_def
                = (tok.var_ | tok.val_) > tok.identifier_ >
                    (':' > tok.identifier_
                     | '=' > expression
                     )
                ;
            
            arg_list
                = var_def % ','
                ;
            
            function_decl // def f (var_def, var_def, ...) : type
                = tok.def_ > tok.identifier_ > '(' > arg_list > ')'// > ~(':' > tok.identifier_)
                ;
            
            function_def
                = function_decl >
                    ('=' > expression > ';'
                     | compound_stmt)
                ;
            
            compound_stmt
                = lit('{') > *stmt > lit('}')
                ;
            
            stmt_list = *stmt;
             
            stmt
                = expression > ';'
            ;
            
            return_expr = tok.return_ >> ~expression;
            
            expression = tok.decnumber_;
            
            qi::on_error<qi::fail>(compilation_unit,
                                   std::cout << phoenix::val("Error! Expecting ")
                                   << qi::_4                               // what failed?
                                   << phoenix::val(" here: \"")
                                   << phoenix::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
                                   << phoenix::val("\"")
                                   << std::endl
                                   );
            
            NAME(function_decl);
            NAME(function_def);
            NAME(class_def);
            NAME(struct_def);
            NAME(var_def);
            NAME(arg_list);
            NAME(stmt);
            NAME(stmt_list);
            NAME(compound_stmt);
            NAME(expression);
            NAME(return_expr);
            NAME(qualifier);
            
            debug(compilation_unit);
        }
        
        qi::rule<Iterator, utree()> compilation_unit;
        
        qi::rule<Iterator, utree()> function_decl;
        
        qi::rule<Iterator, utree()> function_def, class_def, struct_def, var_def, arg_list;
        
        qi::rule<Iterator, utree()> stmt, stmt_list, compound_stmt;
        
        qi::rule<Iterator, utree()> expression, return_expr;
        
        qi::rule<Iterator, utree()> qualifier;
    }; // class IdesParser
    
     
} // namespace detail
} // namespace Parsing
} // namespace Ides

#endif // _IDES_PARSER_IMPL_H_


#include <ides/Parsing/Parser.h>
#include <ides/Parsing/ParserImpl.h>

#include <iostream>
#include <iomanip>

namespace Ides {
namespace Parsing {
    
    using namespace boost::spirit;
    
    typedef classic::position_iterator2<boost::spirit::istream_iterator> iterator_type;
    
    typedef lex::lexertl::token<iterator_type, boost::mpl::vector<int64_t, uint64_t, std::string, double> > token_type;
    typedef lex::lexertl::actor_lexer<token_type> lexer_type;
    
    typedef Ides::Parsing::detail::IdesLexer<lexer_type> Lexer;
    typedef Ides::Parsing::detail::IdesParser<Lexer::iterator_type, Lexer::lexer_def> Parser;
  
    
Ides::AST::AST::Ptr Parse(std::istream& is, const std::string& srcname)
{
    is.unsetf(std::ios::skipws);
    
    istream_iterator stream_iter(is);
    istream_iterator stream_end;
    
    iterator_type src_iter(stream_iter, stream_end, srcname);
    iterator_type src_end;
    
    Lexer lexer;
    Parser parser(lexer);
    
    try {
        boost::spirit::utree t;
        bool r = lex::tokenize_and_parse(src_iter, src_end, lexer, parser, t);
        
        if (r && src_iter == src_end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << t << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \": " << *src_iter << "...\"\n";
            std::cout << "-------------------------\n";
        }
        /**/
        
        
    }
    catch(const qi::expectation_failure<iterator_type>& e)
    {
        const classic::file_position_base<std::string>& pos =
        e.first.get_position();
        std::stringstream msg;
        msg <<
        "syntax error: " << e.what() << std::endl <<
        " at " << pos.file <<
        ":" << pos.line << " column " << pos.column << std::endl <<
        "'" << e.first.get_currentline() << "'" << std::endl <<
        std::setw(pos.column) << " " << "^- here";
        throw std::runtime_error(msg.str());
    }
    return Ides::AST::AST::Ptr();
}
        
        
} // namespace Parsing
} // namespace Ides

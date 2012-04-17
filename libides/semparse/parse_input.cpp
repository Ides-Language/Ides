#include "parse_input.hpp"

using namespace std;
using namespace semparse;
using namespace semparse::resolution_policy;
using namespace semparse::utility;

semparse::semantic_graph_node DummyWhitespaceParent;
semparse::universal_address DummyDeepestAddress;
token_rules generate_token_rules()
{
	std::vector<std::string> DroppedTokens;
	{
		DroppedTokens.push_back(" ");
		DroppedTokens.push_back( "\n");
		DroppedTokens.push_back( "\r");
		DroppedTokens.push_back( "\t");
	}
	std::vector<std::string> KeptTokens;
	{
		KeptTokens.push_back("+");
		KeptTokens.push_back( "-");
		KeptTokens.push_back( "*");
		KeptTokens.push_back( "/");
		KeptTokens.push_back( "%");
		KeptTokens.push_back( "^");
		KeptTokens.push_back( "&");
		KeptTokens.push_back( "|");
		KeptTokens.push_back( "~");
		KeptTokens.push_back( "!");
		KeptTokens.push_back( "=");
		KeptTokens.push_back( "<");
		KeptTokens.push_back( ">");
		KeptTokens.push_back( 
"+=");
		KeptTokens.push_back( "-=");
		KeptTokens.push_back( "*=");
		KeptTokens.push_back( "/=");
		KeptTokens.push_back( "%=");
		KeptTokens.push_back( "^=");
		KeptTokens.push_back( "&=");
		KeptTokens.push_back( "|=");
		KeptTokens.push_back( "<<");
		KeptTokens.push_back( ">>");
		KeptTokens.push_back( "<<=");
		KeptTokens.push_back( ">>=");
		KeptTokens.push_back( "==");
		KeptTokens.push_back( "!=");
		KeptTokens.push_back( "<=");
		KeptTokens.push_back( ">=");
		KeptTokens.push_back( "&&");
		KeptTokens.push_back( "||");
		KeptTokens.push_back( "++");
		KeptTokens.push_back( "--");
		KeptTokens.push_back( ",");
		KeptTokens.push_back( "->*");
		KeptTokens.push_back( "->");
		KeptTokens.push_back(
"\"");
		KeptTokens.push_back( ",");
		KeptTokens.push_back( ";");
		KeptTokens.push_back( "(");
		KeptTokens.push_back( ")");
		KeptTokens.push_back( "==");
		KeptTokens.push_back( "<=");
		KeptTokens.push_back( "[");
		KeptTokens.push_back( "]");
		KeptTokens.push_back( "[[");
		KeptTokens.push_back( "]]");
		KeptTokens.push_back( ":");
		KeptTokens.push_back( "//");
		KeptTokens.push_back( ".");
		KeptTokens.push_back( 
"{");
		KeptTokens.push_back( "}");
		KeptTokens.push_back( "=>");
	}
	token_rules Result(DroppedTokens, KeptTokens);
	return Result;
}

bool parse_cpp_comment_begin(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse cpp_comment_begin\n";
	}
	Result.name = "cpp_comment_begin";
	length ResolutionPolicy = length(2);
	Result.text = Parser.next_static(matches_string("//"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_cpp_comment_character(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse cpp_comment_character\n";
	}
	Result.name = "cpp_comment_character";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_regex("[^\\n]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_cpp_comment_body(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "cpp_comment_body";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_cpp_comment_character, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_cpp_comment(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "cpp_comment";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_cpp_comment_begin, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_cpp_comment_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_newline, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_c_comment_begin(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse c_comment_begin\n";
	}
	Result.name = "c_comment_begin";
	length ResolutionPolicy = length(2);
	Result.text = Parser.next_static(matches_string("/*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_c_comment_end(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse c_comment_end\n";
	}
	Result.name = "c_comment_end";
	length ResolutionPolicy = length(2);
	Result.text = Parser.next_static(matches_string("*/"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_c_comment_character(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse c_comment_character\n";
	}
	Result.name = "c_comment_character";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_regex("[^\\*]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_c_comment_non_terminator(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse c_comment_non_terminator\n";
	}
	Result.name = "c_comment_non_terminator";
	length ResolutionPolicy = length(2);
	Result.text = Parser.next_static(matches_regex("\\*[^/]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_c_comment_body_element(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_c_comment_character, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_c_comment_non_terminator, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'c_comment_body_element'");
}

bool parse_c_comment_body(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "c_comment_body";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_c_comment_body_element, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_c_comment(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "c_comment";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_c_comment_begin, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_c_comment_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_c_comment_end, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_space(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse space\n";
	}
	Result.name = "space";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_string(" "), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_newline(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse newline\n";
	}
	Result.name = "newline";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_string("\n"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_return(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse return\n";
	}
	Result.name = "return";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_string("\r"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_tab(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse tab\n";
	}
	Result.name = "tab";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_string("\t"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_whitespace_x(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse whitespace_x\n";
	}
	Result.name = "whitespace_x";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_static(matches_regex("[ \\n\\r\\t]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

semparse::semantic_graph_node DummyWhitespaceNode;
void whitespace_consume(semparse::parser& Parser, semparse::semantic_graph_node& TargetNode, const bool Verbose)
{
	bool WhitespaceEncountered = false;
	do
	{
		WhitespaceEncountered = false;
		{
			semantic_graph_node TempNode;
			if(try_parse(Parser, parse_whitespace_x, TempNode))
			{
				TargetNode.append_child(TempNode);
				WhitespaceEncountered = true;
				continue;
			}
		}
		{
			semantic_graph_node TempNode;
			if(try_parse(Parser, parse_cpp_comment, TempNode))
			{
				TargetNode.append_child(TempNode);
				WhitespaceEncountered = true;
				continue;
			}
		}
		{
			semantic_graph_node TempNode;
			if(try_parse(Parser, parse_c_comment, TempNode))
			{
				TargetNode.append_child(TempNode);
				WhitespaceEncountered = true;
				continue;
			}
		}
	} while(WhitespaceEncountered == true);
}

bool parse_operator_plus(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_plus\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_plus";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("+"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_minus(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_minus\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_minus";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("-"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_multiply(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_multiply\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_multiply";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_divide(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_divide\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_divide";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("/"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_modulo(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_modulo\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_modulo";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("%"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_xor(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_xor\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_xor";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("^"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_and(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_and\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_and";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("&"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_or(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_or\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_or";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("|"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_complement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_complement\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_complement";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("~"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_not(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_not\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_not";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("!"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_less_than(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_less_than\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_less_than";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("<"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_greater_than(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_greater_than\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_greater_than";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(">"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_plus_plus(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_plus_plus\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_plus_plus";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("++"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_minus_minus(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_minus_minus\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_minus_minus";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("--"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_plus_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_plus_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_plus_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("+="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_minus_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_minus_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_minus_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("-="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_multiply_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_multiply_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_multiply_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("*="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_divide_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_divide_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_divide_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("/="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_modulo_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_modulo_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_modulo_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("%="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_xor_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_xor_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_xor_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("^="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_not_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_not_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_not_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("!="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_equal_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_equal_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_equal_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("=="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_less_than_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_less_than_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_less_than_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("<="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_greater_than_equal(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_greater_than_equal\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_greater_than_equal";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(">="), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_logical_and(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_logical_and\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_logical_and";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("&&"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_logical_or(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_logical_or\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_logical_or";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("||"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_dot(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_dot\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_dot";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("."), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_opening_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_opening_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_opening_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("("), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_closing_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_closing_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_closing_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(")"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_opening_square_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_opening_square_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_opening_square_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("["), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_closing_square_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_closing_square_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_closing_square_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_question_mark(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_question_mark\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_question_mark";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("?"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_colon(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_colon\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_colon";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(":"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_comma(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse operator_comma\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "operator_comma";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(","), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_call_operator(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_plus, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_minus, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_multiply, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_divide, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_modulo, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_xor, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_and, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_or, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_complement, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_not, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_less_than, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_greater_than, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_plus_plus, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_minus_minus, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_plus_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_minus_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_multiply_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_modulo_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_xor_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_not_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_equal_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_less_than_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_greater_than_equal, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_logical_and, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_logical_or, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_dot, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_question_mark, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_colon, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'function_call_operator'");
}

bool parse_operator(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_function_call_operator, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator_comma, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'operator'");
}

bool parse_identifier(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse identifier\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "identifier";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_double_quote(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse double_quote\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "double_quote";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("\""), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_string_constant_character(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse string_constant_character\n";
	}
	Result.name = "string_constant_character";
	length ResolutionPolicy = length(1);
	Result.text = Parser.next_static(matches_regex("[^\\\\\\\"]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_string_constant_escaped_character(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse string_constant_escaped_character\n";
	}
	Result.name = "string_constant_escaped_character";
	length ResolutionPolicy = length(2);
	Result.text = Parser.next_static(matches_regex("\\\\[^\\n]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_string_constant_body_element(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_string_constant_character, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_string_constant_escaped_character, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'string_constant_body_element'");
}

bool parse_string_constant_body(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "string_constant_body";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_string_constant_body_element, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_string_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "string_constant";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_double_quote, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_string_constant_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_double_quote, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_statement_terminator(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse statement_terminator\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "statement_terminator";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(";"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_struct_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse struct_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "struct_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("struct"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_struct_name(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse struct_name\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "struct_name";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_struct_declaration(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "struct_declaration";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_struct_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_struct_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_bool(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse bool\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "bool";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("bool"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_int8(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse int8\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "int8";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("int8"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_uint8(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse uint8\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "uint8";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("uint8"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_int16(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse int16\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "int16";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("int16"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_uint16(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse uint16\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "uint16";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("uint16"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_int32(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse int32\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "int32";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("int32"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_uint32(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse uint32\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "uint32";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("uint32"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_int64(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse int64\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "int64";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("int64"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_uint64(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse uint64\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "uint64";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("uint64"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_float32(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse float32\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "float32";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("float32"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_float64(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse float64\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "float64";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("float64"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_int8(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_int8\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_int8";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_uint8(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_uint8\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_uint8";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_int16(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_int16\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_int16";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_uint16(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_uint16\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_uint16";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_int32(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_int32\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_int32";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_uint32(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_uint32\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_uint32";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_int64(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_int64\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_int64";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_uint64(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_uint64\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_uint64";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_float32(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_float32\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_float32";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_literal_float64(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse literal_float64\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "literal_float64";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_int8, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_uint8, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_int16, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_uint16, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_int32, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_uint32, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_int64, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_uint64, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_float32, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_literal_float64, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_c_string_constant, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_wide_string_constant, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_long_string_constant, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'constant'");
}

bool parse_c_string_prefix(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse c_string_prefix\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "c_string_prefix";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("C"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_wide_string_prefix(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse wide_string_prefix\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "wide_string_prefix";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("WC"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_long_string_prefix(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse long_string_prefix\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "long_string_prefix";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("LC"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_c_string_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "c_string_constant";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_c_string_prefix, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_string_constant, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_wide_string_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "wide_string_constant";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_wide_string_prefix, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_string_constant, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_long_string_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "long_string_constant";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_long_string_prefix, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_string_constant, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_array_begin(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse array_begin\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "array_begin";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("float32"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_array_end(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse array_end\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "array_end";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("float64"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_array(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "array";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_array_begin, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_constant, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
		else
		{
			return false;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_constant, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_array_end, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_public_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse public_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "public_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("public"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_private_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse private_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "private_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("private"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_internal_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse internal_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "internal_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("internal"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_extern_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse extern_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "extern_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("extern"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_access_declaration(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_public_keyword, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_private_keyword, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_internal_keyword, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_extern_keyword, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'function_access_declaration'");
}

bool parse_opening_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse opening_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "opening_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("("), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_closing_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse closing_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "closing_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(")"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_comma(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse comma\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "comma";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(","), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_parameter_name(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse function_parameter_name\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "function_parameter_name";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_parameter(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_parameter";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_var_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_parameter_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_colon, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_function_parameter_list_elements(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_parameter_list_elements";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_anonymous_element_0, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			for(size_t i = 0; i < TempNode.children.size(); ++i)
			{
				Result.append_child(TempNode.children[i]);
			}
			TempNode.clear();
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_function_parameter, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	return true;
}

bool parse_function_parameter_list(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_parameter_list";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_parameter_list_elements, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_type_name_identifier(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse type_name_identifier\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "type_name_identifier";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_pointer_declaration(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse pointer_declaration\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "pointer_declaration";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_type_name(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "type_name";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_name_identifier, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_pointer_declaration, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_def_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse def_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "def_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("def"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_var_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse var_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "var_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("var"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_colon(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse colon\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "colon";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string(":"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_variable_name(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse variable_name\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "variable_name";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_opening_square_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse opening_square_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "opening_square_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("["), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_closing_square_bracket(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse closing_square_bracket\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "closing_square_bracket";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_count(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse count\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "count";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_array_count(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "array_count";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_square_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_count, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_square_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_assignment_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "assignment_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_operator_equal, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_variable_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_uninitialized_variable_definition, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_initialized_variable_definition, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'variable_definition'");
}

bool parse_uninitialized_variable_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "uninitialized_variable_definition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_var_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_variable_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_colon, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_array_count, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_assignment_expression, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_initialized_variable_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "initialized_variable_definition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_var_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_variable_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_array_count, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_assignment_expression, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_octal_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse octal_constant\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "octal_constant";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("0[0-7]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_decimal_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse decimal_constant\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "decimal_constant";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[1-9][0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_hexadecimal_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse hexadecimal_constant\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "hexadecimal_constant";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("0x[0-9A-Fa-f]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_float_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse float_constant\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "float_constant";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*\\.[0-9]*[fF]"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_double_constant(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse double_constant\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "double_constant";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[0-9]*\\.[0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_bracketed_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "bracketed_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_subscript_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "subscript_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_square_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_square_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_function_call_parameter_expression_element(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_function_call_operator, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_bracketed_expression, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_subscript_expression, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_function_call, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_identifier, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_constant, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'function_call_parameter_expression_element'");
}

bool parse_function_call_parameter_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_call_parameter_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_function_call_parameter_expression_element, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
		else
		{
			return false;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_function_call_parameter_expression_element, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_function_call_parameters(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_call_parameters";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_anonymous_element_1, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			for(size_t i = 0; i < TempNode.children.size(); ++i)
			{
				Result.append_child(TempNode.children[i]);
			}
			TempNode.clear();
		}
	}
	return true;
}

bool parse_function_call_parameter_list(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_call_parameter_list";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_call_parameters, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_function_call(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_call";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_identifier, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_call_parameter_list, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_expression_element(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_function_call, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_operator, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_identifier, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_bracketed_expression, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_subscript_expression, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_constant, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'expression_element'");
}

bool parse_expression_body(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "expression_body";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_expression_element, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_statement_block(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "statement_block";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_brace, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_function_statement, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_brace, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_for_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse for_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "for_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("for"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_for_initialization_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "for_initialization_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	return true;
}

bool parse_for_condition_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "for_condition_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	return true;
}

bool parse_for_post_expression(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "for_post_expression";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	return true;
}

bool parse_for_header(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "for_header";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_for_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_for_initialization_expression, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_for_condition_expression, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_for_post_expression, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_for_loop(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "for_loop";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_for_header, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_block, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_return_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse return_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "return_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("return"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_return_statement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "return_statement";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_return_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_if_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse if_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "if_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("if"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_if_condition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "if_condition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_expression_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_statement_block_or_statement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_statement_block, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_expression, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'statement_block_or_statement'");
}

bool parse_else_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse else_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "else_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("else"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_else_block(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "else_block";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_else_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_block_or_statement, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_if_statement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "if_statement";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_if_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_if_condition, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_bracket, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_block_or_statement, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_else_block, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	return true;
}

bool parse_typedef_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse typedef_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "typedef_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("typedef"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_type_alias(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse type_alias\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "type_alias";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_typedef_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "typedef_definition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_typedef_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_alias, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_function_statement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_return_statement, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_variable_definition, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_for_loop, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_if_statement, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_typedef_definition, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_expression, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'function_statement'");
}

bool parse_function_body(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_body";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_function_statement, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_function_name_identifier(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse function_name_identifier\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "function_name_identifier";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("[_A-Za-z][_A-Za-z0-9]*"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_name_operator(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse function_name_operator\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "function_name_operator";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_regex("(\\+|\\*|\\\\|/|\\!)"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_name(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_function_name_identifier, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_function_name_operator, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'function_name'");
}

bool parse_return_type_operator(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse return_type_operator\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "return_type_operator";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("=>"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_function_return_type_declaration(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_return_type_declaration";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_return_type_operator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_function_declaration(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_declaration";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_function_access_declaration, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_def_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_parameter_list, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_function_return_type_declaration, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_opening_brace(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse opening_brace\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "opening_brace";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("{"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_closing_brace(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse closing_brace\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "closing_brace";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("}"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_infix(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse infix\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "infix";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("infix"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_prefix(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse prefix\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "prefix";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("prefix"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_operator_type(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_infix, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_prefix, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'operator_type'");
}

bool parse_function_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "function_definition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_function_access_declaration, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_def_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_operator_type, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_parameter_list, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		if(try_parse(Parser, parse_function_return_type_declaration, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
		}
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_brace, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_body, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_brace, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_import_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse import_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "import_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("import"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_import_statement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "import_statement";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_import_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_string_constant, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_header_keyword(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	if(Verbose)
	{
		cout << "Attempting to parse header_keyword\n";
	}
	if(Verbose)
	{
		cout << "	Consuming whitespace\n";
	}
	whitespace_consume(Parser, WhitespaceParent);
	Result.name = "header_keyword";
	longest ResolutionPolicy = longest();
	Result.text = Parser.next_auto(matches_string("header"), ResolutionPolicy, Result.begin_address, Result.end_address);
	Result.value = Result.text;
	if(Verbose)
	{
		cout << "Parse result:" <<  (!(Result.value.empty())) << "\n";
	}
	return (!(Result.value.empty()));
}

bool parse_header_statement(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "header_statement";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_header_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_string_constant, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_struct_member(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "struct_member";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_variable_definition, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_member_names_element(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_struct_member, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParent;
		if(try_parse(Parser, parse_comma, TempNode, TempWhitespaceParent, DeepestAddress))
		{
			WhitespaceParent = TempWhitespaceParent;
			Result = TempNode;
			return true;
		}
	}
	return false;
	throw parse_exception("Cannot match any element from alias list 'member_names_element'");
}

bool parse_struct_member_names(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "struct_member_names";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_member_names_element, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_struct_members_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "struct_members_definition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_type_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_struct_member_names, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_statement_terminator, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_struct_members(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "struct_members";
	Result.begin_address = Result.end_address = Parser.address();
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_variable_definition, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			Result.append_child(TempNode);
			TempNode.clear();
		}
	}
	return true;
}

bool parse_struct_definition(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "struct_definition";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_struct_keyword, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_struct_name, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_opening_brace, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_struct_members, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_closing_brace, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

root_parser::root_parser()
{
	TokenRules = generate_token_rules();
	Parser.set_token_rules(TokenRules);
}

semparse::parse_results root_parser::parse(const char* const Input, semparse::semantic_graph_node& TargetNode, const bool Verbose)
{
	semparse::parse_results Result;
	Parser.append(Input);
	int FailedParseAttempts = 0;
	universal_address DeepestAddress = Parser.address();
	universal_address MostLikelyAddress = DeepestAddress;
	string MostLikelyName;
	while(!Parser.empty())
	{
		universal_address MostLikelyBegin = Parser.address();
		if(FailedParseAttempts > 1)
		{
			Result.Successful = false;
			Result.MostLikelyBeginAddress = MostLikelyBegin;
			Result.MostLikelyEndAddress = MostLikelyAddress;
			Result.MostLikelyName = MostLikelyName;
			return Result;
		}
		{
			semantic_graph_node TempNode;
			semantic_graph_node TempWhitespaceParentNode;
			universal_address TempAddress;
			if(try_parse(Parser, parse_function_declaration, TempNode, TempWhitespaceParentNode, TempAddress))
			{
				for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
				{
					TargetNode.append_child(TempWhitespaceParentNode.children[i]);
				}
				TargetNode.append_child(TempNode);
				FailedParseAttempts = 0;
				DeepestAddress = Parser.address();
				goto post_parse;
			}
			else
			{
				if(TempAddress > MostLikelyAddress)
				{
					MostLikelyAddress = TempAddress;
					MostLikelyName = "function_declaration";
				}
			}
		}
		{
			semantic_graph_node TempNode;
			semantic_graph_node TempWhitespaceParentNode;
			universal_address TempAddress;
			if(try_parse(Parser, parse_function_definition, TempNode, TempWhitespaceParentNode, TempAddress))
			{
				for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
				{
					TargetNode.append_child(TempWhitespaceParentNode.children[i]);
				}
				TargetNode.append_child(TempNode);
				FailedParseAttempts = 0;
				DeepestAddress = Parser.address();
				goto post_parse;
			}
			else
			{
				if(TempAddress > MostLikelyAddress)
				{
					MostLikelyAddress = TempAddress;
					MostLikelyName = "function_definition";
				}
			}
		}
		{
			semantic_graph_node TempNode;
			semantic_graph_node TempWhitespaceParentNode;
			universal_address TempAddress;
			if(try_parse(Parser, parse_import_statement, TempNode, TempWhitespaceParentNode, TempAddress))
			{
				for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
				{
					TargetNode.append_child(TempWhitespaceParentNode.children[i]);
				}
				TargetNode.append_child(TempNode);
				FailedParseAttempts = 0;
				DeepestAddress = Parser.address();
				goto post_parse;
			}
			else
			{
				if(TempAddress > MostLikelyAddress)
				{
					MostLikelyAddress = TempAddress;
					MostLikelyName = "import_statement";
				}
			}
		}
		{
			semantic_graph_node TempNode;
			semantic_graph_node TempWhitespaceParentNode;
			universal_address TempAddress;
			if(try_parse(Parser, parse_header_statement, TempNode, TempWhitespaceParentNode, TempAddress))
			{
				for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
				{
					TargetNode.append_child(TempWhitespaceParentNode.children[i]);
				}
				TargetNode.append_child(TempNode);
				FailedParseAttempts = 0;
				DeepestAddress = Parser.address();
				goto post_parse;
			}
			else
			{
				if(TempAddress > MostLikelyAddress)
				{
					MostLikelyAddress = TempAddress;
					MostLikelyName = "header_statement";
				}
			}
		}
		{
			semantic_graph_node TempNode;
			semantic_graph_node TempWhitespaceParentNode;
			universal_address TempAddress;
			if(try_parse(Parser, parse_variable_definition, TempNode, TempWhitespaceParentNode, TempAddress))
			{
				for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
				{
					TargetNode.append_child(TempWhitespaceParentNode.children[i]);
				}
				TargetNode.append_child(TempNode);
				FailedParseAttempts = 0;
				DeepestAddress = Parser.address();
				goto post_parse;
			}
			else
			{
				if(TempAddress > MostLikelyAddress)
				{
					MostLikelyAddress = TempAddress;
					MostLikelyName = "variable_definition";
				}
			}
		}
		{
			semantic_graph_node TempNode;
			semantic_graph_node TempWhitespaceParentNode;
			universal_address TempAddress;
			if(try_parse(Parser, parse_struct_definition, TempNode, TempWhitespaceParentNode, TempAddress))
			{
				for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
				{
					TargetNode.append_child(TempWhitespaceParentNode.children[i]);
				}
				TargetNode.append_child(TempNode);
				FailedParseAttempts = 0;
				DeepestAddress = Parser.address();
				goto post_parse;
			}
			else
			{
				if(TempAddress > MostLikelyAddress)
				{
					MostLikelyAddress = TempAddress;
					MostLikelyName = "struct_definition";
				}
			}
		}
	whitespace_consume(Parser, TargetNode);
	++FailedParseAttempts;
post_parse:
		Parser.trim();
	}
	return Result;
}

void root_parser::address(const semparse::universal_address& NewAddress)
{
	return Parser.address(NewAddress);
}

semparse::universal_address root_parser::address() const
{
	return Parser.address();
}


//Anonymous elements
bool parse_anonymous_element_0(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "anonymous_element_0";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_parameter, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_comma, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}

bool parse_anonymous_element_1(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "anonymous_element_1";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_call_parameter_expression, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		while(try_parse(Parser, parse_anonymous_element_2, TempNode, TempWhitespaceParentNode, DeepestAddress))
		{
			for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
			{
				Result.append_child(TempWhitespaceParentNode.children[i]);
			}
			for(size_t i = 0; i < TempNode.children.size(); ++i)
			{
				Result.append_child(TempNode.children[i]);
			}
			TempNode.clear();
		}
	}
	return true;
}

bool parse_anonymous_element_2(parser& Parser, semantic_graph_node& Result, semparse::semantic_graph_node& WhitespaceParent, semparse::universal_address& DeepestAddress, const bool Verbose)
{
	Result.name = "anonymous_element_2";
	Result.begin_address = Result.end_address = Parser.address();
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_comma, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	{
		bool TempBool = true;
		semantic_graph_node TempNode;
		semantic_graph_node TempWhitespaceParentNode;
		TempBool = try_parse(Parser, parse_function_call_parameter_expression, TempNode, TempWhitespaceParentNode, DeepestAddress);
		if(TempBool == false)
		{
			return false;
		}
		for(size_t i = 0; i < TempWhitespaceParentNode.children.size(); ++i)
		{
			Result.append_child(TempWhitespaceParentNode.children[i]);
		}
		Result.append_child(TempNode);
	}
	return true;
}


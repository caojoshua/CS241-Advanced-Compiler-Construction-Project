/*
 * Scanner.cpp
 * Author: Joshua Cao
 */

#include "Scanner.h"

LexAnalysis::Scanner::Scanner(char const* s) : f(s), fname(s), tk(eof), num(0), linenum(1)
{
	if (!f.is_open())
	{
		std::cerr << "cannot open file " << s << std::endl;
		exit(1);
	}
	c = f.get();
}

void LexAnalysis::Scanner::next()
{
	if (f.eof())
	{
		tk = eof;
		return;
	}
//	std::cout << "next called, c is " << int(c) << std::endl;
	switch(c) 
	{
		case 'a' ... 'z':
		case 'A' ... 'Z':
			id = "";
			while((c >= 'a' && c <= 'z')
					|| (c >= 'A' && c <= 'Z')
					|| (c >= '0' && c <= '9'))
			{
				id.append(1,c);
				c = f.get();
			}
			check_keywords();
			break;
		case '0' ... '9':
			num = 0;
			while(c >= '0' && c <= '9')
			{
				num = num * 10 + c - 48;
				c = f.get();
			}
			tk = num_tk;
			break;
		case '=':
			c = f.get();
			switch(c)
			{
				case '=':
					tk = e;
					c = f.get();
					break;
				default:
					err();
			}
			break;
		case '!':
			c = f.get();
			switch(c)
			{
				case '=':
					tk = ne;
					c = f.get();
					break;
				default:
					err();
			}
			break;
		case '<':
			c = f.get();
			switch(c)
			{
				case '-':
					tk = assign;
					c = f.get();
					break;
				case '=':
					tk = lte;
					c = f.get();
					break;
				default:
					tk = lt;
			}
			break;
		case '>':
			c = f.get();
			switch(c)
			{
				case '=':
					tk = gte;
					c = f.get();
					break;
				default:
					tk = gt;
			}
			break;
		case '(':
			tk = open_paren;
			c = f.get();
			break;
		case ')':
			tk = close_paren;
			c = f.get();
			break;
		case '[':
			tk = open_bracket;
			c = f.get();
			break;
		case ']':
			tk = close_bracket;
			c = f.get();
			break;
		case '{':
			tk = open_curlybrace;
			c = f.get();
			break;
		case '}':
			tk = close_curlybrace;
			c = f.get();
			break;
		case ';':
			tk = semicolon;
			c = f.get();
			break;
		case ',':
			tk = comma;
			c = f.get();
			break;
		case '+':
			tk = add;
			c = f.get();
			break;
		case '-':
			tk = sub;
			c = f.get();
			break;
		case '*':
			tk = mul;
			c = f.get();
			break;
		case '.':
			tk = period;
			c = f.get();
			break;
		case '/':
			c = f.get();
			switch(c)
			{
				case '/':
					comment();
					break;
				default:
					tk = div;
			}
			break;
		case '#':
			comment();
			break;
		case '\n':
			++linenum;
			c = f.get();	
			next();
			break;
		case ' ':
		case '\t':
		case '\r':
			c = f.get();
			next();
			break;
		default:
			err();
	}
}

void LexAnalysis::Scanner::err()
{
	std::cerr << "unexpected character '" << c << "' in " << fname << ":" << linenum << std::endl;
	exit(1);
}

void LexAnalysis::Scanner::check_keywords()
{
	if (id == "main")
	{
		tk = main;
	}
	else if (id == "var")
	{
		tk = var;
	}
	else if (id == "array")
	{
		tk = array;
	}
	else if (id == "function" || id == "procedure")
	{
		tk = func;
	}
	else if (id == "call")
	{
		tk = call;
	}
	else if (id == "return")
	{
		tk = return_tk;
	}
	else if (id == "let")
	{
		tk = let;
	}
	else if (id == "if")
	{
		tk = if_tk;
	}
	else if (id == "then")
	{
		tk = then;
	}
	else if (id == "else")
	{
		tk = else_tk;
	}
	else if (id == "fi")
	{
		tk = fi;
	}
	else if (id == "while")
	{
		tk = while_tk;
	}
	else if (id == "do")
	{
		tk = do_tk;
	}
	else if (id == "od")
	{
		tk = od;
	}
	else
	{
		tk = id_tk;
	}
}

void LexAnalysis::Scanner::comment()
{
	while (c != '\n')
	{
		c = f.get();
	}
	next();
}

char const* LexAnalysis::tkToStr(Token tk)
{
	switch (tk)
	{
	case id_tk: return "identifier";
	case num_tk: return "number";
	case assign: return "<-";
	case e: return "==";
	case ne: return "!=";
	case lte: return "<=";
	case gte: return ">=";
	case main: return "main";
	case var: return "var";
	case array: return "array";
	case func: return "function";
	case call: return "call";
	case return_tk: return "return";
	case let: return "let";
	case if_tk: return "it";
	case then: return "then";
	case else_tk: return "else";
	case fi: return "fi";
	case while_tk: return "while";
	case do_tk: return "do";
	case od: return "od";
	case open_paren: return "(";
	case close_paren: return ")";
	case open_bracket: return "[";
	case close_bracket: return "]";
	case open_curlybrace: return "{";
	case close_curlybrace: return "}";
	case semicolon: return ";";
	case comma: return ",";
	case add: return "+";
	case sub: return "-";
	case mul: return "*";
	case div: return "/";
	case lt: return "<";
	case gt: return ">";
	case period: return ".";
	case eof: return "EOF";
	default: return std::to_string(tk).c_str();
	}
}

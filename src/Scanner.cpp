
#include "Scanner.h"

Scanner::Scanner(char const* s) : f(s), fname(s), linenum(1)
{
	if (!f.is_open())
	{
		std::cerr << "cannot open file " << s << std::endl;
		exit(1);
	}
	c = f.get();
}

void Scanner::next() 
{
	if (f.eof())
	{
		tk = eof;
		return;
	}
	// std::cout << "next called, c is " << int(c) << std::endl;
	switch(c) 
	{
		case 'a' ... 'z':
		case 'A' ... 'Z':
			id = "";
			while((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
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
			tk = add_tk;
			c = f.get();
			break;
		case '-':
			tk = sub_tk;
			c = f.get();
			break;
		case '*':
			tk = mul_tk;
			c = f.get();
			break;
		case '>':
			tk = gt;
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
					while (c != '\n')
					{
						c = f.get();
					}
					next();
					break;
				default:
					err();
				break;
			}
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

void Scanner::err()
{
	std::cerr << "unexpected token '" << c << "' in " << fname << ":" << linenum << std::endl;
	exit(1);
}

void Scanner::check_keywords()
{
	if (id == "main")
	{
		tk = main_tk;
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
		tk = func_tk;
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
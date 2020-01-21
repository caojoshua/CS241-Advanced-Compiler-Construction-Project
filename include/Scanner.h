/*
 * Scanner.h
 * Author: Joshua Cao
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <fstream>
#include <iostream>
#include <string>

namespace LexAnalysis
{

	enum Token {
		// multi-char tokens
		id_tk,
		num_tk,
		assign,
		e,
		ne,
		lte,
		gte,
		// keywords
		main,
		var,
		array,
		func,
		call,
		return_tk,
		let,
		if_tk,
		then,
		else_tk,
		fi,
		while_tk,
		do_tk,
		od,
		// single chars, define explicitly to ensure all tokens have unique values
		open_paren,
		close_paren,
		open_bracket,
		close_bracket,
		open_curlybrace,
		close_curlybrace,
		semicolon,
		comma,
		add,
		sub,
		mul,
		div,
		lt,
		gt,
		period,
		eof
	};

	class Scanner
	{
	private:
		std::ifstream f;
		char c;
		void err();
		void check_keywords();
		void comment();
	public:
		Token tk;
		std::string fname;
		std::string id;
		int num;
		int linenum;
		Scanner(char const* s);
		void next();
	};

	char const* tkToStr(Token tk);
}

#endif

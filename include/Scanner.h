#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <fstream>
#include <iostream>
#include <string>

enum Token {
	// multi-char tokens
	id_tk,
	num_tk,
	assign,
	lte,
	// keywords
	main_tk,
	var,
	array,
	func_tk,
	call,
	return_tk,
	let,
	if_tk,
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
	add_tk,
	sub_tk,
	mul_tk,
	lt,
	gt,
	period,
	eof
};

class Scanner
{
private:
	std::ifstream f;
	char const* fname;
	char c;
	std::string id;
	int num;
	int linenum;
	void err();
	void check_keywords();
public:
	Token tk;
	Scanner(char const* s);
	void next();
};

#endif
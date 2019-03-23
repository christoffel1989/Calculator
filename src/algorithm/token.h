#pragma once

//token类型
enum TokenType
{
	Number,
	End,
	Plus = '+',
	Minus = '-',
	Mul = '*',
	Div = '/', 
	Lp = '(',
	Rp = ')',
};

struct Token
{
	TokenType type;
	double value;
};
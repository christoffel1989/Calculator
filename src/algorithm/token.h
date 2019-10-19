#pragma once

#include <tuple>
#include <string>
#include <functional>

//token类型
enum TokenType
{
	Number,
	Symbol,
	End,
	Plus = '+',
	Minus = '-',
	Mul = '*',
	Div = '/',
	Lp = '(',
	Rp = ')',
	Assign = '=',
	BadType
};

struct Token
{
	TokenType type;
	//token为数值值时存储的变量
	double value;
	//token为字符串值时存储的变量
	std::string name;
};

//将新的变量注册到变量表中
bool registVariable(std::string variable, double value);

//获得特定名字的变量的值
//如果该名字的变量并不存在则返回值的第二个分量为false
//更好的写法是用std::optional
std::tuple<double, bool> getVaiableValue(std::string variable);

//将新的函数注册到函数表中
bool registFunction(std::string function, std::function<double(double)> impliment);

//获得特定名字的函数的实体
//如果该名字的函数并不存在则返回值的第二个分量为false
//更好的写法是用std::optional
std::tuple<std::function<double(double)>, bool> getFunctionImpliment(std::string variable);
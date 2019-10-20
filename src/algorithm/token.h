#pragma once

#include <string>
#include <variant>
#include <optional>
#include <functional>

//token类型
enum TokenType
{
	Number,
	//symbol的起始为下划线或者字母
	Symbol,
	End,
	Plus = '+',
	Minus = '-',
	Mul = '*',
	Div = '/',
	Pow = '^',
	Fact = '!',
	Lp = '(',
	Rp = ')',
	Assign = '=',
	BadType
};

//含有特定意义的词组
struct Token
{
	//类型
	TokenType type;
	//存储非符号类型Token的值的变量
	//std::variant类型 可能是double可能是std::string
	std::variant<double, std::string> value;
};

//将新的变量注册到变量表中
bool registVariable(std::string variable, double value);

//获得特定名字的变量的值
std::optional<double> getVaiableValue(std::string variable);

//将新的函数注册到函数表中
bool registFunction(std::string function, std::function<double(double)> impliment);

//获得特定名字的函数的实体
std::optional<std::function<double(double)>> getFunctionImpliment(std::string variable);
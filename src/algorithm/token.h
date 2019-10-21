#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

//token类型
enum TokenType
{
	Number,
	PrimitiveSymbol,
	//symbol的起始为下划线或者字母
	UserSymbol,
	Plus = '+',
	Minus = '-',
	Mul = '*',
	Div = '/',
	Pow = '^',
	Fact = '!',
	Lp = '(',
	Rp = ')',
	Comma = ',',
	//赋值
	Assign = '=',
	//定义变量
	DefVar,
	//定义函数
	DefProc,
	End,
	//错误类型
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

//词法解析
std::tuple<Token, std::string> parseToken(std::string input);

//用户自定义变量或函数
//是一个元组
//第一个分量是由输入变量字符串构成的vector
//第二个分量是本体
//当输入参量个数为0时 第二分量具体类型是double
//当输入参量个数大于1时 第二分量具体类型是string 存储了表达式
using UserType = std::tuple<std::vector<std::string>, std::variant<double, std::string>>;

//环境表存储了一些定义了的符号 以及 他的父环境指针
struct Environment
{
	//环境
	//每一个symbol对应
	std::map<std::string, UserType> EnvMap;

	//指向父类环境的智能指针
	Environment* parent = nullptr;
};

std::optional<std::variant<double, std::function<double(double)>>> getPrimitiveSymbol(std::string symbol);

//将新的变量或函数注册到解释器环境里
void setEnvSymbol(std::string symbol, UserType value, Environment* env);

//获得特定名字的函数的实体
std::optional<UserType> getEnvSymbol(std::string symbol, Environment* env);
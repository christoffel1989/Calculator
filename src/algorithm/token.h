#pragma once

#include <string>
#include <variant>
#include <optional>
#include <memory>
#include <map>
#include <functional>

//token类型
enum TokenType
{
	Number,
	//symbol的起始为下划线或者字母
	Symbol,
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

//定义环境中定义的symbol可能的类型
//0元函数(即变量)
using Arg0Fun = double;
//1元函数
using Arg1Fun = std::function<double(double)>;
//2元函数
using Arg2Fun = std::function<double(double, double)>;
//3元函数
using Arg3Fun = std::function<double(double, double, double)>;

//环境表存储了一些定义了的符号 以及 他的父环境指针
struct Environment
{
	//环境
	std::map<std::string, std::variant<Arg0Fun, Arg1Fun, Arg2Fun, Arg3Fun>> EnvMap;

	//指向父类环境的智能指针
	std::shared_ptr<Environment> parent = nullptr;
};

//初始化全局环境
void initGlobalEnvironment(std::shared_ptr<Environment> env);

//将新的变量或函数注册到解释器环境里
void setSymbol(std::string symbol, std::variant<Arg0Fun, Arg1Fun, Arg2Fun, Arg3Fun> value, std::shared_ptr<Environment> env);

//获得特定名字的函数的实体
std::optional<std::variant<Arg0Fun, Arg1Fun, Arg2Fun, Arg3Fun>> getSymbol(std::string symbol, std::shared_ptr<Environment> env);
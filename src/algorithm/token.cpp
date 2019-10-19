#include "token.h"

#include <cmath>
#include <map>

#include <stdexcept>

//初始化预设值的符号集合
bool tokenInit();

//变量表
static std::map<std::string, double> VariableMap;
//函数表
static std::map<std::string, std::function<double(double)>> FunctionMap;

//初始化
static bool init = tokenInit();

//初始化预设值的符号集合
bool tokenInit()
{
	//初始化默认变量
	registVariable("PI", atan(1.0) * 4);
	registVariable("e", 2.718281828459);

	//初始化默认函数(不存在异常)
	registFunction("sin", [](double val) { return sin(val); });
	registFunction("cos", [](double val) { return cos(val); });
	registFunction("tan", [](double val) { return tan(val); });
	registFunction("atan", [](double val) { return atan(val); });
	registFunction("exp", [](double val) { return exp(val); });
	//初始化默认函数(可能存在异常)
	registFunction("asin", [](double val) 
	{
		if (val < -1 || val > 1)
		{
			throw std::runtime_error("out of asin's domain!");
		}
		return asin(val); 
	});
	registFunction("acos", [](double val) 
	{
		if (val < -1 || val > 1)
		{
			throw std::runtime_error("out of acos's domain!");
		}
		return acos(val); 
	});
	registFunction("ln", [](double val)
	{
		if (val <= 0)
		{
			throw std::runtime_error("out of ln's domain!");
		}
		return log(val);
	});
	registFunction("log", [](double val)
	{
		if (val <= 0)
		{
			throw std::runtime_error("out of log's domain!");
		}
		return log10(val);
	});
	registFunction("sqrt", [](double val)
	{
		if (val < 0)
		{
			throw std::runtime_error("out of sqrt's domain!");
		}
		return sqrt(2);
	});
	registFunction("inv", [](double val)
	{
		if (val == 0)
		{
			throw std::runtime_error("can not inverse zero!");
		}
		return 1 / val;
	});

	return true;
}

//将新的变量注册到变量表中
bool registVariable(std::string variable, double value)
{
	//先判断函数表中是否存在这个变量
	if (FunctionMap.find(variable) == FunctionMap.end())
	{
		//同时兼顾了注册和更新
		VariableMap[variable] = value;
		return true;
	}
	return false;
}

//获得特定名字的变量的值
//如果该名字的变量并不存在则返回值的第二个分量为false
//更好的写法是用std::optional
std::tuple<double, bool> getVaiableValue(std::string variable)
{
	//查找
	auto iter = VariableMap.find(variable);
	//如果存在
	if (iter != VariableMap.end())
	{
		return std::make_tuple(iter->second, true);
	}
	return std::make_tuple(0, false);
}

//将新的函数注册到函数表中
bool registFunction(std::string function, std::function<double(double)> impliment)
{
	//先判断变量中是否存在这个变量
	if (VariableMap.find(function) == VariableMap.end())
	{
		//同时兼顾了注册和更新
		FunctionMap[function] = impliment;
		return true;
	}
	return false;
}

//获得特定名字的函数的实体
//如果该名字的函数并不存在则返回值的第二个分量为false
//更好的写法是用std::optional
std::tuple<std::function<double(double)>, bool> getFunctionImpliment(std::string variable)
{
	//查找
	auto iter = FunctionMap.find(variable);
	//如果存在
	if (iter != FunctionMap.end())
	{
		return std::make_tuple(iter->second, true);
	}
	//不存在时返回一个恒返回0的函数
	return std::make_tuple([](double) { return 0; }, false);
}
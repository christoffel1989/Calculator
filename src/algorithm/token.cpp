#include "token.h"

#include <cmath>

#include <stdexcept>

//初始化预设值的符号集合
bool tokenInit();
//从字符串中解析出第一个数值
std::tuple<double, std::string> parseNum(std::string input);
//从字符串中解析出第一个英文单词
std::tuple<std::string, std::string> parseSymbol(std::string input);

//定义变长参数模板求一组数的最大和最小值
//求最大(终止条件)
template <typename T>
T myMax(T val1, T val2)
{
	return (val1 > val2) ? val1 : val2;
}
//求最大(一般条件)
template <typename T, typename... Args>
T myMax(T val, Args... args)
{
	return myMax(val, myMax(args...));
}
//求最小(终止条件)
template <typename T>
T myMin(T val1, T val2)
{
	return (val1 < val2) ? val1 : val2;
}
//求最小(一般条件)
template <typename T, typename... Args>
T myMin(T val, Args... args)
{
	return myMin(val, myMin(args...));
}

//用折叠表达式求合
template <typename ... Ts>
auto mySum(Ts... ts)
{
	return (... + ts);
}

//初始化全局环境
void initGlobalEnvironment(std::shared_ptr<Environment> env)
{
	//////////////////初始化默认0元函数(即变量)////////////////////////
	setSymbol("PI", atan(1.0) * 4, env);
	setSymbol("e", exp(1.0), env);

	/////////////////////初始化默认1元函数////////////////////////////
	//不存在异常的函数
	setSymbol("sin", [](double val) { return sin(val); }, env);
	setSymbol("cos", [](double val) { return cos(val); }, env);
	setSymbol("tan", [](double val) { return tan(val); }, env);
	setSymbol("atan", [](double val) { return atan(val); }, env);
	setSymbol("exp", [](double val) { return exp(val); }, env);
	//可能存在异常
	setSymbol("asin", [](double val)
	{
		if (val < -1 || val > 1)
		{
			throw std::runtime_error("error(function call): out of asin's domain!\n");
		}
		return asin(val);
	}, env);
	setSymbol("acos", [](double val)
	{
		if (val < -1 || val > 1)
		{
			throw std::runtime_error("error(function call): out of acos's domain!\n");
		}
		return acos(val);
	}, env);
	setSymbol("ln", [](double val)
	{
		if (val <= 0)
		{
			throw std::runtime_error("error(function call): out of ln's domain!\n");
		}
		return log(val);
	}, env);
	setSymbol("log", [](double val)
	{
		if (val <= 0)
		{
			throw std::runtime_error("error(function call): out of log's domain!\n");
		}
		return log10(val);
	}, env);
	setSymbol("sqrt", [](double val)
	{
		if (val < 0)
		{
			throw std::runtime_error("error(function call): out of sqrt's domain!\n");
		}
		return sqrt(val);
	}, env);
	setSymbol("inv", [](double val)
	{
		if (val == 0)
		{
			throw std::runtime_error("error(function call): can not inverse zero!\n");
		}
		return 1 / val;
	}, env);

	/////////////////////初始化默认2元函数////////////////////////////
	setSymbol("max2", [](double val1, double val2) { return myMax(val1, val2); }, env);
	setSymbol("min2", [](double val1, double val2) { return myMin(val1, val2); }, env);
	setSymbol("sum2", [](double val1, double val2) { return mySum(val1, val2); }, env);

	/////////////////////初始化默认3元函数////////////////////////////
	setSymbol("max3", [](double val1, double val2, double val3) { return myMax(val1, val2, val3); }, env);
	setSymbol("min3", [](double val1, double val2, double val3) { return myMin(val1, val2, val3); }, env);
	setSymbol("sum3", [](double val1, double val2, double val3) { return mySum(val1, val2, val3); }, env);
}

std::tuple<double, std::string> parseNum(std::string input)
{

	// input "12.34a" numstr ""
	// ch 1 "2.34a" numstr "1"
	// ch 2 ".34a" numstr "12"
	// ch . "34a" numstr "12."
	char ch;
	std::string numstr;
	bool firstDot = true;

	while (1)
	{
		//是不是空
		if (input.empty())
		{
			break;
		}
		ch = *input.begin();
		if ((ch >= '0' && ch <= '9') || (ch == '.' && firstDot))
		{
			numstr.push_back(ch);
			input.erase(input.begin());
			if (ch == '.')
			{
				firstDot = false;
			}
		}
		//遇到了非数字或者是第二次遇到小数点的情况
		else
		{
			break;
		}
	}

	return { std::stod(numstr), input };
}

std::tuple<std::string, std::string> parseSymbol(std::string input)
{
	//根据调用关系 能确保这个程序的输入一定是字母或者下划线
	char ch;
	std::string name;

	while (1)
	{
		//是不是空
		if (input.empty())
		{
			break;
		}
		ch = *input.begin();
		//'Z'和'a'之间还有别的字符
		if (ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z' || ch >= '0' && ch <= '9' || ch == '_')
		{
			name.push_back(ch);
			input.erase(input.begin());
		}
		//遇到了非数字或者是第二次遇到小数点的情况
		else
		{
			break;
		}
	}

	return { name, input };
}

std::tuple<Token, std::string> parseToken(std::string input)
{
	char ch;
	Token tk;
	tk.type = TokenType::End;

	//把空格去掉
	do
	{
		//先判断一下是否是空
		if (input.empty())
		{
			tk.type = TokenType::End;
			return { tk, "" };
		}
		ch = *input.begin();
		input.erase(input.begin());
	} while (ch == ' ');

	switch (ch)
	{
	case '+':
	case '-':
	case '*':
	case '/':
	case '^':
	case '!':
	case '(':
	case ')':
	case ',':
	case '=':
		tk.type = TokenType(ch);
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		//刚才吃掉的数字补回去
		input.insert(input.begin(), ch);
		std::tie(tk.value, input) = parseNum(input);
		tk.type = TokenType::Number;
		break;
	default:
		//字母或者下划线的情况
		if (isalpha(ch) || ch == '_')
		{
			//刚才吃掉的字母补回去
			input.insert(input.begin(), ch);
			//解析出名字
			std::tie(tk.value, input) = parseSymbol(input);
			//如果是系统内部定义的关键字
			if (std::get<std::string>(tk.value) == "var")
			{
				tk.type = TokenType::DefVar;
			}
			else if (std::get<std::string>(tk.value) == "proc")
			{
				tk.type = TokenType::DefProc;
			}
			else
			{
				tk.type = TokenType::Symbol;
			}
		}
		else
		{
			tk.type = TokenType::BadType;
		}
		break;
	}

	return { tk, input };
}

void setSymbol(std::string symbol, std::variant<Arg0Fun, Arg1Fun, Arg2Fun, Arg3Fun> value, std::shared_ptr<Environment> env)
{
	/*//查找
	auto iter = env->EnvMap.find(symbol);
	//如果存在
	if (iter != env->EnvMap.end())
	{
		//更新
		env->EnvMap[symbol] = value;
	}
	//如果不存在 则有两种可能
	else
	{
		//情况1 父环境中存在这个符号
		for (auto parent = env->parent; parent != nullptr; parent = parent->parent)
		{
			if (parent->EnvMap.find(symbol) != parent->EnvMap.end())
			{
				setSymbol(symbol, value, parent);
				return;
			}
		}
		//看看父类中
	}*/

	//先不搞太复杂的
	//就更新在当前环境 不考虑父环境
	//更新
	env->EnvMap[symbol] = value;
}

//获得特定名字的函数的实体
std::optional<std::variant<Arg0Fun, Arg1Fun, Arg2Fun, Arg3Fun>> getSymbol(std::string symbol, std::shared_ptr<Environment> env)
{
	//查找
	auto iter = env->EnvMap.find(symbol);
	//如果存在
	if (iter != env->EnvMap.end())
	{
		return { iter->second };
	}
	//如果存在父环境 则继续查询父环境
	else if (env->parent)
	{
		return getSymbol(symbol, env->parent);
	}
	else
	{
		//不存在时返回无效值
		return {};
	}
}
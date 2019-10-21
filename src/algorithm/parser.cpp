#include "parser.h"

#include <stdexcept>

#include <list>

//解析因子
std::tuple<double, std::string> parseFactor(std::string input, std::shared_ptr<Environment> env);
//解析项
std::tuple<double, std::string> parseTerm(std::string input, std::shared_ptr<Environment> env);

//解析因子
std::tuple<double, std::string> parseFactor(std::string input, std::shared_ptr<Environment> env)
{
	using std::tie;
	double result;
	std::string symbol;
	Token tk;
	tie(tk, input) = parseToken(input);

	switch (tk.type)
	{
	//数字
	case TokenType::Number:
		result = std::get<double>(tk.value);
		break;
	//负号
	case TokenType::Minus:
		//再解析一个Factor
		std::tie(result, input) = parseFactor(input, env);
		//解析出的结果前面添加一个负号
		result = -result;
		break;
	//左括号
	case TokenType::Lp:
		//括号中的表达式
		tie(result, input) = parseExpression(input, env);
		//解析右括号
		tie(tk, input) = parseToken(input);
		if (tk.type != TokenType::Rp)
		{
			//报一个错误
			throw std::runtime_error("error(bad syntax): miss a )!\n");
		}
		break;
	//自定义Symbo
	case TokenType::Symbol:
		symbol = std::get<std::string>(tk.value);
		//symbol已在环境当中定义
		if (auto val = getSymbol(symbol, env))
		{
			//如果是变量
			if (std::holds_alternative<Arg0Fun>(val.value()))
			{
				result = std::get<Arg0Fun>(val.value());
			}
			//如果是1元函数
			else if (std::holds_alternative<Arg1Fun>(val.value()))
			{
				//解析左括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Lp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a (!\n");
				}
				//解析括号中的函数输入参量
				double para1;
				tie(para1, input) = parseExpression(input, env);
				//解析右括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Rp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a )!\n");
				}
				//计算函数值
				result = std::get<Arg1Fun>(val.value())(para1);
			}
			//如果是2元函数
			else if (std::holds_alternative<Arg2Fun>(val.value()))
			{
				//解析左括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Lp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a (!\n");
				}
				//解析括号中的函数输入参量
				double para1, para2;
				//解析第1个输入参量
				tie(para1, input) = parseExpression(input, env);
				//解析逗号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Comma)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a ,!\n");
				}
				//解析第2个输入参量
				tie(para2, input) = parseExpression(input, env);
				//解析右括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Rp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a )!\n");
				}
				//计算函数值
				result = std::get<Arg2Fun>(val.value())(para1, para2);
			}
			//如果是3元函数
			else if (std::holds_alternative<Arg3Fun>(val.value()))
			{
				//解析左括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Lp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a (!\n");
				}
				//解析括号中的函数输入参量
				double para1, para2, para3;
				//解析第1个输入参量
				tie(para1, input) = parseExpression(input, env);
				//解析逗号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Comma)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a ,!\n");
				}
				//解析第2个输入参量
				tie(para2, input) = parseExpression(input, env);
				//解析逗号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Comma)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a ,!\n");
				}
				//解析第3个输入参量
				tie(para3, input) = parseExpression(input, env);
				//解析右括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Rp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): miss a )!\n");
				}
				//计算函数值
				result = std::get<Arg3Fun>(val.value())(para1, para2, para3);
			}
		}
		//symbol并在环境当中未定义
		else
		{
			//不是赋值符号 抛出一个未定义类型的错误
			throw std::runtime_error("error(bad syntax): undefined symbol " + symbol + "!\n");
		}
		break;
	default:
		//报一个错误
		throw std::runtime_error("error(bad syntax): need a number or a (!\n");
		break;
	}

	//如果算完后法线后面还跟了一个!号则进行进一步的阶乘
	std::string res;
	tie(tk, res) = parseToken(input);
	if (tk.type == TokenType::Fact)
	{
		//伽马函数是阶乘的推广
		result = tgamma(result + 1);
		input = res;
	}

	return { result, input };
}

//解析项
std::tuple<double, std::string> parseTerm(std::string input, std::shared_ptr<Environment> env)
{
	using std::tie;
	double result, factor;
	tie(result, input) = parseFactor(input, env);

	bool onloop = true;
	Token op;
	std::string str;
	while (onloop)
	{
		tie(op, str) = parseToken(input);
		switch (op.type)
		{
		case TokenType::Mul:
			tie(factor, input) = parseFactor(str, env);
			result *= factor;
			break;
		case TokenType::Div:
			tie(factor, input) = parseFactor(str, env);
			if (factor == 0)
			{
				throw std::runtime_error("error(arithmatic): divided by zero!\n");
			}
			result /= factor;
			break;
		case TokenType::Pow:
			tie(factor, input) = parseFactor(str, env);
			//当底为0 幂为非正实数时幂操作无效
			if (result == 0 && factor <= 0)
			{
				throw std::runtime_error("error(arithmatic): zero can not be power by non-positive value!\n");
			}
			result = pow(result, factor);
			break;
		default:
			onloop = false;
			break;
		}
	}

	return { result, input };
}

//解析表达式
std::tuple<double, std::string> parseExpression(std::string input, std::shared_ptr<Environment> env)
{
	using std::tie;
	double result, term;
	tie(result, input) = parseTerm(input, env);

	bool onloop = true;
	Token op;
	std::string str;
	while (onloop)
	{
		tie(op, str) = parseToken(input);
		switch (op.type)
		{
		case TokenType::Plus:
			tie(term, input) = parseTerm(str, env);
			result += term;
			break;
		case TokenType::Minus:
			tie(term, input) = parseTerm(str, env);
			result -= term;
			break;
		default:
			onloop = false;
			break;
		}
	}

	return { result, input };
}

//解析语句
std::tuple<double, std::string> parseStatement(std::string input, std::shared_ptr<Environment> env)
{
	double result;
	//先看第一个字符是什么
	auto[tk, res] = parseToken(input);
	//变量定义
	if (tk.type == TokenType::DefVar)
	{
		//读取变量名
		std::tie(tk, input) = parseToken(res);
		//如果类型不是Symbo则报错
		if (tk.type != TokenType::Symbol)
		{
			throw std::runtime_error("error(define var): need a symbo for var name!\n");
		}
		auto symbol = std::get<std::string>(tk.value);
		//读取赋值符号
		std::tie(tk, input) = parseToken(input);
		//如果类型不是=号则报错
		if (tk.type != TokenType::Assign)
		{
			throw std::runtime_error("error(define var): need a =!\n");
		}
		//读取表达式
		std::tie(result, input) = parseExpression(input, env);
		//变量值更新
		setSymbol(symbol, result, env);
	}
	//函数定义
	else if (tk.type == TokenType::DefProc)
	{
		//读取函数名
		std::tie(tk, input) = parseToken(res);
		//如果类型不是Symbo则报错
		if (tk.type != TokenType::Symbol)
		{
			throw std::runtime_error("error(define proc): need a symbo for proc name!\n");
		}
		auto proc = std::get<std::string>(tk.value);
		//读取输入参量(目前假定只有一个输入参量)
		std::tie(tk, input) = parseToken(input);
		//如果类型不是Symbo则报错
		if (tk.type != TokenType::Symbol)
		{
			throw std::runtime_error("error(define proc): need a symbo for proc para name!\n");
		}
		auto para = std::get<std::string>(tk.value);
		//读取赋值符号
		std::tie(tk, input) = parseToken(input);
		//如果类型不是=号则报错
		if (tk.type != TokenType::Assign)
		{
			throw std::runtime_error("error(define var): need a =!\n");
		}
		//剩下的是根据表达式构建函数添加到env中
		//首先 创建一个子环境 其父类是env
		std::shared_ptr<Environment> subenv(new Environment);
		subenv->parent = env;
		//方法是定义一个lambda
		auto fun = [subenv, para, input](double val)
		{
			//将para用值val注册到
			setSymbol(para, val, subenv);
			//在这个环境下求值
			auto[v, res] = parseExpression(input, subenv);
			return v;
		};
		//将proc注册为这个一元lambda
		setSymbol(proc, fun, env);
		result = 0;
		//定义函数不需要显示数值
		input = ";";
	}
	//自定义符号
	else if (tk.type == TokenType::Symbol)
	{
		auto symbol = std::get<std::string>(tk.value);
		//如果已经定义了
		if (auto val = getSymbol(symbol, env))
		{
			//读取赋值符号
			std::tie(tk, res) = parseToken(res);
			//如果类型是=号求解等号右边的表达式值
			if (tk.type == TokenType::Assign)
			{
				//读取表达式
				std::tie(result, input) = parseExpression(res, env);
				//变量值更新
				setSymbol(symbol, result, env);
			}
			//如果不是等号则把连读出的符号一起包括的当作一个表达式解析
			else
			{
				//还有可能是计算表达式的值
				std::tie(result, input) = parseExpression(input, env);
			}
		}
		else
		{
			throw std::runtime_error("error(bad syntax): undefined symbol " + symbol + "!\n");
		}
	}
	//表达式计算
	else
	{
		//读取表达式
		std::tie(result, input) = parseExpression(input, env);
	}

	return { result, input };
}
#include "parser.h"

#include <stdexcept>

#include <iostream>

//解析因子
std::tuple<double, std::string> parseFactor(std::string input, Environment* env);
//解析项
std::tuple<double, std::string> parseTerm(std::string input, Environment* env);

//解析因子
std::tuple<double, std::string> parseFactor(std::string input, Environment*env)
{
	using std::tie;
	double result;
	std::string symbol;
	std::variant<double, std::function<double(double)>> primitive;
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
	//Primitive Symbol
	case TokenType::PrimitiveSymbol:
		symbol = std::get<std::string>(tk.value);
		primitive = getPrimitiveSymbol(symbol).value();
		//如果是变量
		if (std::holds_alternative<double>(primitive))
		{
			result = std::get<double>(primitive);
		}
		//如果是1元函数
		else if (std::holds_alternative<std::function<double(double)>>(primitive))
		{
			//解析左括号
			tie(tk, input) = parseToken(input);
			if (tk.type != TokenType::Lp)
			{
				//报一个错误
				throw std::runtime_error("error(bad syntax): function call miss a (!\n");
			}
			//解析括号中的函数输入参量
			double para1;
			tie(para1, input) = parseExpression(input, env);
			//解析右括号
			tie(tk, input) = parseToken(input);
			if (tk.type != TokenType::Rp)
			{
				//报一个错误
				throw std::runtime_error("error(bad syntax): not enough arguments for function call or function call miss a )!\n");
			}
			//计算函数值
			result = std::get<std::function<double(double)>>(primitive)(para1);
		}
		break;
	//自定义Symbol
	case TokenType::UserSymbol:
		symbol = std::get<std::string>(tk.value);
		//symbol已在环境当中定义
		if (auto val = getEnvSymbol(symbol, env))
		{
			//解包参数
			auto[paras, body] = val.value();
			//如果body是值类型则说明为变量
			if (std::holds_alternative<double>(body))
			{
				result = std::get<double>(body);
			}
			//函数类型
			else
			{
				//获得参数个数
				auto NPara = paras.size();
				//构造一个调用函数新的环境
				Environment subenv;
				//他的父亲时env
				subenv.parent = env;
				//解析左括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Lp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): function call miss a (!\n");
				}
				//解析各个参数
				for (int i = 0; i < NPara; i++)
				{
					//解析括号中的函数输入参量
					double val;
					tie(val, input) = parseExpression(input, env);
					//在subenv中设置第i个para的值为val
					setEnvSymbol(paras[i], { {}, val }, env);
					//不是最后一个参量则再解析一个逗号
					if (i != NPara - 1)
					{
						tie(tk, input) = parseToken(input);
						if (tk.type != TokenType::Comma)
						{
							//报一个错误
							throw std::runtime_error("error(bad syntax): not enough arguments for function call!\n");
						}
					}
				}
				//解析右括号
				tie(tk, input) = parseToken(input);
				if (tk.type != TokenType::Rp)
				{
					//报一个错误
					throw std::runtime_error("error(bad syntax): not enough arguments for function call or function call miss a )!\n");
				}

				//执行函数
				std::string res;
				tie(result, res) = parseExpression(std::get<std::string>(body), env);
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
std::tuple<double, std::string> parseTerm(std::string input, Environment* env)
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
std::tuple<double, std::string> parseExpression(std::string input, Environment* env)
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

	//如果计算出来的数特别小就看作是0
	if (abs(result) < 1e-10)
	{
		result = 0;
	}

	return { result, input };
}

//解析语句
void parseStatement(std::string input, Environment* env)
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
		if (tk.type != TokenType::UserSymbol)
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
		setEnvSymbol(symbol, { {}, result }, env);

		//结果打印输出
		if (input == "")
		{
			std::cout << symbol << " = " << result << std::endl;
		}
		else if (input != ";")
		{
			std::cout << "error(define var): unexpect symbol " + input + "!" << std::endl;
		}
	}
	//函数定义
	else if (tk.type == TokenType::DefProc)
	{
		//如果最后一个英文字母不是;则报错
		if (*input.rbegin() != ';')
		{
			throw std::runtime_error("error(define proc): miss ; at the end!\n");
		}

		//读取函数名
		std::tie(tk, input) = parseToken(res);
		//如果类型不是Symbo则报错
		if (tk.type != TokenType::UserSymbol)
		{
			throw std::runtime_error("error(define proc): need a symbo for proc name!\n");
		}
		auto proc = std::get<std::string>(tk.value);

		//读取左括号
		tie(tk, input) = parseToken(input);
		if (tk.type != TokenType::Lp)
		{
			//报一个错误
			throw std::runtime_error("error(define proc): miss a (!\n");
		}

		//读取函数输入形式参量
		std::vector<std::string> paras;
		std::string res;
		std::tie(tk, res) = parseToken(input);

		//0输入参量
		if (tk.type == TokenType::Rp)
		{
			input = res;
		}
		//大于等1个输入参量
		else
		{
			do
			{
				//读取参量
				std::tie(tk, input) = parseToken(input);

				//不是用户自定义变量则报错
				if (tk.type != TokenType::UserSymbol)
				{
					throw std::runtime_error("error(define proc): need a symbo for proc para name!\n");
				}

				//注册信息
				paras.push_back(std::get<std::string>(tk.value));

				//再读取一个token
				std::tie(tk, input) = parseToken(input);
			} while (tk.type == TokenType::Comma);

			//判断停止while后的符号是不是右括号 如果不是则报错
			if (tk.type != TokenType::Rp)
			{
				throw std::runtime_error("error(define var): miss a )!\n");
			}
		}

		//读取赋值符号
		tie(tk, input) = parseToken(input);
		//如果类型不是=号则报错
		if (tk.type != TokenType::Assign)
		{
			throw std::runtime_error("error(define var): need a =!\n");
		}

		//剩下的部分为函数的本体
		//注册函数到当前env中
		setEnvSymbol(proc, { paras, input }, env);
	}
	//自定义符号
	else if (tk.type == TokenType::UserSymbol)
	{
		auto symbol = std::get<std::string>(tk.value);
		//如果已经定义了
		if (auto val = getEnvSymbol(symbol, env))
		{
			//读取赋值符号
			std::tie(tk, res) = parseToken(res);
			//如果类型是=号求解等号右边的表达式值
			if (tk.type == TokenType::Assign)
			{
				//读取表达式
				std::tie(result, input) = parseExpression(res, env);
				//变量值更新
				setEnvSymbol(symbol, { {}, result }, env);

				//结果打印输出
				if (input == "")
				{
					std::cout << symbol << " = " << result << std::endl;
				}
				else if (input != ";")
				{
					std::cout << "error(assignment): unexpect symbol " + input + "!" << std::endl;
				}
			}
			//如果不是等号则把连读出的符号一起包括的当作一个表达式解析
			else
			{
				//还有可能是计算表达式的值
				std::tie(result, input) = parseExpression(input, env);

				//结果打印输出
				if (input == "")
				{
					std::cout << "ans = " << result << std::endl;
				}
				else if (input != ";")
				{
					std::cout << "error(arithmetic): unexpect symbol " + input + "!" << std::endl;
				}
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

		//结果打印输出
		if (input == "")
		{
			std::cout << "ans = " << result << std::endl;
		}
		else if (input != ";")
		{
			std::cout << "error(arithmetic): unexpect symbol " + input + "!" << std::endl;
		}
	}
}
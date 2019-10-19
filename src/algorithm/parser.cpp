#include "parser.h"

#include <stdexcept>

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

	return std::make_tuple(std::stod(numstr), input);
}

std::tuple<std::string, std::string> parseSymbol(std::string input)
{
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
		if (ch >= 'A' && ch <= 'z')
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

	return std::make_tuple(name, input);
}

std::tuple<Token, std::string> parseToken(std::string input)
{
	char ch;
	Token result;
	result.type = TokenType::End;

	//把空格去掉
	do
	{
		//先判断一下是否是空
		if (input.empty())
		{
			result.type = TokenType::End;
			return std::make_tuple(result, "");
		}
		ch = *input.begin();
		input.erase(input.begin());
	} 
	while (ch == ' ');

	switch (ch)
	{
	case '+':
	case '-':
	case '*':
	case '/':
	case '(':
	case ')':
	case '=':
		result.type = TokenType(ch);
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
		result.type = TokenType::Number;
		std::tie(result.value, input) = parseNum(input);
		break;
	default:
		//字母的情况
		if (isalpha(ch))
		{
			//刚才吃掉的字母补回去
			input.insert(input.begin(), ch);
			//解析出名字
			std::tie(result.name, input) = parseSymbol(input);
			result.type = TokenType::Symbol;
		}
		else
		{
			result.type = TokenType::BadType;
		}
		break;
	}

	return std::make_tuple(result, input);
}

//解析因子
std::tuple<double, std::string> parseFactor(std::string input)
{
	using std::tie;
	double result;
	std::function<double(double)> fun;
	bool regist;
	Token tk;
	tie(tk, input) = parseToken(input);

	switch (tk.type)
	{
	//负号
	case TokenType::Minus:
		//继续向下解析
		tie(tk, input) = parseToken(input);
		//如果是一个数字
		if (tk.type == TokenType::Number)
		{
			result = -tk.value;
			break;
		}
		//如果是一个Symbo
		if (tk.type == TokenType::Symbol)
		{
			//是否在变量表中 如果确实在则break;
			tie(result, regist) = getVaiableValue(tk.name);
			if (regist)
			{
				result = -result;
				break;
			}
		}
		//抛出异常
		throw std::runtime_error("unxepect -!\n");
		break;
	//数字
	case TokenType::Number:
		result = tk.value;
		break;
	//左括号
	case TokenType::Lp:
		//括号中的表达式
		tie(result, input) = parseExpression(input);
		//解析右括号
		tie(tk, input) = parseToken(input);
		if (tk.type != TokenType::Rp)
		{
			//报一个错误
			throw std::runtime_error("bad syntax: miss a )!\n");
		}
		break;
	//英文符号
	case TokenType::Symbol:
		//是否在变量表中 如果确实在则break;
		tie(result,regist) = getVaiableValue(tk.name);
		if (regist)
		{
			//如果后面跟了一个赋值的操作 还需要做进一步的处理
			std::string name = tk.name;
			std::string res;
			tie(tk, res) = parseToken(input);
			if (tk.type == TokenType::Assign)
			{
				//获得的赋予的值的大小
				tie(result, input) = parseExpression(res);
				//更新该值
				registVariable(name, result);
			}
			break;
		}
		//是否在函数表中 如果确实在则break;
		tie(fun, regist) = getFunctionImpliment(tk.name);
		if (regist)
		{
			//解析左括号
			tie(tk, input) = parseToken(input);
			if (tk.type != TokenType::Lp)
			{
				//报一个错误
				throw std::runtime_error("bad syntax: miss a (!\n");
			}
			//解析括号中的表达式
			//括号中的表达式
			tie(result, input) = parseExpression(input);
			//解析右括号
			tie(tk, input) = parseToken(input);
			if (tk.type != TokenType::Rp)
			{
				//报一个错误
				throw std::runtime_error("bad syntax: miss a )!\n");
			}
			//计算函数值
			result = fun(result);
			break;
		}
		{
			//如果Symbo并未定义 则检查下一个符号是否是=号，如果是则注册新的变量为=号后面的值
			std::string name = tk.name;
			std::string res;
			tie(tk, res) = parseToken(input);
			if (tk.type == TokenType::Assign)
			{
				//获得的赋予的值的大小
				tie(result, input) = parseExpression(res);
				//注册该新变量
				registVariable(name, result);
				break;
			}
			//不是赋值符号 抛出一个未定义类型的错误
			throw std::runtime_error(name + " is an undefine variable or function!\n");
		}
		break;
	default:
		//报一个错误
		throw std::runtime_error("bad syntax: need a number or a (!\n");
		break;
	}

	return std::make_tuple(result, input);
}

//解析项
std::tuple<double, std::string> parseTerm(std::string input)
{
	using std::tie;
	double result, factor;
	tie(result, input) = parseFactor(input);

	bool onloop = true;
	Token op;
	std::string str;
	while (onloop)
	{
		tie(op, str) = parseToken(input);
		switch (op.type)
		{
		case TokenType::Mul:
			tie(factor, input) = parseFactor(str);
			result *= factor;
			break;
		case TokenType::Div:
			tie(factor, input) = parseFactor(str);
			if (factor == 0)
			{
				throw std::runtime_error("error: divided by zero!\n");
			}
			result /= factor;
			break;
		default:
			onloop = false;
			break;
		}
	}

	return std::make_tuple(result, input);
}

//解析表达式
std::tuple<double, std::string> parseExpression(std::string input)
{
	using std::tie;
	double result, term;
	tie(result, input) = parseTerm(input);

	bool onloop = true;
	Token op;
	std::string str;
	while (onloop)
	{
		tie(op, str) = parseToken(input);
		switch (op.type)
		{
		case TokenType::Plus:
			tie(term, input) = parseTerm(str);
			result += term;
			break;
		case TokenType::Minus:
			tie(term, input) = parseTerm(str);
			result -= term;
			break;
		default:
			onloop = false;
			break;
		}
	}

	return std::make_tuple(result, input);
}
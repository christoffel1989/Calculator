#include "parser.h"

#include <stdexcept>

std::tuple<double, std::string> parseNum(std::string input)
{

	// input "12.34a" numstr ""
	// ch 1 "2.34a" numstr "1"
	// ch 2 ".34.a" numstr "12"
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

	return std::make_tuple(atof(numstr.c_str()), input);
}

std::tuple<Token, std::string> parseToken(std::string input)
{
	char ch;
	std::tuple<double, std::string> tempNum;
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
		break;
	}

	return std::make_tuple(result, input);
}

//解析因子
std::tuple<double, std::string> parseFactor(std::string input)
{
	using std::tie;
	double result;
	Token tk;
	tie(tk, input) = parseToken(input);

	switch (tk.type)
	{
	case TokenType::Number:
		result = tk.value;
		break;
	case TokenType::Lp:
		//括号中的表达式
		tie(result, input) = parseExpression(input);
		//解析右括号
		tie(tk, input) = parseToken(input);
		if (tk.type != TokenType::Rp)
		{
			//报一个错误
			throw std::runtime_error("bad syntax: miss a ) !");
		}
		break;
	default:
		//报一个错误
		throw std::runtime_error("bad syntax: need a number or a (");
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
	while (onloop)
	{
		auto [op, str] = parseToken(input);
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
				throw std::runtime_error("error: divided by zero!");
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
	while (onloop)
	{
		auto [op, str] = parseToken(input);
		switch (op.type)
		{
		case TokenType::Plus:
			tie(term, input) = parseTerm(str);
			result += term;
			break;
		case TokenType::Minus:
			tie(term, input) = parseTerm(str);
			result += term;
			break;
		default:
			onloop = false;
			break;
		}
	}

	return std::make_tuple(result, input);
}
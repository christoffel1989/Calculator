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
		tempNum = parseNum(input);
		result.value = std::get<0>(tempNum);
		input = std::get<1>(tempNum);
		break;
	default:
		break;
	}

	return std::make_tuple(result, input);
}

//解析因子
std::tuple<double, std::string> parseFactor(std::string input)
{
	std::tuple<double, std::string> exp;
	auto tk = parseToken(input);
	Token value = std::get<0>(tk);
	double result = 0;

	switch (value.type)
	{
	case TokenType::Number:
		result = value.value;
		input = std::get<1>(tk);
		break;
	case TokenType::Lp:
		input = std::get<1>(tk);
		exp = parseExpression(input);
		result = std::get<0>(exp);
		input = std::get<1>(exp);
		//解析右括号
		tk = parseToken(input);
		if (std::get<0>(tk).type != TokenType::Rp)
		{
			//报一个错误
			throw std::runtime_error("bad syntax: miss a ) !");
		}
		input = std::get<1>(exp);
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
	//用C++17重写后
	auto [result, str1] = parseFactor(input);

	while (1)
	{
		auto [op, str2] = parseToken(str1);
		if (op.type == TokenType::Mul)
		{
			auto[factor, str3] = parseFactor(str2);
			result *= factor;
			str1 = str3;
		}
		else if (op.type == TokenType::Div)
		{
			auto[factor, str3] = parseFactor(str2);
			if (factor == 0)
			{
				throw std::runtime_error("error: divided by zero!");
			}
			result /= factor;
			str1 = str3;
		}
		else
		{
			break;
		}
	}

	return std::make_tuple(result, str1);
}

//解析表达式
std::tuple<double, std::string> parseExpression(std::string input)
{
	auto term = parseTerm(input);
	double result = std::get<0>(term);
	input = std::get<1>(term);

	bool onloop = true;

	while (onloop)
	{
		auto tk = parseToken(input);
		Token op = std::get<0>(tk);

		switch (op.type)
		{
		case TokenType::Plus:
			input = std::get<1>(tk);
			term = parseTerm(input);
			result += std::get<0>(term);
			input = std::get<1>(term);
			break;
		case TokenType::Minus:
			input = std::get<1>(tk);
			term = parseTerm(input);
			result -= std::get<0>(term);
			input = std::get<1>(term);
			break;
		default:
			onloop = false;
			break;
		}
	}

	return std::make_tuple(result, input);
}
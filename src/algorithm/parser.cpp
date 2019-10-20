#include "parser.h"

#include <stdexcept>

//从字符串中解析出第一个数值
std::tuple<double, std::string> parseNum(std::string input);
//从字符串中解析出第一个英文单词
std::tuple<std::string, std::string> parseSymbol(std::string input);
//词法解析
std::tuple<Token, std::string> parseToken(std::string input);
//解析因子
std::tuple<double, std::string> parseFactor(std::string input);
//解析项
std::tuple<double, std::string> parseTerm(std::string input);


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
	} 
	while (ch == ' ');

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
			tk.type = TokenType::Symbol;
		}
		else
		{
			tk.type = TokenType::BadType;
		}
		break;
	}

	return { tk, input };
}

//解析因子
std::tuple<double, std::string> parseFactor(std::string input)
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
		std::tie(result, input) = parseFactor(input);
		//解析出的结果前面添加一个负号
		result = -result;
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
			throw std::runtime_error("error(bad syntax): miss a )!\n");
		}
		break;
	//英文符号
	case TokenType::Symbol:
		symbol = std::get<std::string>(tk.value);
		//是否在变量表中 如果确实在则break;
		if (auto val = getVaiableValue(symbol))
		{
			result = val.value();
			//如果后面跟了一个赋值的操作 还需要做进一步的处理
			std::string res;
			tie(tk, res) = parseToken(input);
			if (tk.type == TokenType::Assign)
			{
				//获得的赋予的值的大小
				tie(result, input) = parseExpression(res);
				//更新该值
				registVariable(symbol, result);
			}
		}
		//是否在函数表中 如果确实在则break;
		else if (auto fun = getFunctionImpliment(symbol))
		{
			//解析左括号
			tie(tk, input) = parseToken(input);
			if (tk.type != TokenType::Lp)
			{
				//报一个错误
				throw std::runtime_error("error(bad syntax): miss a (!\n");
			}
			//解析括号中的表达式
			//括号中的表达式
			tie(result, input) = parseExpression(input);
			//解析右括号
			tie(tk, input) = parseToken(input);
			if (tk.type != TokenType::Rp)
			{
				//报一个错误
				throw std::runtime_error("error(bad syntax): miss a )!\n");
			}
			//计算函数值
			result = fun.value()(result);
		}
		//Symbol并未定义 则
		else
		{
			//检查下一个符号是否是=号
			std::string res;
			tie(tk, res) = parseToken(input);
			//如果是则注册新的变量为 = 号后面的值
			if (tk.type == TokenType::Assign)
			{
				//获得的赋予的值的大小
				tie(result, input) = parseExpression(res);
				//注册该新变量
				registVariable(symbol, result);
			}
			else
			{
				//不是赋值符号 抛出一个未定义类型的错误
				throw std::runtime_error("error(bad syntax): undefined symbol " + symbol + "!\n");
			}
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
				throw std::runtime_error("error(arithmatic): divided by zero!\n");
			}
			result /= factor;
			break;
		case TokenType::Pow:
			tie(factor, input) = parseFactor(str);
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

	return { result, input };
}
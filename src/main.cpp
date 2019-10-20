#include <QApplication>

#include "CalculatorUI.h"
#include "parser.h"

#include <iostream>

int guiVersion(int argc, char* argv[])
{
	QApplication app(argc, argv);

	CalculatorUI w;
	w.show();

	return app.exec();
}

int consoleVersion(int argc, char* argv[])
{
	std::cout << "Hello! Welcome to use this small calculator!" << std::endl;
	while (1)
	{
		std::string input, res;
		double result;
		std::cout << ">> ";
		//获取一整行
		getline(std::cin, input);

		if (!input.empty())
		{
			try
			{
				std::tie(result, res) = parseExpression(input);
				//如果计算出来的数特别小就看作是0
				if (abs(result) < 1e-10)
				{
					result = 0;
				}
				if (res == "")
				{
					std::cout << "ans = " << result << std::endl;
				}
				else if (res != ";")
				{
					std::cout << "error(bad syntax): unexpect symbol " + res + "!" << std::endl;
				}
			}
			catch (std::exception& e)
			{
				std::cout << e.what();
			}
		}
	}

	return 0;
}


int main(int argc, char* argv[])
{
	//return guiVersion(argc, argv);
	return consoleVersion(argc, argv);
}
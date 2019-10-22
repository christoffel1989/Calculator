#include <QApplication>

#include "CalculatorUI.h"
#include "parser.h"
#include "ASTNode.h"
#include "CreateASTNode.h"
#include "TranslateASTNode.h"

#include <iostream>
#include <stdexcept>

int guiVersion(int argc, char* argv[])
{
	QApplication app(argc, argv);

	CalculatorUI w;
	w.show();

	return app.exec();
}

int consoleVersion(int argc, char* argv[])
{
	//构造全局环境
	Environment GlobalEnv;

	std::cout << "Hello! Welcome to use this small calculator!" << std::endl;
	while (1)
	{
		std::string input;
		std::cout << ">> ";
		//获取一整行
		getline(std::cin, input);

		if (!input.empty())
		{
			try
			{
				parseStatement(input, &GlobalEnv);
			}
			catch (std::exception& e)
			{
				std::cout << e.what();
			}
		}
	}

	return 0;
}

int consoleASTVersion(int argc, char* argv[])
{
	//构造全局环境
	ASTEnvironment GlobalEnv;

	std::cout << "Hello! Welcome to use this small calculator!" << std::endl;
	while (1)
	{
		std::string input;
		std::cout << ">> ";
		//获取一整行
		getline(std::cin, input);

		if (!input.empty())
		{
			try
			{
				auto[ast, res] = createStatementASTNode(input);
				auto result = executeAST(ast, &GlobalEnv);
				std::cout << "ans = " << result << std::endl;
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
	//return consoleVersion(argc, argv);

	return consoleASTVersion(argc, argv);
}
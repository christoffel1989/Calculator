#include <QApplication>

#include "CalculatorUI.h"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	CalculatorUI w;
	w.show();

	return app.exec();
}
#include <QApplication>

#include "MainWindow.h"
#include "CalculatorUI.h"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	//MainWindow w;
	CalculatorUI w;
	w.show();
	//w.resize(800, 600);

	return app.exec();
}
#include "MainWindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QAction>

#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	m_pHello = new QAction("Hello", this);

	QMenu* menu = menuBar()->addMenu("First");

	menu->addAction(m_pHello);

	connect(m_pHello, SIGNAL(triggered()), this, SLOT(actionHellow()));
}

void MainWindow::actionHellow()
{
	std::cout << "hello world" << std::endl;
}
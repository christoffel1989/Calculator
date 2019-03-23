#pragma once

#include <QMainWindow>

//前置声明
class QAction;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

private slots:
	void actionHellow();

private:
	QAction* m_pHello;
};
#pragma once

#include <QDialog>
#include "token.h"

struct Environment;
class QLineEdit;

class CalculatorUI : public QDialog
{
public:
	CalculatorUI(QWidget* parent = nullptr);

private:
	void onKeyButtonClicked(int row, int column);

private:
	static const int row = 7;
	static const int column = 5;
	QString m_KeyDict[row][column] =
	{
		{ "PI", "sin", "asin", "cos", "acos"},
		{ "e", "tan", "atan", "log", "ln" },
		{ "exp", "(", ")", "<-", "/" },
		{ "inv", "7", "8", "9", "*" },
		{ "sqrt", "4", "5", "6", "-" },
		{ "!", "1", "2", "3", "+" },
		{ "^", "CE", "0", ".", "=" }
	};

	QString m_Text;
	QLineEdit* m_pDisplay;

	bool m_HasCal;
	bool m_HasError;

	Environment m_Env;
};
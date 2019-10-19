﻿#pragma once

#include <QDialog>

class QLineEdit;

class CalculatorUI : public QDialog
{
public:
	CalculatorUI(QWidget* parent = nullptr);

private:
	void onKeyButtonClicked(int row, int column);

private:
	/*const int row = 5;
	const int column = 4;
	QString m_KeyDict[5][4] =
	{
		{ "(", ")", "<-", "*" },
		{ "7", "8", "9", "/" },
		{ "4", "5", "6", "+" },
		{ "1", "2", "3", "-" },
		{ "CE", "0", ".", "=" }
	};*/

	static const int row = 7;
	static const int column = 5;
	QString m_KeyDict[row][column] =
	{
		{ "sin", "cos", "tan", "PI", "e" },
		{ "asin", "acos", "atan", "exp", "ln" },
		{ "inv", "(", ")", "<-", "/" },
		{ "sqrt", "7", "8", "9", "*" },
		{ "abs", "4", "5", "6", "-" },
		{ "log", "1", "2", "3", "+" },
		{ "exp", "CE", "0", ".", "=" }
	};

	QString m_Text;
	QLineEdit* m_pDisplay;

	bool m_HasCal;
	bool m_HasError;
};
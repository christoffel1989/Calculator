#pragma once

#include <QDialog>

class QLineEdit;

class CalculatorUI : public QDialog
{
public:
	CalculatorUI(QWidget* parent = nullptr);

private:
	void onKeyButtonClicked(int row, int column);

private:
	QString m_KeyDict[5][4] =
	{
		{ "(", ")", "<-", "*" },
		{ "7", "8", "9", "/" },
		{ "4", "5", "6", "+" },
		{ "1", "2", "3", "-" },
		{ "CE", "0", ".", "=" }
	};

	QString m_Text;
	QLineEdit* m_pDisplay;

	bool m_HasCal;
	bool m_HasError;
};
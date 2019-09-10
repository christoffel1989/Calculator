#pragma once

#include <QDialog>

#include <map>
#include <string>

class QPushButton;
class QLineEdit;
class QButtonGroup;

class CalculatorUI : public QDialog
{
public:
	CalculatorUI(QWidget* parent = nullptr);

private:
	void onKeyButtonClick(int id);

private:
	QPushButton* m_pKeyButton[5][4];
	QLineEdit* m_pDisplay;
	QButtonGroup* m_pKeyButtonGroup;

	std::map<int, std::string> m_KeyMap;

	QString m_ResultStr;

	bool m_HasCal;
	bool m_HasError;
};
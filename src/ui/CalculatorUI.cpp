﻿#include "CalculatorUI.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>

#include "parser.h"

CalculatorUI::CalculatorUI(QWidget* parent) : QDialog(parent)
{
	m_HasError = false;
	m_HasCal = false;
	m_ResultStr = "0.0";
	m_pDisplay = new QLineEdit(m_ResultStr, this);


	QString dict[5][4] = 
	{
		{ "(", ")", "CE", "*" },
		{ "7", "8", "9", "/" },
		{ "4", "5", "6", "+" },
		{ "1", "2", "3", "-" },
		{ "C", "0", ".", "=" }
	};

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_pKeyButton[i][j] = new QPushButton(dict[i][j], this);
			m_KeyMap.insert({ i * 4 + j, dict[i][j].toLatin1().data() });
			//利用lambda链接消息
			connect(m_pKeyButton[i][j], &QPushButton::clicked, [=](int id) { onKeyButtonClick(i * 4 + j); });
		}
	}

	QWidget* KeyPannel = new QWidget(this);
	QGridLayout* KeyLayout = new QGridLayout(KeyPannel);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			KeyLayout->addWidget(m_pKeyButton[i][j], i, j);
		}
	}

	QVBoxLayout* CenterLayout = new QVBoxLayout(this);
	CenterLayout->addWidget(m_pDisplay);
	CenterLayout->addWidget(KeyPannel);

	setLayout(CenterLayout);
}

void CalculatorUI::onKeyButtonClick(int id)
{
	//分情况讨论
	//清空按钮的情况
	if (m_KeyMap[id] == "CE")
	{
		m_ResultStr = "0.0";
	}
	//退一格的情况
	else if (m_KeyMap[id] == "C")
	{
		m_ResultStr.remove(m_ResultStr.length() - 1, 1);
		if (m_ResultStr == "")
		{
			m_ResultStr = "0.0";
		}

		if (m_HasError)
		{
			m_HasError = false;
			m_ResultStr = "0.0";
		}
	}
	//求值的情况
	else if (m_KeyMap[id] == "=")
	{
		try
		{
			//需要把QString 转换成 std::string
			double result;
			std::string res;
			std::tie(result, res) = parseExpression(m_ResultStr.toLatin1().data());
			m_ResultStr = QString::number(result);
			m_HasCal = true;
		}
		catch (std::exception& e)
		{
			m_HasError = true;
			m_ResultStr = QString(e.what());
		}
	}
	else
	{
		if (m_ResultStr == "0.0" || m_HasCal || m_HasError)
		{
			m_HasCal = false;
			m_HasError = false;
			m_ResultStr = QString(m_KeyMap[id].c_str());
		}
		else
		{
			m_ResultStr += QString(m_KeyMap[id].c_str());
		}
	}

	m_pDisplay->setText(m_ResultStr);
}
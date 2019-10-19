#include "CalculatorUI.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>

#include "parser.h"

CalculatorUI::CalculatorUI(QWidget* parent) : QDialog(parent)
{
	m_HasError = false;
	m_HasCal = false;
	m_Text = "0";
	m_pDisplay = new QLineEdit("0", this);

	auto pannel = new QWidget(this);
	auto gridlayout = new QGridLayout(this);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			auto button = new QPushButton(m_KeyDict[i][j], this);
			gridlayout->addWidget(button, i, j);
			connect(button, &QPushButton::clicked, [=]() { onKeyButtonClicked(i, j); });
		}
	}
	pannel->setLayout(gridlayout);

	auto layout = new QVBoxLayout(this);
	layout->addWidget(m_pDisplay);
	layout->addWidget(pannel);
	setLayout(layout);
}

void CalculatorUI::onKeyButtonClicked(int row, int column)
{
	//分情况讨论
	//清空按钮的情况
	if (m_KeyDict[row][column] == "CE")
	{
		m_Text = "0";
	}
	//退一格的情况
	else if (m_KeyDict[row][column] == "<-")
	{
		//当前处于计算错误的状态
		if (m_HasError)
		{
			m_HasError = false;
			m_Text = "0";
		}
		//如果是完成计算的情况下 按退格无效
		else if (!m_HasCal)
		{
			m_Text.remove(m_Text.length() - 1, 1);
			//如果所有的数字都被删光了就直接重置为0
			if (m_Text == "")
			{
				m_Text = "0";
			}
		}
	}
	//小数点
	else if (m_KeyDict[row][column] == ".")
	{
		////之前的逻辑是有问题的 会导致一个表达式中只能出现一个小数
		////所以逻辑做了更复杂的修改

		//从后向前寻找'.'
		auto index = m_Text.lastIndexOf('.');
		//如果当前输入并无小数点
		if (index == -1)
		{
			m_Text += ".";
		}
		else
		{
			//取最后一个小数点之后剩下的字串
			auto res = m_Text.right(m_Text.length() - index - 1);
			// 如果存在小数点的位置之后还存在运算符或者括号 并且 最后一个字符是数字
			//判断是否还存在符号利用了正则表达式
			if (res.contains(QRegExp("[+-*/()]")) && res.rbegin()->isNumber())
			{
				//则输入小数点有效
				m_Text += ".";
			}
		}
	}
	else if (m_KeyDict[row][column] == "0")
	{
		////之前的逻辑是有问题的 会导致第二个数字可能还是出现00的情况
		////所以逻辑做了更复杂的修改

		//如果当前字串是0
		if (m_Text == "0")
		{
			//无法输入0 因为不能显示00形式的数字
		}
		else
		{
			//寻找最后一个运算符或者括号
			auto index = m_Text.lastIndexOf(QRegExp("[+-*/()]"));
			//如果确实存在
			if (index != -1)
			{
				//取运算符或者括号后剩下的字串好为0
				auto res = m_Text.right(m_Text.length() - index - 1);
				if (res != "0")
				{
					m_Text += "0";
				}
			}
			else
			{
				m_Text += "0";
			}
			
		}
	}
	else if (m_KeyDict[row][column] == "=")
	{
		try
		{
			//需要把QString 转换成 std::string
			double result;
			std::string res;
			std::tie(result, res) = parseExpression(m_Text.toLatin1().data());
			//存在残渣
			if (!res.empty())
			{
				m_Text =  "unexpect symbo ( or )!";
				m_HasError = true;
			}
			else
			{
				//如果计算出来的数特别小就看作是0
				if (abs(result) < 1e-10)
				{
					result = 0;
				}
				m_Text = QString::number(result);
				m_HasCal = true;
			}
		}
		//出现异常 例如除以0或者括号不平衡 则在屏幕上显示错误信息
		catch (std::exception& e)
		{
			m_HasError = true;
			m_Text = QString(e.what());
		}
	}
	//其他数字 括号
	else
	{
		//初始为0 或者 计算错误 或者计算完毕 则新的输入将把当前显示的东西完全覆盖
		if (m_Text == "0" || m_HasCal || m_HasError)
		{
			m_HasCal = false;
			m_HasError = false;
			m_Text = m_KeyDict[row][column];
		}
		else
		{
			m_Text += m_KeyDict[row][column];
		}
	}

	m_pDisplay->setText(m_Text);
}
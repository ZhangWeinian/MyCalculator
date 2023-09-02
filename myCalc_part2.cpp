#pragma once

#include "stdafx.h"

#include "./myCalc.h"
#include <ui_myCalc.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <version>
#include <cmath>

#include <qchar.h>
#include <qlist.h>
#include <qobject.h>
#include <qstack.h>
#include <qstring.h>
#include <qabstractbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qnamespace.h>

#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/detail/et_ops.hpp>
#include <boost/multiprecision/detail/number_compare.hpp>
#include <boost/multiprecision/number.hpp>

// 基础功能初始化
_NORETURN void myCalc::initUI(void)
{
	// 显示初始化
	showMode(ShowModeSign::_Basic);

	// 数据初始化
	initData();

	// 功能初始化
	initFun();
}

_NORETURN void myCalc::initData(void)
{
	info = _init_UPtr(BasicInformation);
	flag = _init_UPtr(ImportantFlag);
	buttonGroup = _init_UPtr(QButtonGroup, this);
}

_NORETURN void myCalc::initFun(void)
{
	// 找到所有按钮，放到一个组里
	auto btnList = findChildren<QPushButton*>();
	for (auto& btn : btnList)
	{
		buttonGroup->addButton(btn);
	}

	// 关联信号到槽
	connect(buttonGroup.get(), &QButtonGroup::buttonClicked, this, &myCalc::onButtonGroupCliked);

	// 监听键盘
	this->grabKeyboard();

	// line1  与 line2 不可获取焦点，无法编辑
	ui.line1->setFocusPolicy(_QT NoFocus);
	ui.line2->setFocusPolicy(_QT NoFocus);
}

// 控制三个特殊位置的显示
_NORETURN void myCalc::showMode(const ShowModeSign& _Sign)
{
	switch (_Sign)
	{
		case ShowModeSign::_Basic:
		{
			ui.btn1->setText("( )");
			ui.btn2->setText("CE");
			ui.btn3->setText("C");

			break;
		}

		case ShowModeSign::_BC:
		{
			ui.btn1->setText("(");
			ui.btn2->setText(")");
			ui.btn3->setText("C");

			break;
		}

		case ShowModeSign::_BCE:
		{
			ui.btn1->setText("(");
			ui.btn2->setText(")");
			ui.btn3->setText("CE");

			break;
		}

		case ShowModeSign::_OnlyBK:
		{
			ui.btn1->setText("(");
			ui.btn2->setText(")");

			break;
		}

		case ShowModeSign::_OnlyCE:
		{
			ui.btn3->setText("CE");

			break;
		}

		case ShowModeSign::_OnlyC:
		{
			ui.btn3->setText("C");

			break;
		}

		default:
			break;
	}
}

// 判断基础操作符
_NODISCARD bool myCalc::isBasicOper(const qstr& BasicOperEvent) const
{
	if (BasicOperEvent.isEmpty())
	{
		return false;
	}
	else
	{
		return _STD any_of(
			TheBasicOper.begin(), TheBasicOper.end(), [&BasicOperEvent](const auto& i) { return i == BasicOperEvent; });
	}
}

// 判断高级操作符
_NODISCARD bool myCalc::isAdvancedOper(const qstr& AdvancedOperEvent) const
{
	if (AdvancedOperEvent.isEmpty())
	{
		return false;
	}
	else
	{
		return _STD any_of(TheAdvancedOper.begin(),
						   TheAdvancedOper.end(),
						   [&AdvancedOperEvent](const auto& i) { return i == AdvancedOperEvent; });
	}
}

// 判断数字
_NODISCARD bool myCalc::isNum(const qstr& NumEvent) const
{
	auto funTmp = [&](const auto& i)
	{
		return _STD any_of(TheNums.begin(), TheNums.end(), [&i](const auto& j) { return i == j; });
	};

	if (NumEvent.isEmpty())
	{
		return false;
	}
	else if (NumEvent.size() == 1)
	{
		return funTmp(NumEvent.front());
	}
	else
	{
		return _STD all_of(NumEvent.begin(), NumEvent.end(), [&](const auto& i) { return i == '-' || funTmp(i); });
	}
}

// 判断控制符
_NODISCARD bool myCalc::isCtrl(const qstr& CtrlEvent) const
{
	if (CtrlEvent.isEmpty())
	{
		return false;
	}
	else
	{
		return _STD any_of(TheCtrl.begin(), TheCtrl.end(), [&CtrlEvent](const auto& i) { return i == CtrlEvent; });
	}
}

// 判断来自键盘的事件是否应该响应
_NODISCARD bool myCalc::isCorresponding(const int QtKey) const
{
	return _STD any_of(
		TheCorrespondingStr.begin(), TheCorrespondingStr.end(), [&QtKey](const auto& i) { return QtKey == i.first; });
}

// 获取键盘事件转换后的文本
_NODISCARD qstr myCalc::getCorrespondingStr(const int StrKey) const
{
	qstr ans = "";

	_STD for_each(TheCorrespondingStr.begin(),
				  TheCorrespondingStr.end(),
				  [&](const auto& i)
				  {
					  if (i.first == StrKey)
					  {
						  ans = i.second;
					  }
				  });

	return ans;
}

// 获取键盘事件转换后的数字
_NODISCARD qstr myCalc::getCorrespondingNum(const int NumKey) const
{
	qstr ans = "";

	_STD for_each(TheCorrespondingNum.begin(),
				  TheCorrespondingNum.end(),
				  [&](const auto& i)
				  {
					  if (i.first == NumKey)
					  {
						  ans = i.second;
					  }
				  });

	return ans;
}

// 识别按钮类型
_NODISCARD BtnType myCalc::getBtnType(const qstr& BtnEvent) const
{
	try
	{
		if (isNum(BtnEvent))
		{
			return BtnType::_Num;
		}
		else if (isBasicOper(BtnEvent))
		{
			if (BtnEvent == "( )" || BtnEvent == "(")
			{
				return BtnType::_LeftBK;
			}
			else if (BtnEvent == ")")
			{
				return BtnType::_RightBK;
			}
			else
			{
				return BtnType::_BasicOper;
			}
		}
		else if (isAdvancedOper(BtnEvent))
		{
			return BtnType::_AdvancedOper;
		}
		else if (isCtrl(BtnEvent))
		{
			return BtnType::_Ctrl;
		}
		else
		{
			throw BtnType::_TypeErr;
		}
	}
	catch (const BtnType& errCode)
	{
		return errCode;
	}
}

// 识别控制类型
_NODISCARD CtrlType myCalc::getCtrlType(const qstr& CtrlEvent) const
{
	try
	{
		if (CtrlEvent == "C")
		{
			return CtrlType::_C;
		}
		else if (CtrlEvent == "CE")
		{
			return CtrlType::_CE;
		}
		else if (CtrlEvent == "DEL")
		{
			return CtrlType::_DEL;
		}
		else if (CtrlEvent == "=")
		{
			return CtrlType::_EQT;
		}
		else if (CtrlEvent == "EXIT")
		{
			return CtrlType::_EXIT;
		}
		else
		{
			throw CtrlType::_TypeErr;
		}
	}
	catch (const CtrlType& errCode)
	{
		return errCode;
	}
}

// 识别显示类型
_NODISCARD ShowModeSign myCalc::getBtnShowType(void) const
{
	if (ui.btn1->text() == "( )")
	{
		return ShowModeSign::_Basic;
	}
	else if (ui.btn3->text() == "CE")
	{
		return ShowModeSign::_BCE;
	}
	else if (ui.btn3->text() == "C")
	{
		return ShowModeSign::_BC;
	}
	else
	{
		return (ShowModeSign)_PUBINFO_ERROR;
	}
}

// 获取运算符优先级
_NODISCARD bool myCalc::getPriority(const qstr& Oper1, const qstr& Oper2) const
{
	Type priOper1 = 0;
	Type priOper2 = 0;

	for (const auto& [first, second] : ThePriority)
	{
		if (first == Oper1)
		{
			priOper1 = second;
		}
		if (first == Oper2)
		{
			priOper2 = second;
		}
	}

	return priOper1 > priOper2;
}

// 处理括号
_NORETURN void myCalc::handleBkEvent(const qstr& BkEvent)
{
	if (auto tmp_BK = (BkEvent == "( )") ? "(" : BkEvent; tmp_BK == "(")
	{
		flag->bkFlag++;
		takeData(tmp_BK, BtnType::_LeftBK);
	}
	else
	{
		flag->bkFlag--;
		takeData(tmp_BK, BtnType::_RightBK);
	}

	// 更新显示内容
	if (flag->bkFlag > 0)
	{
		showMode(ShowModeSign::_BCE);
	}
	else
	{
		showMode(ShowModeSign::_Basic);
	}
}

// 处理数字正负号
_NODISCARD bool myCalc::handleNumSign(void)
{
	if (info->Num == "0") // 不对0作正负号调整
	{
		return false;
	}

	if (flag->numSignFlag)
	{
		flag->numSignFlag = false; // 未完成一次正负号调整
	}

	if (info->Num.front() != '-')
	{
		info->Num = '-' + info->Num;
	}
	else
	{
		pop_front(info->Num);
	}

	return true;
}

// 处理delete建
_NORETURN void myCalc::handleDelete(void)
{
	if (info->lastTimeEvent->second == BtnType::_AdvancedOper)
	{
		clearLastAdvancedStr();
	}
	else if (ui.line2->text() != "0")
	{
		if (ui.line2->text().size() == 1)
		{
			ui.line2->setText("0");
		}
		else
		{
			if (ui.line2->text().back() == '.')
			{
				flag->dPointFlag = false;
			}

			ui.line2->backspace();
		}
	}

	info->Num = ui.line2->text();
}

// 基础操作符入栈
void myCalc::basicOperPushStack(const qstr& BasicOperEvent)
{
	if (BasicOperEvent == "(")
	{
		info->CalcStack.push(BasicOperEvent);

		return;
	}
	else if (BasicOperEvent == ")")
	{
		while (info->CalcStack.top() != "(")
		{
			info->HouZhui.push_back(info->CalcStack.pop());
		}

		info->CalcStack.pop();

		calcBracketStr();

		return;
	}

	if (!info->CalcStack.isEmpty())
	{
		auto& tmp = info->CalcStack.top();
		while (getPriority(tmp, BasicOperEvent))
		{
			info->HouZhui.push_back(info->CalcStack.pop());
			tmp = info->CalcStack.top();
		}
	}

	info->CalcStack.push(BasicOperEvent);
}

// 计算高级操作符表达式
_NODISCARD qstr myCalc::calcAdvancedOperData(const qstr& Data, const qstr& Oper) const
{
	_FLOAT numAns = 0.0L;
	auto numTmp = cove_type(getFloat(Data), _FLOAT);

	try
	{
		if (numTmp < cove_type(_NUM_MIN, _FLOAT) || numTmp > cove_type(_NUM_MAX, _FLOAT))
		{
			throw AdvancedOperErrCode::_Num_OutOfRange;
		}
		else if (Oper == "√x")
		{
			if (Data.front() == '-')
			{
				throw AdvancedOperErrCode::_Num_isNegative;
			}
			else
			{
				numAns = sqrt(numTmp);
			}
		}
		else if (Oper == "x²")
		{
			numAns = pow(numTmp, 2);
		}
		else if (Oper == "1/x")
		{
			if (Data == "0")
			{
				throw AdvancedOperErrCode::_Num_isZero;
			}
			else
			{
				numAns = 1.0L / numTmp;
			}
		}
		else if (Oper == "%")
		{
			numAns = 0.01L * numTmp;
		}
		else if (Oper == "!")
		{
			numAns = getFactorial(Data);

			if (numAns == -1)
			{
				throw AdvancedOperErrCode::_Num_isNegative;
			}
			else if (numAns == -2)
			{
				throw AdvancedOperErrCode::_Num_OutOfRange;
			}
		}

		return getQstr(numAns);
	}
	catch (const AdvancedOperErrCode& errCode)
	{
		flag->calcFlag = true;

		switch (errCode)
		{
			case AdvancedOperErrCode::_Num_isNegative:
			case AdvancedOperErrCode::_Num_isZero:
			{
				return "无效输入";
			}

			case AdvancedOperErrCode::_Num_OutOfRange:
			{
				return "超出范围";
			}

			default:
			{
				return "myCalc::calcAdvancedOperData() ERR!";
			}
		}
	}
}

// 获取阶乘
_NODISCARD _FLOAT myCalc::getFactorial(const qstr& Data) const
{
	auto num = cove_type(getFloat(Data), _INT);
	LL ans = 1LL;

	if (num < 0)
	{
		return -1; //未定义负数的阶乘
	}
	else if (num == 0)
	{
		return 1;
	}
	else if (num >= 20)
	{
		return -2; //计算结果超出范围
	}
	else
	{
		for (int i = 2; i <= num; i++)
		{
			ans *= i;
		}

		return cove_type(ans, _FLOAT);
	}
}

// 获取当前显示在输入栏中的文本
_NODISCARD qstr myCalc::getDisplayingStr(void) const
{
	return ui.line2->text();
}

// 配合 C/CE，清除内容
_NORETURN void myCalc::clear(const ClearSign& _Sign)
{
	clearLastAdvancedStr();

	info->clear(_Sign);
	flag->clear(_Sign);

	if (_Sign == ClearSign::_All)
	{
		ui.line1->clear();
	}

	ui.line2->setText("0");
}

// 清除最后一个高级操作符表达式
_NORETURN void myCalc::clearLastAdvancedStr(void)
{
	info->lastAdvancedOperStr.clear();
	info->Num.clear();

	if (!info->HouZhui.isEmpty())
	{
		info->HouZhui.pop_back();
	}
}

// 配合基础操作符，进行简单计算
_NODISCARD qstr myCalc::simpleCalc(const _FLOAT& OperNum2, const qstr& OperSymbol, const _FLOAT& OperNum1) const
{
	_FLOAT ansNum = 0.0L;

	try
	{
		if (OperSymbol == "+")
		{
			ansNum = OperNum1 + OperNum2;
		}
		else if (OperSymbol == "-")
		{
			ansNum = OperNum1 - OperNum2;
		}
		else if (OperSymbol == "*")
		{
			ansNum = OperNum1 * OperNum2;
		}
		else if (OperSymbol == "/")
		{
			if (OperNum2 == 0)
			{
				throw AdvancedOperErrCode::_Num_isZero;
			}
			else
			{
				ansNum = OperNum1 / OperNum2;
			}
		}

		return getQstr(ansNum);
	}
	catch (const AdvancedOperErrCode& errCode)
	{
		if (errCode == AdvancedOperErrCode::_Num_isZero)
		{
			return "被除数为0";
		}
		else
		{
			return "myCalc::simpleCalc() ERR!";
		}
	}
}

// 配合高级操作符，更新显示文本
_NODISCARD bool myCalc::updataZhongZhuiStr(void)
{
	auto [begin, end] = findBracketStr(_NeedPosition);

	if (begin == 0 && end == 0)
	{
		return false;
	}
	else
	{
		info->ZhongZhuiStr.truncate(begin);
		return true;
	}
}

// 寻找最小括号表达式位置
_NODISCARD BKPosition myCalc::findBracketStr(const BKPosition&)
{
	size_t strB = info->ZhongZhuiStr.size() - 1;
	size_t strE = strB - 1;
	size_t bkNums = 0;
	size_t tmp = strB;

	try
	{
		for (auto i = info->ZhongZhuiStr.rbegin() + 1; i != info->ZhongZhuiStr.rend(); i++)
		{
			if (*i == ')')
			{
				bkNums++;
			}
			else if (*i == '(')
			{
				bkNums--;
			}

			if (bkNums == 0)
			{
				strB = tmp - 1;
				break;
			}
			else
			{
				tmp--;
			}
		}

		if (bkNums != 0)
		{
			throw StackErrCode::_Bracket_Mismatch;
		}
		else
		{
			return { strB, strE };
		}
	}
	catch (const StackErrCode&)
	{
		ui.line2->setText("括号不匹配");
		flag->calcFlag = true;

		return { 0, 0 };
	}
}

// 配合计算，获取栈顶数字
_NODISCARD _FLOAT myCalc::getOperNum(_SK_QSTR& OperSk)
{
	if (OperSk.isEmpty())
	{
		flag->calcStackEmpty = true;
		return 0.0;
	}
	else
	{
		auto ans = OperSk.pop();
		if (ans.front() == '(')
		{
			pop_back_front(ans);

			return getFloat("-" + ans);
		}
		else
		{
			return getFloat(ans);
		}
	}
}

// 防止重复记录
_NODISCARD bool myCalc::dontDuplicateRecord(const ClickEvent& Event) const
{
	if (Event != *info->lastTimeEvent)
	{
		return true;
	}
	else if (Event.second == BtnType::_Num || Event.second == BtnType::_AdvancedOper)
	{
		return true;
	}
	else if (Event.first == "±" || Event.first == "( )" || Event.first == "(" || Event.first == ")" ||
			 Event.first == "DEL" || Event.first == "#")
	{
		return true;
	}
	else
	{
		return false;
	}
}

// line2 只能显示数字
void myCalc::setLine2Displaying(void)
{
	qstr tmp = getDisplayingStr();

	while (!isNum(tmp.back()))
	{
		pop_back(tmp);
	}

	ui.line2->setText(tmp);
}

// 预处理
_NORETURN void myCalc::Preprocessing(const ClickEvent& Event)
{
	// 预处理1：若已经完成一次完整的计算，则清除之前的信息
	if (flag->calcFlag)
	{
		clear(ClearSign::_All);

		if (Event.second != BtnType::_Num)
		{
			flag->numGetOverFlag = false;
			info->Num = info->lastAns;
		}
	}

	// 预处理2：如果完成了一次高级操作符的计算，则保存高级操作符表达式以便必要时嵌套使用
	if (info->lastTimeEvent->second == BtnType::_AdvancedOper)
	{
		if (Event.second == BtnType::_Num)
		{
			clearLastAdvancedStr();
			ui.line1->setText(info->ZhongZhuiStr);
		}
		else if (Event.second == BtnType::_BasicOper || Event.second == BtnType::_LeftBK ||
				 Event.second == BtnType::_RightBK || Event.second == BtnType::_Ctrl)
		{
			takeData(info->Num, BtnType::_AdvancedOper);
		}

		if (Event.second != BtnType::_Num && Event.second != BtnType::_AdvancedOper)
		{
			info->ZhongZhuiStr += (info->lastAdvancedOperStr + " ");
		}
	}

	// 预处理3：调整显示模式
	if (Event.first != "C" && getBtnShowType() == ShowModeSign::_BC)
	{
		showMode(ShowModeSign::_BCE);
	}
}

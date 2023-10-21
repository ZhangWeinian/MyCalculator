#pragma once

#include "./myCalc.h"
#include <ui_myCalc.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <qabstractbutton.h>
#include <qbuttongroup.h>
#include <qchar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstack.h>
#include <qstring.h>
#include <qwidget.h>
#include <utility>
#include <version>

// 基础功能初始化
void myCalc::initUI(void) noexcept
{
	// 显示初始化
	showMode(ShowModeSign::_Basic);

	// 功能初始化
	initFun();
}

// 函数初始化
void myCalc::initFun(void) noexcept
{
	// 找到所有按钮，放到一个组里
	for (auto& btn: findChildren<QPushButton*>())
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
void myCalc::showMode(const ShowModeSign& _Sign) noexcept
{
	switch (_Sign)
	{
		using enum ShowModeSign;

		case _Basic:
		{
			ui.btn1->setText("( )");
			ui.btn2->setText("CE");
			ui.btn3->setText("C");

			break;
		}

		case _BC:
		{
			ui.btn1->setText("(");
			ui.btn2->setText(")");
			ui.btn3->setText("C");

			break;
		}

		case _BCE:
		{
			ui.btn1->setText("(");
			ui.btn2->setText(")");
			ui.btn3->setText("CE");

			break;
		}

		case _OnlyBK:
		{
			ui.btn1->setText("(");
			ui.btn2->setText(")");

			break;
		}

		case _OnlyCE:
		{
			ui.btn3->setText("CE");

			break;
		}

		case _OnlyC:
		{
			ui.btn3->setText("C");

			break;
		}

		default:
		{
			break;
		}
	}
}

// 判断基础操作符
bool myCalc::isBasicOper(const _qstr& BasicOperEvent) const noexcept
{
	if (BasicOperEvent.isEmpty())
	{
		return false;
	}
	else
	{
		return _RG any_of(TheBasicOper, [&BasicOperEvent](const auto& i) { return i == BasicOperEvent; });
	}
}

// 判断高级操作符
bool myCalc::isAdvancedOper(const _qstr& AdvancedOperEvent) const noexcept
{
	if (AdvancedOperEvent.isEmpty())
	{
		return false;
	}
	else
	{
		return _RG any_of(TheAdvancedOper, [&AdvancedOperEvent](const auto& i) { return i == AdvancedOperEvent; });
	}
}

// 判断数字
bool myCalc::isNum(const _qstr& NumEvent) const noexcept
{
	auto funTmp = [&](const auto& i)
	{
		return _RG any_of(TheNums, [&i](const auto& j) { return i == j; });
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
		return _RG all_of(NumEvent, [&](const auto& i) { return i == '-' || funTmp(i); });
	}
}

// 判断控制符
bool myCalc::isCtrl(const _qstr& CtrlEvent) const noexcept
{
	if (CtrlEvent.isEmpty())
	{
		return false;
	}
	else
	{
		return _RG any_of(TheCtrl, [&CtrlEvent](const auto& i) { return i == CtrlEvent; });
	}
}

// 判断来自键盘的事件是否应该响应
bool myCalc::isCorresponding(const int QtKey) const
{
	return _RG any_of(TheCorrespondingStr, [&QtKey](const auto& i) { return QtKey == i.first; });
}

// 获取键盘事件转换后的文本
_qstr myCalc::getCorrespondingStr(const int StrKey) const noexcept
{
	_qstr ans = "";

	_RG for_each(TheCorrespondingStr,
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
_qstr myCalc::getCorrespondingNum(const int NumKey) const noexcept
{
	_qstr ans { "" };

	_RG for_each(TheCorrespondingNum,
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
BtnType myCalc::getBtnType(const _qstr& BtnEvent) const noexcept
{
	try
	{
		using enum BtnType;

		if (isNum(BtnEvent))
		{
			return _Num;
		}
		else if (isBasicOper(BtnEvent))
		{
			if (BtnEvent == "( )" || BtnEvent == "(")
			{
				return _LeftBK;
			}
			else if (BtnEvent == ")")
			{
				return _RightBK;
			}
			else
			{
				return _BasicOper;
			}
		}
		else if (isAdvancedOper(BtnEvent))
		{
			return _AdvancedOper;
		}
		else if (isCtrl(BtnEvent))
		{
			return _Ctrl;
		}
		else
		{
			throw _TypeErr;
		}
	}
	catch (const BtnType& errCode)
	{
		return errCode;
	}
}

// 识别控制类型
CtrlType myCalc::getCtrlType(const _qstr& CtrlEvent) const noexcept
{
	try
	{
		using enum CtrlType;

		if (CtrlEvent == "C")
		{
			return _C;
		}
		else if (CtrlEvent == "CE")
		{
			return _CE;
		}
		else if (CtrlEvent == "DEL")
		{
			return _DEL;
		}
		else if (CtrlEvent == "=")
		{
			return _EQT;
		}
		else if (CtrlEvent == "EXIT")
		{
			return _EXIT;
		}
		else
		{
			throw _TypeErr;
		}
	}
	catch (const CtrlType& errCode)
	{
		return errCode;
	}
}

// 识别显示类型
ShowModeSign myCalc::getBtnShowType(void) const noexcept
{
	using enum ShowModeSign;

	if (ui.btn1->text() == "( )")
	{
		return _Basic;
	}
	else if (ui.btn3->text() == "CE")
	{
		return _BCE;
	}
	else if (ui.btn3->text() == "C")
	{
		return _BC;
	}
	else
	{
		return _cove_type(_PUBINFO_ERROR, ShowModeSign);
	}
}

// 获取运算符优先级
bool myCalc::getPriority(const _qstr& Oper1, const _qstr& Oper2) const noexcept
{
	Type priOper1 {};
	Type priOper2 {};

	for (const auto& [first, second]: ThePriority)
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
void myCalc::handleBkEvent(const _qstr& BkEvent)
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
bool myCalc::handleNumSign(void)
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
		info->Num.push_front('-');
	}
	else
	{
		pop_front(info->Num);
	}

	return true;
}

// 处理delete建
void myCalc::handleDelete(void)
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
				flag->dPointFlag  = false;
				flag->haveDecimal = false;
			}

			ui.line2->backspace();

			formatDisplaying();
		}
	}

	info->Num = clearCommas(ui.line2->text());
}

// 基础操作符入栈
void myCalc::basicOperPushStack(const _qstr& BasicOperEvent)
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
_qstr myCalc::calcAdvancedOperData(const _qstr& Data, const _qstr& Oper) const
{
	FLOAT numAns = 0.0L;
	auto  numTmp = _cove_type(getFloat(Data), FLOAT);

	try
	{
		if (numTmp < _cove_type(_NUM_MIN, FLOAT) || numTmp > _cove_type(_NUM_MAX, FLOAT))
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
			using enum AdvancedOperErrCode;

			case _Num_isNegative:
			case _Num_isZero:
			{
				return "无效输入";
			}

			case _Num_OutOfRange:
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
FLOAT myCalc::getFactorial(const _qstr& Data) const noexcept
{
	auto num = _cove_type(getFloat(Data), INT);
	LL	 ans = 1LL;

	if (num < 0)
	{
		return -1L; //未定义负数的阶乘
	}
	else if (num == 0)
	{
		return 1L;
	}
	else if (num >= 20)
	{
		return -2L; //计算结果超出范围
	}
	else
	{
		for (int i = 2; i <= num; i++)
		{
			ans *= i;
		}

		return _cove_type(ans, FLOAT);
	}
}

// 获取当前显示在输入栏中的文本
_qstr myCalc::getDisplayingStr(void) const noexcept
{
	return ui.line2->text();
}

// 标准数字显示
void myCalc::formatDisplaying(const _qstr& Num)
{
	auto ShowStr = Num.isEmpty() ? getDisplayingStr() : Num;

	if (!flag->haveDecimal || flag->getLastAns)
	{
		bool  haveNumSign = false;
		bool  haveDecimal = false;
		_qstr decimal	  = "";

		if (ShowStr.front() == '-')
		{
			pop_front(ShowStr);
			haveNumSign = true;
		}
		if (auto i = ShowStr.indexOf('.'); i != -1)
		{
			decimal = ShowStr.mid(i);
			ShowStr = ShowStr.left(i);

			haveDecimal		  = true;
			flag->haveDecimal = true;
		}

		if (ShowStr.size() >= 4)
		{
			ShowStr = clearCommas(ShowStr);
			reverse_qstr(ShowStr);

			auto len = ShowStr.size();
			for (auto i = 3, j = 0; i < len; i += 3, j++)
			{
				ShowStr.insert(i + j, ',');
			}

			reverse_qstr(ShowStr);
		}

		if (haveDecimal)
		{
			ShowStr += decimal;
		}
		if (haveNumSign)
		{
			ShowStr = "-" + ShowStr;
		}
	}

	ui.line2->setText(ShowStr);
}

// 清理数字中的逗号
_qstr myCalc::clearCommas(const _qstr& Str) const noexcept
{
	_qstr ans = "";
	for (const auto& i: Str)
	{
		if (i != ',')
		{
			ans += i;
		}
	}

	return ans;
}

// 配合 C/CE，清除内容
void myCalc::clear(const ClearSign& _Sign)
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
void myCalc::clearLastAdvancedStr(void)
{
	info->lastAdvancedOperStr.clear();
	info->Num.clear();

	if (!info->HouZhui.isEmpty())
	{
		info->HouZhui.pop_back();
	}
}

// 配合基础操作符，进行简单计算
_qstr myCalc::simpleCalc(const FLOAT& OperNum2, const _qstr& OperSymbol, const FLOAT& OperNum1) const noexcept
{
	FLOAT ansNum = 0.0L;

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
bool myCalc::updataZhongZhuiStr(void)
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
BKPosition myCalc::findBracketStr(const BKPosition&)
{
	size_t strB	  = info->ZhongZhuiStr.size() - 1;
	size_t strE	  = strB - 1;
	size_t bkNums = 0;
	size_t tmp	  = strB;

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
FLOAT myCalc::getOperNum(_def_qsk() & OperSk) noexcept
{
	if (OperSk.isEmpty())
	{
		flag->calcStackEmpty = true;
		return 0.0L;
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
bool myCalc::dontDuplicateRecord(const ClickEvent& Event) const noexcept
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
	_qstr tmp = getDisplayingStr();

	while (!isNum(tmp.back()))
	{
		pop_back(tmp);
	}

	ui.line2->setText(tmp);
}

// 预处理
void myCalc::thePreprocessing(const ClickEvent& Event) noexcept
{
	using enum BtnType;

	// 预处理1：若已经完成一次完整的计算，则清除之前的信息
	if (flag->calcFlag)
	{
		clear(ClearSign::_All);

		if (Event.second != _Num)
		{
			flag->numGetOverFlag = false;
			info->Num			 = info->lastAns;
		}
	}

	// 预处理2：如果完成了一次高级操作符的计算，则保存高级操作符表达式，以便必要时嵌套使用
	if (info->lastTimeEvent->second == _AdvancedOper)
	{
		if (Event.second == _Num)
		{
			clearLastAdvancedStr();
			ui.line1->setText(info->ZhongZhuiStr);
		}
		else if (Event.second == _BasicOper || Event.second == _LeftBK || Event.second == _RightBK ||
				 Event.second == _Ctrl)
		{
			takeData(info->Num, _AdvancedOper);
		}

		if (Event.second != _Num && Event.second != _AdvancedOper)
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

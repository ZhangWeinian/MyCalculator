#pragma once

#include "myCalc.h"
#include <ui_myCalc.h>

// 0.1 初始化
myCalc::myCalc(QWidget* parent): QWidget(parent)
{
	ui.setupUi(this);

	initUI();
}

// 0.2 析构
myCalc::~myCalc(void) = default;

// 1.1 监听鼠标点击的事件
void myCalc::onButtonGroupCliked(const QAbstractButton* Btn) noexcept
{
	// 通过鼠标点击屏幕获得的的内容和类型
	auto text { _move(Btn->text()) };
	auto type { _move(getBtnType(text)) };

	flag->fromKeyboard = false;
	flag->fromClickEvt = true;

	// 处理事件
	inspectionStr_InputTime(_def_pair(text, type));
}

// 1.2 监听键盘点击（结束）事件
void myCalc::keyReleaseEvent(QKeyEvent* KeyEvent)
{
	if (!KeyEvent->isAutoRepeat()) // 不听取持续事件
	{
		ui.line2->setEnabled(true);
		ui.line2->setFocus();

		if (const auto& text { _move(getKeyboardText(KeyEvent)) }; !_isEmpty(text))
		{
			// 不听取错误类型(对于计算器，如英文字母)
			if (const auto& type { getBtnType(text) }; type != BtnType::_TypeErr)
			{
				flag->fromKeyboard = true;
				flag->fromClickEvt = false;

				inspectionStr_InputTime(_def_pair(text, type));
			}

			if (flag->charMode == true)
			{
				this->showMinimized();
			}
		}
	}
}

// 1.3 部分键盘文本转换
_qstr myCalc::getKeyboardText(const QKeyEvent* KeyEvent) noexcept
{
	if (const auto& key { KeyEvent->key() }; key == 16777248)
	{
		setLine2Displaying();
	}
	else if (const auto& text { _move(KeyEvent->text()) }; isNum(text))
	{
		return _move(getCorrespondingNum(key));
	}
	else if (isCorresponding(key))
	{
		return _move(getCorrespondingStr(key));
	}

	return _qstr("");
}

// 1.3 为了处理异常自主添加的事件
void myCalc::onInspectionStr(const ClickEvent& AddEvent) noexcept
{
	handleEvent(AddEvent);
}

// 2.1 输入时检查是否合理
void myCalc::inspectionStr_InputTime(const ClickEvent& ThisTimeEvent)
{
	bool pass { true };
	bool temp { true };

	// case1：防止重复输入
	if (!dontDuplicateRecord(ThisTimeEvent))
	{
		pass = false;
		temp = false;
	}

	// case2：检查输入的数字是否过大
	if (temp && (_cove_type(getFloat(getDisplayingStr()), LL) > _NUM_MAX))
	{
		clear(ClearSign::_All);
		ui.line2->setText("输入的数字过大");

		pass = false;
		temp = false;
	}

	// case3：在左括号与数字之间添加一个乘号
	if (temp && (ThisTimeEvent.second == BtnType::_LeftBK) && (info->lastTimeEvent->second == BtnType::_Num))
	{
		onInspectionStr(_make_ce("*", BtnType::_BasicOper));
	}

	// case4：特殊处理来自键盘的、连续两次 DEL 事件
	if (temp && (flag->fromKeyboard == true) && (info->lastTimeEvent->first == "DEL") &&
		(ThisTimeEvent.first == "DEL") && (getDisplayingStr() == "0"))
	{
		onInspectionStr(_make_ce("C", BtnType::_Ctrl));

		pass = false;
	}

	// case5：点击基础操作符后直接按等号
	if (temp && (ThisTimeEvent.first == "=") && (info->lastTimeEvent->second == BtnType::_BasicOper))
	{
		takeData(info->Symbol, BtnType::_BasicOper);
		takeData(_move(getDisplayingStr()), BtnType::_Num);
	}

	// case5：监听字符模式
	if (ThisTimeEvent.first == "#")
	{
		if (info->lastTimeEvent->first == "#")
		{
			onInspectionStr(_make_ce("C", BtnType::_Ctrl));

			// 此处实现 字符模式 初始化或调用。例如：
			//	*flag->charMode = true;
			//	charModeWindow->show();

			ui.line2->setText("进入字符模式");
		}
		else
		{
			*info->lastTimeEvent = ThisTimeEvent;
		}

		pass = false;
	}

	// 检查完成后，处理此次点击事件
	if (pass)
	{
		handleEvent(ThisTimeEvent);
	}
}

// 2.2 正式处理点击事件
void myCalc::handleEvent(const ClickEvent& Event)
{
	// 预处理
	thePreprocessing(Event);

	// 记录本次事件的内容
	*info->thisTimeEvent = Event;

	// 按类处理事件
	switch (Event.second)
	{
		using enum BtnType;

		case _Num:
		{
			handleNum(Event.first);
			break;
		}

		case _BasicOper:
		case _LeftBK:
		case _RightBK:
		{
			handleBasicOper(Event.first);
			break;
		}

		case _AdvancedOper:
		{
			handleAdvancedOper(Event.first);
			break;
		}

		case _Ctrl:
		{
			handleCtrl(getCtrlType(Event.first));
			break;
		}

		default:
			break;
	}

	// 处理显示
	showInfo(Event);

	// 存储上一次事件的内容
	*info->lastTimeEvent = Event;
}

// 2.3 处理数字
void myCalc::handleNum(const _qstr& NumEvent)
{
	// 标识符更新。其中，数字正负号调整是否结束不做要求
	if (!flag->operFlag)
	{
		flag->operFlag = true;						 // 运算符调整结束，可以记录

		info->Num.clear();							 // 清理之前存储的数字
		takeData(info->Symbol, BtnType::_BasicOper); // 记录运算符
	}
	flag->numGetOverFlag = false;					 // 未获取到一个完整的数字

	bool check { true };

	// 记录事件内容
	if (NumEvent == ".")
	{
		if (!flag->dPointFlag)
		{
			flag->dPointFlag									   = true;
			(info->Num = (info->Num.isEmpty() ? "0" : info->Num)) += NumEvent;
		}

		check = false;
	}

	if (check)
	{
		if (info->Num == "0")
		{
			info->Num = NumEvent;
		}
		else
		{
			info->Num += NumEvent;
		}
	}
}

// 2.4 处理基础操作符
void myCalc::handleBasicOper(const _qstr& BasicOperEvent)
{
	// 点击基础操作符之前，保证有一个数字被记录
	if (info->Num.isEmpty() && (BasicOperEvent != "( )") && (BasicOperEvent != "(") && (BasicOperEvent != ")"))
	{
		info->Num = "0";

		if (info->ZhongZhuiStr.isEmpty())
		{
			takeData("0", BtnType::_Num, false);
		}
	}

	bool check { true };

	// 数字正负号调整
	if ((BasicOperEvent == "±") && handleNumSign())
	{
		check = false;
	}

	// 标识符更新
	else if (check && !flag->numGetOverFlag) // 除正负号调整，点击其他基础操作符，均意味着：
	{
		flag->numGetOverFlag = true;		 // 1，已经获取到一个完整的数字，可以记录
		flag->numSignFlag	 = true;		 // 2，完成了一次正负号调整
		flag->dPointFlag	 = false;		 // 3，小数点标识符重置

		// 记录数字
		takeData(info->Num, BtnType::_Num);
	}
	else if (check && (info->lastTimeEvent->second == BtnType::_AdvancedOper))
	{
		flag->numSignFlag = true;  // 完成了一次正负号调整
		flag->dPointFlag  = false; // 小数点标识符重置
	}

	if (check)
	{
		// 括号处理
		if (BasicOperEvent == "( )" || BasicOperEvent == "(" || BasicOperEvent == ")")
		{
			takeData(info->Symbol, BtnType::_BasicOper);

			handleBkEvent(BasicOperEvent);
		}

		// 基础操作符中，其他无需特殊处理的操作符，直接记录
		else
		{
			info->Symbol = BasicOperEvent;

			flag->operFlag = false;
		}
	}
}

// 2.5 处理高级操作符
void myCalc::handleAdvancedOper(const _qstr& AdvancedOperEvent)
{
	// 此部分的操作符需要直接计算答案，故保证有一个数字被记录
	if ((info->lastTimeEvent->second == BtnType::_AdvancedOper || info->lastTimeEvent->first == ")") &&
		!info->HouZhui.isEmpty())
	{
		info->Num = info->HouZhui.back();
		info->HouZhui.pop_back();
	}
	else if (info->Num.isEmpty())
	{
		info->Num = "0";

		if (info->ZhongZhuiStr.isEmpty())
		{
			takeData("0", BtnType::_Num, false);
		}
	}

	// 标识符更新
	flag->numSignFlag	 = true;
	flag->dPointFlag	 = false;
	flag->numGetOverFlag = true;
	flag->haveDecimal	 = false;

	// 计算
	info->Num = _move(calcAdvancedOperData(info->Num, AdvancedOperEvent));

	if (_cove_type(getFloat(info->Num), LL) > _NUM_MAX)
	{
		info->Num = "计算结果超出最大值";

		flag->calcFlag = true;
	}

	info->lastAdvancedOper = AdvancedOperEvent;
}

// 2.6 处理控制符
void myCalc::handleCtrl(const CtrlType& _Type)
{
	switch (_Type)
	{
		using enum CtrlType;

		case _C:
		{
			clear(ClearSign::_All);
			showMode(ShowModeSign::_Basic);

			break;
		}

		case _CE:
		{
			clear(ClearSign::_OnlyLine2);

			if (flag->bkFlag != 0)
			{
				showMode(ShowModeSign::_OnlyC);
			}
			else
			{
				showMode(ShowModeSign::_Basic);
			}

			break;
		}

		case _DEL:
		{
			handleDelete();
			break;
		}

		case _EQT:
		{
			if (inspectionStr_FinallyCalc())
			{
				takeData(info->Num, BtnType::_Num);

				info->Num	  = _move(calcStr());
				info->lastAns = info->Num;
			}
			else
			{
				info->Num = "表达式错误过多！";
			}

			flag->calcFlag	 = true;
			flag->getLastAns = true;

			break;
		}

		case _TypeErr:
		{
			break;
		}

		case _EXIT:
		{
			_calc_exit(0)
		}

		default:
			break;
	}
}

// 3.1 记录左值数据
void myCalc::takeData(_qstr& Data, const BtnType& _Type, bool ClearData)
{
	bool check { true };

	if (Data.isEmpty() || _Type == _PUBINFO_ERROR)
	{
		check = false;
	}

	if (check)
	{
		switch (_Type)
		{
			using enum BtnType;

			case _Num:
			{
				info->ZhongZhuiStr += (Data + " ");
				info->HouZhui.emplace_back(Data);

				break;
			}

			case _AdvancedOper:
			{
				info->HouZhui.emplace_back(info->Num);
				break;
			}

			case _BasicOper:
			case _LeftBK:
			case _RightBK:
			{
				info->ZhongZhuiStr += (Data + " ");
				basicOperPushStack(Data);

				break;
			}

			case _Ctrl:
			{
				info->ZhongZhuiStr += (Data + " ");

				break;
			}

			default:
				break;
		}

		if (ClearData)
		{
			Data = "";
		}
	}
}

// 3.1 记录右值数据
void myCalc::takeData(_qstr&& Data, const BtnType& _Type, bool ClearData)
{
	auto tmp { _move(Data) };

	takeData(tmp, _Type, ClearData);
}

// 4.1 最终计算前进行简单检查
bool myCalc::inspectionStr_FinallyCalc(void)
{
	// case1：缺少最后的右括号
	if ((flag->bkFlag >= 1) && (flag->bkFlag <= 8))
	{
		while (flag->bkFlag-- > 0)
		{
			onInspectionStr(_make_ce(")", BtnType::_RightBK));
		}

		return false;
	}
	else
	{
		return true;
	}
}

// 4.2 获取一个高级操作符表达式
_qstr myCalc::getAdvancedStr(const _qstr& Event, const _qstr& keyNum) const noexcept
{
	if (Event == "1/x")
	{
		return _qstr("1/(" + keyNum + ")");
	}

	if (Event == "x²")
	{
		return _qstr("(" + keyNum + ")²");
	}

	if (Event == "√x")
	{
		return _qstr("√(" + keyNum + ")");
	}

	if (Event == "%")
	{
		return _qstr("(" + keyNum + ")%");
	}

	if (Event == "!")
	{
		return _qstr("(" + keyNum + ")!");
	}

	return _qstr("");
}

// 4.3 获取整个表达式的计算结果
_qstr myCalc::calcStr(void) noexcept
{
	while (!info->CalcStack.isEmpty())
	{
		info->HouZhui.emplace_back(info->CalcStack.pop());
	}

	return _move(calcStr(info->HouZhui, info->CalcStack));
}

// 4.4 获取指定表达式的计算结果
_qstr myCalc::calcStr(_def_qvec() & NumsVec, _def_qsk() & OperSk) noexcept
{
	using enum StackErrCode;

	try
	{
		if (NumsVec.isEmpty() || !OperSk.isEmpty())
		{
			throw _Stack_Empty;
		}

		for (auto& i: NumsVec)
		{
			if (isNum(i))
			{
				OperSk.push(i);
			}
			else if (isBasicOper(i))
			{
				if (i == "×")
				{
					i = "*";
				}
				else if (i == "÷")
				{
					i = "/";
				}

				FLOAT OperNum2 = getOperNum(OperSk);
				FLOAT OperNum1 = getOperNum(OperSk);

				if (!flag->calcStackEmpty)
				{
					OperSk.push(_move(simpleCalc(OperNum2, i, OperNum1)));
				}
				else
				{
					throw _Stack_WithoutOperands;
				}
			}
		}

		if (OperSk.isEmpty())
		{
			throw _Stack_Empty;
		}

		if (_cove_type(getFloat(OperSk.top()), LL) > _NUM_MAX)
		{
			throw _Ans_OutOfRange;
		}

		return OperSk.top();
	}
	catch (const StackErrCode errCode)
	{
		if (errCode == _Stack_WithoutOperands)
		{
			return _qstr("StackErr，少操作数");
		}
		else if (errCode == _Stack_Empty)
		{
			return _qstr("操作栈空，无返回值");
		}
		else if (errCode == _Ans_OutOfRange)
		{
			return _qstr("计算结果超出最大值");
		}
		else
		{
			return _qstr("myCalc::calcStr() ERR!");
		}
	}
}

// 4.5 计算括号中的表达式
void myCalc::calcBracketStr(void)
{
	int basicOperNums { 0 };
	for (auto i { info->HouZhui.rbegin() }; i != info->HouZhui.rend(); ++i)
	{
		if (isBasicOper(*i))
		{
			basicOperNums++;
		}
		else
		{
			break;
		}
	}

	const auto strE { 2 * basicOperNums + 1 };
	const auto strB { _cove_type(info->HouZhui.size() - strE, int) };

	_def_qvec() Vec { info->HouZhui.mid(strB, strE) };
	info->HouZhui.remove(strB, strE);

	_def_qsk() Sk {};

	info->HouZhui.emplace_back(_move(calcStr(Vec, Sk)));
}

// 5.1 显示结果
void myCalc::showInfo(const ClickEvent& Event) noexcept
{
	switch (Event.second)
	{
		using enum BtnType;

		case _Num:
		{
			if ((ui.line2->text() == "0") && (Event.first != "."))
			{
				ui.line2->clear();
			}
			else if ((ui.line2->text() != "0") && (info->lastTimeEvent->first != "DEL") &&
					 (info->lastTimeEvent->first != "±") && (info->lastTimeEvent->second != _Num))
			{
				ui.line2->clear();
			}

			if (ui.line2->text().isEmpty() && (Event.first == "."))
			{
				ui.line2->insert("0.");
			}
			else
			{
				formatDisplaying(_move(_move(getDisplayingStr()) + Event.first));
			}

			break;
		}

		case _LeftBK:
		{
			ui.line1->setText(info->ZhongZhuiStr + "  ");
			ui.line2->setText("0");

			break;
		}

		case _RightBK:
		{
			ui.line1->setText(info->ZhongZhuiStr + "  ");
			formatDisplaying(info->HouZhui.back());

			break;
		}

		case _BasicOper:
		{
			if (Event.first == "±")
			{
				formatDisplaying(info->Num);
			}
			else
			{
				ui.line1->setText(info->ZhongZhuiStr + Event.first + "  ");
			}

			break;
		}

		case _AdvancedOper:
		{
			showAdvancedOperInfo(Event);

			break;
		}

		case _Ctrl:
		{
			if (Event.first == "=")
			{
				ui.line1->setText(info->ZhongZhuiStr + "=  ");
				formatDisplaying(info->Num);
			}

			break;
		}

		default:
			break;
	}
}

// 5.2 显示高级操作符所得结果
void myCalc::showAdvancedOperInfo(const ClickEvent& Event)
{
	_qstr keyNum { "" };

	bool rightBK { false };
	if (info->lastTimeEvent->second == BtnType::_AdvancedOper)
	{
		keyNum = info->lastAdvancedOperStr;
	}
	else if (info->lastTimeEvent->first == ")")
	{
		rightBK = true;
		keyNum	= ui.line2->text();
	}
	else
	{
		keyNum = _move(getDisplayingStr());
	}

	if (!(rightBK && !updataZhongZhuiStr()))
	{
		if (const auto& showStr { _move(getAdvancedStr(Event.first, keyNum)) }; !_isEmpty(showStr))
		{
			info->lastAdvancedOperStr = showStr;

			ui.line1->setText(info->ZhongZhuiStr + showStr + "  ");
			formatDisplaying(info->Num);
		}
	}
}

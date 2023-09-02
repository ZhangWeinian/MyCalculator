#pragma once

#include "stdafx.h"

#include "./myCalc_part0.h"

#include <qchar.h>
#include <qlist.h>
#include <qstack.h>
#include <qstring.h>
#include <qcontainerfwd.h>

#include <utility>
#include <string>
#include <version>
#include <memory>
#include <limits.h>
#include <type_traits>

// 按钮类型枚举
const enum class BtnType: Type
{
	_Num = 10,	   // 数字
	_BasicOper,	   // 基础操作符
	_AdvancedOper, // 高级操作符
	_Ctrl,		   // 控制符
	_LeftBK,	   // 左括号
	_RightBK,	   // 右括号
	_EqualTo,	   // 等于号
	_TypeErr,	   // 定义键盘上的未知类型(此未知是对于计算器而言，例如英文字母)
};

// 三个独立显示的按钮类型枚举
const enum class ShowModeSign: Sign
{
	_Basic = 20, // 显示 “( )”，  “CE”，   “C”
	_BCE,		 // 显示 “(”，    “)”，    “CE”
	_BC,		 // 显示 “(”，    “)”，    “C”
	_OnlyBK,	 // 显示 “(”，    “)”，    “none”
	_OnlyCE,	 // 显示 “none”， “CE”，   “none”
	_OnlyC,		 // 显示 “none”， “none”， “C”
};

// 清理类型枚举
const enum class ClearSign: Sign
{
	_All = 30,	// 清除全部
	_OnlyLine2, // 清除未确定的部分
};

// 控制符类型枚举
const enum class CtrlType: Type
{
	_C = 40,   // “C”
	_CE,	   // “CE”
	_DEL,	   // “DEL”
	_EQT,	   // “=”
	_EXIT,	   // “Esc”（来自键盘）
	_CharMode, // 字符模式
	_TypeErr,  // 未知类型
};

// 高级操作符错误类型代码
const enum class AdvancedOperErrCode: Sign
{
	_Num_isNegative = 50, // 负数
	_Num_OutOfRange,	  // 超出范围
	_Num_isZero,		  // 零
};

// 后缀表达式计算错误类型代码
const enum class StackErrCode: Sign
{
	_Stack_WithoutOperands = 60, // 缺少操作数
	_Bracket_Mismatch,			 // 括号不匹配
	_Stack_Empty,				 // 栈异常空
	_Ans_OutOfRange,			 // 超出范围
};

// 需要表达式位置
constexpr BKPosition _NeedPosition = BKPosition(USHRT_MAX, USHRT_MAX);

// 需要基础操作符数量
constexpr int _NeedBasicOperNums = -1;

// 预定义所有基础操作符
const _VEC_QSTR TheBasicOper = { "+", "-", "×", "*", "÷", "/", "( )", "(", ")", "±" };

// 预定义所有高级操作符
const _VEC_QSTR TheAdvancedOper = { "1/x", "x²", "√x", "%", "!", "^" };

// 预定义所有数字
const _VEC(QChar) TheNums = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.' };

// 预定于所有控制符
const _VEC_QSTR TheCtrl = { "CE", "C", "DEL", "=", "EXIT", "#" };

// 预定义运算符优先级
const _VEC(_make_Pair(qstr, Type)) ThePriority = { defPair("+", 0), defPair("-", 0), defPair("*", 1),
												   defPair("×", 1), defPair("/", 1), defPair("÷", 1) };

// 预定义键盘转换字符
const _VEC(_make_Pair(_QT Key, qstr)) TheCorrespondingStr = {
	defPair(_QT Key_Backspace, "DEL"), // 退格键
	defPair(_QT Key_Delete, "DEL"),	   // 删除建
	defPair(_QT Key_Enter, "="),	   // 回车建(小键盘)
	defPair(_QT Key_Return, "="),	   // 回车键(大键盘)
	defPair(_QT Key_Exclam, "!"),	   // 阶乘
	defPair(_QT Key_Percent, "%"),	   // 百分数
	defPair(_QT Key_Plus, "+"),		   // 加
	defPair(_QT Key_Minus, "-"),	   // 减
	defPair(_QT Key_Asterisk, "*"),	   // 乘
	defPair(_QT Key_Space, "*"),	   // 空格(定义为乘号)
	defPair(_QT Key_Slash, "/"),	   // 除
	defPair(_QT Key_ParenLeft, "("),   // 左括号
	defPair(_QT Key_ParenRight, ")"),  // 右括号
	defPair(_QT Key_AsciiCircum, "^"), // 求幂
	defPair(_QT Key_NumberSign, "#"),  // 字符模式
	defPair(_QT Key_Escape, "EXIT"),   // 退出
};

// 预定义键盘转换数字
const _VEC(_make_Pair(_QT Key, qstr)) TheCorrespondingNum = {
	defPair(_QT Key_0, "0"), defPair(_QT Key_1, "1"), defPair(_QT Key_2, "2"),	   defPair(_QT Key_3, "3"),
	defPair(_QT Key_4, "4"), defPair(_QT Key_5, "5"), defPair(_QT Key_6, "6"),	   defPair(_QT Key_7, "7"),
	defPair(_QT Key_8, "8"), defPair(_QT Key_9, "9"), defPair(_QT Key_Period, ".")
};

/*----------------------------------------------------------------------------------------------*/
using ClickEvent = _make_Pair(qstr, BtnType);

#define _make_CE(qstr_text, BtnType_type) ClickEvent(_STD make_pair<qstr, BtnType>((qstr_text), (BtnType_type)))

#define _clear_CE(UPtr)                                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		UPtr.release();                                                                                                \
		UPtr = _init_UPtr_CE();                                                                                        \
                                                                                                                       \
	} while (false)

class BasicInformation
{
public:
	/*--DATA--*/
	defUPtr_CE() thisTimeEvent = _init_UPtr_CE(); // 此次获取的事件
	defUPtr_CE() lastTimeEvent = _init_UPtr_CE(); // 上次获取的事件

	qstr Num = "";				   // 一个完整的数字
	qstr Symbol = "";			   // 一个确定的符号
	qstr lastAns = "";			   // 一次有效的、完整的、按“=”后的计算结果
	qstr lastAdvancedOperStr = ""; // 上一次的高级运算符表达式
	qstr lastAdvancedOper = "";	   // 上一个高级算符

	_VEC_QSTR HouZhui = {};	 // 后缀表达式
	_SK_QSTR CalcStack = {}; // 操作栈

	qstr ZhongZhuiStr = ""; // 中缀表达式（字符串）

	short BasicOperNums = 0; // 括号中的基础操作符计数

	/*--Fun--*/
	_NORETURN BasicInformation(void) = default;

	_NORETURN ~BasicInformation(void) = default;

	_NORETURN void clear(const ClearSign _Sign)
	{
		_clear_CE(thisTimeEvent);

		lastAdvancedOper = "";

		if (_Sign == ClearSign::_All)
		{
			Symbol = "";
			ZhongZhuiStr = "";

			HouZhui = {};
			CalcStack = {};

			lastTimeEvent = _init_UPtr_CE();

			Symbol = "";

			BasicOperNums = 0;
		}

		// 特别的，lastAns不会被清除，除非析构
	}
};

class ImportantFlag
{
public:
	/*--DATA--*/
	short bkFlag = 0; // 括号匹配计数

	bool calcFlag = false;		// 是否完成一次完整的运算
	bool numSignFlag = true;	// 正负号调整。true：调整结束；false：正在调整
	bool operFlag = true;		// 运算符调整。true：调整结束；false：正在调整
	bool numGetOverFlag = true; // 是否获取到一个完整的数字。true；已获取；false：未获取
	bool dPointFlag = false;	// 是否拥有小数点

	bool calcStackEmpty = false; // 后缀运算时，操作栈异常为空：缺少操作数

	bool fromKeyboard = false; // 此次点击来自键盘
	bool fromClick = false;	   // 此次点击来自鼠标

	bool charMode = false; // 是否进入字符模式

	/*--Fun--*/
	_NORETURN ImportantFlag(void) = default;

	_NORETURN ~ImportantFlag(void) = default;

	_NORETURN void clear(const ClearSign _Sign)
	{
		numSignFlag = true;
		numGetOverFlag = true;
		dPointFlag = false;

		if (_Sign == ClearSign::_All)
		{
			bkFlag = 0;

			fromClick = false;
			fromKeyboard = false;

			calcFlag = false;
			operFlag = true;
			calcStackEmpty = false;

			charMode = false;
		}
	}
};

_NODISCARD inline bool operator==(const Sign num1, ClearSign num2)
{
	return cove_ushort(num1) == cove_ushort(num2);
}

_NODISCARD inline bool operator!=(const Sign num1, ClearSign num2)
{
	return cove_ushort(num1) != cove_ushort(num2);
}

_NODISCARD inline bool operator==(const Type num1, BtnType num2)
{
	return cove_ushort(num1) == cove_ushort(num2);
}

_NODISCARD inline bool operator!=(const Type num1, BtnType num2)
{
	return cove_ushort(num1) != cove_ushort(num2);
}

_NODISCARD inline bool operator==(const BtnType num1, Type num2)
{
	return cove_ushort(num1) == cove_ushort(num2);
}

_NODISCARD inline bool operator!=(const BtnType num1, Type num2)
{
	return cove_ushort(num1) != cove_ushort(num2);
}

// _FLOAT 转 qstr
_NODISCARD inline qstr getQstr(const _FLOAT& Num)
{
	string tmp = "";

#if defined(_myFLOAT)
	tmp = cove_type(Num, _STD string);
#else
	tmp = _STD to_string(Num);
#endif

	if (auto dpoint = tmp.find('.'); dpoint != -1)
	{
		while (tmp.back() == '0')
		{
			tmp.pop_back();
		}

		if (tmp.back() == '.')
		{
			tmp.pop_back();
		}
	}

	return qstr::fromStdString(tmp);
}

// qstr 转 _FLOAT
_NODISCARD inline _FLOAT getFloat(const qstr& Str)
{
#if defined(_myFLOAT)
	return cove_type(Str.toStdString(), _FLOAT);
#else
	return _STD stold(Str.toStdString());
#endif
}

// 删除 qstr 的最后一个字符（如果是汉字，则删除最后一个汉字）
_NORETURN inline void pop_back(qstr& Str)
{
	if (auto len = Str.size() - 1; len >= 0)
	{
		Str = _STD move(Str.left(len));
	}
}

// 删除 qstr 的第一个字符（如果是汉字，则删除第一个汉字）
_NORETURN inline void pop_front(qstr& Str)
{
	if (auto len = Str.size() - 1; len >= 0)
	{
		Str = _STD move(Str.right(len));
	}
}

// 删除 qstr 的第一个和最后一个字符
_NORETURN inline void pop_back_front(qstr& Str)
{
	if (Str.size() - 2 >= 0)
	{
		pop_front(Str);
		pop_back(Str);
	}
}

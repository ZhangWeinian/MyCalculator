#pragma once

#include "myCalc_part0.h"
#include <type_traits>

#include <functional>
#include <limits.h>
#include <memory>
#include <qchar.h>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qstack.h>
#include <qstring.h>
#include <string>
#include <utility>
#include <version>


// 按钮类型枚举
const enum class BtnType: Type
{
	_Num = _init_type(10, Type), // 数字
	_BasicOper,					 // 基础操作符
	_AdvancedOper,				 // 高级操作符
	_Ctrl,						 // 控制符
	_LeftBK,					 // 左括号
	_RightBK,					 // 右括号
	_EqualTo,					 // 等于号
	_TypeErr, // 定义键盘上的未知类型(此未知是对于计算器而言，例如英文字母)
};

// 三个独立显示的按钮类型枚举
const enum class ShowModeSign: Sign
{
	_Basic = _init_type(20, Sign), // 显示 “( )”，  “CE”，   “C”
	_BCE,						   // 显示 “(”，    “)”，    “CE”
	_BC,						   // 显示 “(”，    “)”，    “C”
	_OnlyBK,					   // 显示 “(”，    “)”，    “none”
	_OnlyCE,					   // 显示 “none”， “CE”，   “none”
	_OnlyC,						   // 显示 “none”， “none”， “C”
};

// 清理类型枚举
const enum class ClearSign: Sign
{
	_All = _init_type(30, Sign), // 清除全部
	_OnlyLine2,					 // 清除未确定的部分
};

// 控制符类型枚举
const enum class CtrlType: Type
{
	_C = _init_type(40, Type), // “C”
	_CE,					   // “CE”
	_DEL,					   // “DEL”
	_EQT,					   // “=”
	_EXIT,					   // “Esc”（来自键盘）
	_CharMode,				   // 字符模式
	_TypeErr,				   // 未知类型
};

// 高级操作符错误类型代码
const enum class AdvancedOperErrCode: Sign
{
	_Num_isNegative = _init_type(50, Sign), // 负数
	_Num_OutOfRange,						// 超出范围
	_Num_isZero,							// 零
};

// 后缀表达式计算错误类型代码
const enum class StackErrCode: Sign
{
	_Stack_WithoutOperands = _init_type(60, Sign), // 缺少操作数
	_Bracket_Mismatch,							   // 括号不匹配
	_Stack_Empty,								   // 栈异常空
	_Ans_OutOfRange,							   // 超出范围
};

// 需要表达式位置
constexpr BKPosition _NeedPosition { BKPosition(USHRT_MAX, USHRT_MAX) };

// 需要基础操作符数量
constexpr int _NeedBasicOperNums { -1 };

// 预定义所有基础操作符
const _def_qvec() TheBasicOper { "+", "-", "×", "*", "÷", "/", "( )", "(", ")", "±" };

// 预定义所有高级操作符
const _def_qvec() TheAdvancedOper { "1/x", "x²", "√x", "%", "!", "^" };

// 预定义所有数字
const _def_vec(QChar) TheNums { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.' };

// 预定于所有控制符
const _def_qvec() TheCtrl { "CE", "C", "DEL", "=", "EXIT", "#" };

// 预定义运算符优先级
const _def_vec(_make_Pair(_qstr, Type)) ThePriority { _def_pair("+", 0), _def_pair("-", 0), _def_pair("*", 1),
													  _def_pair("×", 1), _def_pair("/", 1), _def_pair("÷", 1) };

// 预定义键盘转换字符
const _def_vec(_make_Pair(_QT Key, _qstr)) TheCorrespondingStr {
	_def_pair(_QT Key_Backspace, "DEL"), // 退格键
	_def_pair(_QT Key_Delete, "DEL"),	 // 删除建
	_def_pair(_QT Key_Enter, "="),		 // 回车建(小键盘)
	_def_pair(_QT Key_Return, "="),		 // 回车键(大键盘)
	_def_pair(_QT Key_Exclam, "!"),		 // 阶乘
	_def_pair(_QT Key_Percent, "%"),	 // 百分数
	_def_pair(_QT Key_Plus, "+"),		 // 加
	_def_pair(_QT Key_Minus, "-"),		 // 减
	_def_pair(_QT Key_Asterisk, "*"),	 // 乘
	_def_pair(_QT Key_Space, "*"),		 // 空格(定义为乘号)
	_def_pair(_QT Key_Slash, "/"),		 // 除
	_def_pair(_QT Key_ParenLeft, "("),	 // 左括号
	_def_pair(_QT Key_ParenRight, ")"),	 // 右括号
	_def_pair(_QT Key_AsciiCircum, "^"), // 求幂
	_def_pair(_QT Key_NumberSign, "#"),	 // 字符模式
	_def_pair(_QT Key_Escape, "EXIT"),	 // 退出
};

// 预定义键盘转换数字
const _def_vec(_make_Pair(_QT Key, _qstr)) TheCorrespondingNum {
	_def_pair(_QT Key_0, "0"), _def_pair(_QT Key_1, "1"), _def_pair(_QT Key_2, "2"),	 _def_pair(_QT Key_3, "3"),
	_def_pair(_QT Key_4, "4"), _def_pair(_QT Key_5, "5"), _def_pair(_QT Key_6, "6"),	 _def_pair(_QT Key_7, "7"),
	_def_pair(_QT Key_8, "8"), _def_pair(_QT Key_9, "9"), _def_pair(_QT Key_Period, ".")
};

/*----------------------------------------------------------------------------------------------*/
using ClickEvent = _make_Pair(_qstr, BtnType);

#define _make_ce(qstr_text, BtnType_type) ClickEvent(::std::make_pair<_qstr, BtnType>((qstr_text), (BtnType_type)))

#define _clear_ce(UPtr) UPtr.reset(new ClickEvent)

class BasicInformation
{
public:

	/*--DATA--*/
	_def_ce() thisTimeEvent { _init_ce() }; // 此次获取的事件
	_def_ce() lastTimeEvent { _init_ce() }; // 上次获取的事件

	_qstr Num { "" };						// 一个完整的数字
	_qstr Symbol { "" };					// 一个确定的符号
	_qstr lastAns { "" };					// 一次有效的、完整的、按“=”后的计算结果
	_qstr lastAdvancedOper { "" };			// 上一个高级算符
	_qstr lastAdvancedOperStr { "" };		// 上一次的高级运算符表达式

	_def_qvec() HouZhui {};					// 后缀表达式
	_def_qsk() CalcStack {};				// 操作栈

	_qstr ZhongZhuiStr { "" };				// 中缀表达式（字符串）

	short BasicOperNums { 0 };				// 括号中的基础操作符计数

	/*--Fun--*/
	_NORETURN BasicInformation(void) = default;

	_NORETURN ~BasicInformation(void) = default;

	_NORETURN void clear(const ClearSign _Sign) noexcept
	{
		_clear_ce(thisTimeEvent);

		lastAdvancedOper = "";

		if (_Sign == ClearSign::_All)
		{
			Symbol		  = "";
			ZhongZhuiStr  = "";
			BasicOperNums = 0;

			HouZhui	  = {};
			CalcStack = {};

			lastTimeEvent = _init_ce();
		}

		// 特别的，lastAns不会被清除，除非析构
	}
};

class ImportantFlag
{
public:

	/*--DATA--*/
	unsigned short bkFlag = 0;	   // 括号匹配计数

	bool calcFlag { false };	   // 是否完成一次完整的运算
	bool operFlag { true };		   // 运算符调整。true：调整结束；false：正在调整
	bool dPointFlag { false };	   // 是否拥有小数点。true：有；false：无
	bool numSignFlag { true };	   // 正负号调整。true：调整结束；false：正在调整
	bool numGetOverFlag { true };  // 是否获取到一个完整的数字。true；已获取；false：未获取

	bool calcStackEmpty { false }; // 后缀运算时，操作栈异常为空：缺少操作数

	bool fromKeyboard { false };   // 此次点击来自键盘
	bool fromClickEvt { false };   // 此次点击来自鼠标

	bool charMode { false };	   // 是否进入字符模式

	bool haveDecimal { false };	   // 有小数点时，标准数字处理只执行一次
	bool getLastAns { false };	   // 是否已经到了获取最终答案的时刻

	/*--Fun--*/
	_NORETURN ImportantFlag(void) = default;

	_NORETURN ~ImportantFlag(void) = default;

	_NORETURN void clear(const ClearSign _Sign) noexcept
	{
		numSignFlag	   = true;
		numGetOverFlag = true;
		dPointFlag	   = false;
		haveDecimal	   = false;

		if (_Sign == ClearSign::_All)
		{
			bkFlag = 0;

			fromClickEvt = false;
			fromKeyboard = false;
			charMode	 = false;
			getLastAns	 = false;

			calcFlag	   = false;
			operFlag	   = true;
			calcStackEmpty = false;
		}
	}
};

_NODISCARD inline bool operator==(const Sign num1, ClearSign num2) noexcept
{
	return _cove_ushort(num1) == _cove_ushort(num2);
}

_NODISCARD inline bool operator==(const Type num1, BtnType num2) noexcept
{
	return _cove_ushort(num1) == _cove_ushort(num2);
}

// _FLOAT 转 _qstr
_NODISCARD inline _qstr getQstr(const FLOAT& Num) noexcept
{
	_str tmp { _move(_STD to_string(Num)) };

	if (auto dpoint { tmp.find('.') }; dpoint != -1)
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

	return _qstr::fromStdString(tmp);
}

_NODISCARD inline void getQstr(const FLOAT& Num, _qstr& ans) noexcept
{
	ans = _move(getQstr(Num));
}

// _qstr 转 _FLOAT
_NODISCARD inline FLOAT getFloat(const _qstr& Str) noexcept
{
	return stold(Str.toStdString());
}

_NODISCARD inline void getFloat(const _qstr& Str, FLOAT& ans) noexcept
{
	ans = _move(getFloat(Str));
}

// 删除 _qstr 的最后一个字符（如果是汉字，则删除最后一个汉字）
_NORETURN inline void pop_back(_qstr& Str) noexcept
{
	if (auto len { Str.size() - 1 }; len >= 0)
	{
		Str = _move(Str.left(len));
	}
}

// 删除 _qstr 的第一个字符（如果是汉字，则删除第一个汉字）
_NORETURN inline void pop_front(_qstr& Str) noexcept
{
	if (auto len { Str.size() - 1 }; len >= 0)
	{
		Str = _move(Str.right(len));
	}
}

// 删除 _qstr 的第一个和最后一个字符
_NORETURN inline void pop_back_front(_qstr& Str) noexcept
{
	if (Str.size() - 2 >= 0)
	{
		pop_front(Str);
		pop_back(Str);
	}
}

// _qstr 反转
_NORETURN inline void reverse_qstr(_qstr& Str) noexcept
{
	_qstr ans { "" };
	_RG	  for_each(Str.rbegin(), Str.rend(), [&ans](const auto& i) { ans += i; });

	Str = _move(ans);
}

// isEmpty()
_NODISCARD inline bool _isEmpty(const _qstr& str) noexcept
{
	return str.size() == 0;
}

_NODISCARD inline bool _isEmpty(_str&& str) noexcept
{
	return _isEmpty(_move(str));
}

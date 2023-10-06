#pragma once

#include "./myCalc_part1.h"
#include <ui_myCalc.h>

#include <qabstractbutton.h>
#include <qbuttongroup.h>
#include <qevent.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class myCalc: public QWidget
{
	Q_OBJECT

public:

	_NORETURN myCalc(QWidget* parent = nullptr);
	_NORETURN ~myCalc(void);

private:

	/*--Data--*/
	Ui::myCalcClass ui;

	_def_uptr(BasicInformation) info;	 // 基础数据集合
	_def_uptr(ImportantFlag) flag;		 // 计算过程中的重要标识符
	_def_uptr(QButtonGroup) buttonGroup; // 存储按钮的组

	// _def_uptr(myCalc_CharMode) charModeWindow; // 字符模式

	/*--Fun--*/
	_NORETURN void initUI(void);   // 基本初始化
	_NORETURN void initData(void); // 数据初始化
	_NORETURN void initFun(void);  // 函数初始化

	// 监听键盘
	_NORETURN virtual void keyReleaseEvent(QKeyEvent* KeyEvent);

	// 部分键盘文本转换
	_NODISCARD _qstr getKeyboardText(const QKeyEvent* KeyEvent);

	// 特殊按钮的显示
	_NORETURN void showMode(const ShowModeSign& _Sign);

	// 分别判断是否是操作符、数字、控制符。返回 bool 类型的判断结果：是--true、否--false
	_NODISCARD bool isBasicOper(const _qstr& BasicOperEvent) const;
	_NODISCARD bool isAdvancedOper(const _qstr& AdvancedOperEvent) const;
	_NODISCARD bool isNum(const _qstr& NumEvent) const;
	_NODISCARD bool isCtrl(const _qstr& CtrlEvent) const;

	// 判断来自键盘的事件是否应该相应
	_NODISCARD bool isCorresponding(const int QtKey) const;

	// 分别返回事件、控制操作符、当前显示内容的类型
	_NODISCARD BtnType		getBtnType(const _qstr& BtnEvent) const;
	_NODISCARD CtrlType		getCtrlType(const _qstr& CtrlEvent) const;
	_NODISCARD ShowModeSign getBtnShowType(void) const;

	// 获取键盘事件转换后的文本或数字
	_NODISCARD _qstr getCorrespondingStr(const int StrKey) const;
	_NODISCARD _qstr getCorrespondingNum(const int NumKey) const;

	// 获取基础操作符优先级
	_NODISCARD bool getPriority(const _qstr& Oper1, const _qstr& Oper2) const;

	// 分别处理数字、基础操作符、高级操作符、括号、控制符、删除键
	void		   handleNum(const _qstr& NumEvent);
	void		   handleBasicOper(const _qstr& BasicOperEvent);
	_NORETURN void handleAdvancedOper(const _qstr& AdvancedOperEvent);
	_NORETURN void handleBkEvent(const _qstr& BkEvent);
	_NORETURN void handleCtrl(const CtrlType& _Type);
	_NORETURN void handleDelete(void);

	// 处理正负号。定义：true--可以继续；false--结束基础操作符处理
	_NODISCARD bool handleNumSign(void);

	// 基本操作符入栈
	void basicOperPushStack(const _qstr& BasicOperEvent);

	// 高级操作符计算
	_NODISCARD _qstr calcAdvancedOperData(const _qstr& Data, const _qstr& Oper) const;

	// 最终计算前进行简单检查
	_NORETURN void inspectionStr_FinallyCalc(void);

	// 输入时检查是否合理
	_NORETURN void inspectionStr_InputTime(const ClickEvent& ThisTimeEvent);

	// 获取计算结果
	_NODISCARD _qstr calcStr(void);
	_NODISCARD _qstr calcStr(_def_vec_qstr& NumsVec, _def_sk_qstr& OperSk);

	// 获取当前显示在输入栏中的文本
	_NODISCARD _qstr getDisplayingStr(void) const;

	// 输入一个“）”时，计算一个最小的、完整括号表达式的值
	void calcBracketStr(void);

	// 获取CalcStack栈顶的操作数
	_NODISCARD _FLOAT getOperNum(_def_sk_qstr& OperSk);

	// 执行单操作符的简单运算
	_NODISCARD _qstr simpleCalc(const _FLOAT& OperNum2, const _qstr& OperSymbol, const _FLOAT& OperNum1) const;

	// 根据案件内容的类型在line1，line2中显示数据
	_NORETURN void showInfo(const ClickEvent& Event);

	// 标准数字显示
	_NORETURN void formatDisplaying(const _qstr& Num = "");

	// 清理数字中的逗号
	_NODISCARD _qstr clearCommas(const _qstr& Str) const;

	// 高级操作符直接显示答案
	void showAdvancedOperInfo(const ClickEvent& Event);

	// 获取一个高级操作符表达式
	_NODISCARD _qstr getAdvancedStr(const _qstr& Event, const _qstr& keyNum) const;

	// 配合高级操作符，更新显示文本。定义：true-更新成功；false--更新失败
	_NODISCARD bool updataZhongZhuiStr(void);

	// 寻找最右边的、完整的括号表达式
	_NODISCARD BKPosition findBracketStr(const BKPosition&);

	// 记录数据、清除数据
	void		   takeData(_qstr&& Data, const BtnType& _Type, bool ClearData = true);
	void		   takeData(_qstr& Data, const BtnType& _Type, bool ClearData = true);
	_NORETURN void clear(const ClearSign& _Sign);
	_NORETURN void clearLastAdvancedStr(void);

	// 防止重复记录。定义：true--可以记录，继续；flase--重复记录，跳过
	_NODISCARD bool dontDuplicateRecord(const ClickEvent& Event) const;

	// line2 只能显示数字
	_NORETURN void setLine2Displaying(void);

	// 预处理
	_NORETURN void Preprocessing(const ClickEvent& Event);

	// 正式处理事件
	_NORETURN void handleEvent(const ClickEvent& Event);

	// 获取阶乘
	_NODISCARD _FLOAT getFactorial(const _qstr& Data) const;

private slots:

	// 槽
	_NORETURN void onButtonGroupCliked(const QAbstractButton* Btn);

	// 为了处理异常自主添加的事件
	_NORETURN void onInspectionStr(const ClickEvent& AddEvent);
};

#pragma once

#include "./HighPrecisionOperations.h"
#include "stdafx.h"

#include <limits.h>
#include <qcontainerfwd.h>
#include <qstack.h>
#include <qstring.h>
#include <string>
#include <version>



#ifndef _STD
	#define _STD ::std::
#endif // !_STD

#ifndef _QT
	#define _QT ::Qt::
#endif // !_QT

#ifndef _LOG
	#define _LOG ::spdlog::
#endif // !_LOG

#ifndef _NORETURN
	#define _NORETURN [[noreturn]]
#endif // !_NORETURN

#ifndef _NODISCARD
	#define _NODISCARD [[nodiscard]]
#endif // !_NODISCARD

#ifndef _NODISCARD_MSG
	#define _NODISCARD_MSG(_msg) [[nodiscard(_msg)]]
#endif // !_NODISCARD_MSG

#ifndef LL
using LL = long long;
#endif // !LL

#ifndef ULL
using ULL = unsigned long long;
#endif // !ULL


using Type = unsigned _int16;
using Sign = unsigned _int16;


#if defined(_myINT)
using _INT = _myINT;
#else
using _INT	 = _int64;
#endif


#if defined(_myFLOAT)
using _FLOAT = _myFLOAT;
#else
using _FLOAT = long double;
#endif


using _str		 = _STD string;
using _qstr		 = QString;
using _con_qstr	 = const QString;
using BKPosition = QPair<size_t, size_t>;


#define _def_sk(data_type)	QStack<data_type>
#define _def_sk_qstr		_def_sk(_qstr)
#define _def_vec(data_type) QVector<data_type>
#define _def_vec_qstr		_def_vec(_qstr)


constexpr Type _PUBINFO_ERROR = USHRT_MAX;

constexpr LL _NUM_MAX = LLONG_MAX;
constexpr LL _NUM_MIN = LLONG_MIN;

/*----------------------------------------------------------------------------------------------*/


#define _def_uptr(PtrType) _STD unique_ptr<PtrType>
#define _def_uptr_ce()	   _STD unique_ptr<ClickEvent>

#define _def_pair(type1, type2)	 _STD make_pair(type1, type2)
#define _make_Pair(type1, type2) _STD pair<type1, type2>

#define _cove_ushort(num) static_cast<unsigned short>(num)

#ifndef _myFLOAT
	#define _cove_type(content, type) static_cast<type>(content)
#else
	#define _cove_type(content, type) _BOOST lexical_cast<type>(content)
#endif // !_myFLOAT

#define _init_type(init_content, init_type) _cove_type(init_content, init_type)
#define _init_uptr(CE_Type, ...)			_STD make_unique<CE_Type>(##__VA_ARGS__)
#define _init_uptr_ce(...)					_STD make_unique<ClickEvent>(##__VA_ARGS__)

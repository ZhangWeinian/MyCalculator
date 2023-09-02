#pragma once

#include "stdafx.h"
#include "./HighPrecisionOperations.h"

#include <qcontainerfwd.h>
#include <qstack.h>
#include <qstring.h>

#include <version>
#include <limits.h>
#include <string>

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
using _INT = _int64;
#endif

#if defined(_myFLOAT)
using _FLOAT = _myFLOAT;
#else
using _FLOAT = long double;
#endif

using string = _STD string;
using qstr = QString;
using BKPosition = QPair<size_t, size_t>;

#define _SK(data_type) QStack<data_type>
#define _SK_QSTR _SK(qstr)
#define _VEC(data_type) QVector<data_type>
#define _VEC_QSTR _VEC(qstr)

constexpr Type _PUBINFO_ERROR = USHRT_MAX;

constexpr LL _NUM_MAX = LLONG_MAX;
constexpr LL _NUM_MIN = LLONG_MIN;

/*----------------------------------------------------------------------------------------------*/

#define defUPtr(PtrType) _STD unique_ptr<PtrType>
#define defUPtr_CE() _STD unique_ptr<ClickEvent>

#define defPair(type1, type2) _STD make_pair(type1, type2)

#define _make_Pair(type1, type2) _STD pair<type1, type2>

#define _init_UPtr(CE_Type, ...) _STD make_unique<CE_Type>(##__VA_ARGS__)
#define _init_UPtr_CE(...) _STD make_unique<ClickEvent>(##__VA_ARGS__)

#define cove_ushort(num) static_cast<unsigned short>(num)

#ifndef _myFLOAT
#define cove_type(content, type) static_cast<type>(content)
#else
#define cove_type(content, type) _BOOST lexical_cast<type>(content)
#endif // !_myFLOAT

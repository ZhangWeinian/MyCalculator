#pragma once

#include <limits.h>
#include <qcontainerfwd.h>
#include <qstack.h>
#include <qstring.h>
#include <ranges>
#include <string>
#include <version>


#ifndef _STD
	#define _STD ::std::
#endif // !_STD

#ifndef _RG
	#define _RG ::std::ranges::
	#define _VI ::std::ranges::views::
#endif // !_RG

#ifndef _QT
	#define _QT ::Qt::
#endif // !_QT


#ifndef _NORETURN
	#define _NORETURN [[noreturn]]
#endif // !_NORETURN

#ifndef _NODISCARD
	#define _NODISCARD [[nodiscard]]
#endif // !_NODISCARD

#ifndef _NODISCARD_MSG
	#define _NODISCARD_MSG(_msg) [[nodiscard(_msg)]]
#endif // !_NODISCARD_MSG



using Type = unsigned _int16;
using Sign = unsigned _int16;

using LL	= long long;
using ULL	= unsigned long long;
using INT	= long;
using FLOAT = long double;

using _str		 = ::std::string;
using _qstr		 = QString;
using BKPosition = QPair<size_t, size_t>;

constexpr Type _PUBINFO_ERROR = USHRT_MAX;
constexpr LL   _NUM_MAX		  = LLONG_MAX;
constexpr LL   _NUM_MIN		  = LLONG_MIN;

/*----------------------------------------------------------------------------------------------*/

#define _def_stack(data_type) QStack<data_type>
#define _def_qsk()			  QStack<_qstr>
#define _def_vec(data_type)	  QVector<data_type>
#define _def_qvec()			  QVector<_qstr>


#define _def_uptr(PtrType)		::std::unique_ptr<PtrType>
#define _def_ce()				::std::unique_ptr<ClickEvent>
#define _def_pair(type1, type2) ::std::make_pair(type1, type2)

#define _make_Pair(type1, type2) ::std::pair<type1, type2>

#define _cove_ushort(num)		  static_cast<unsigned short>(num)
#define _cove_type(content, type) static_cast<type>(content)

#define _init_type(init_content, init_type) static_cast<init_type>(init_content)
#define _init_uptr(CE_Type, ...)			::std::make_unique<CE_Type>(##__VA_ARGS__)
#define _init_ce(...)						::std::make_unique<ClickEvent>(##__VA_ARGS__)

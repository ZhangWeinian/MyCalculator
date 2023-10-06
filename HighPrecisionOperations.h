#pragma once

#define _use_BOOST
#undef _use_BOOST

#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/detail/et_ops.hpp>
#include <boost/multiprecision/detail/number_compare.hpp>
#include <boost/multiprecision/number.hpp>

#ifndef _BOOST
	#define _BOOST ::boost::
#endif // !_BOOST

#ifndef _MATH
	#define _MATH _BOOST math::
#endif // !_BOOST_MATH

#ifndef _MUL
	#define _MUL _BOOST multiprecision::
#endif // !_MUL

#ifdef _use_BOOST

	#ifndef _myINT
		#define _myINT _MUL cpp_int
	#endif // !_myINT

	#ifndef _myFLOAT
		#define _myFLOAT _MUL cpp_dec_float_100
	#endif // !_myFLOAT

#endif	   // _use_BOOST


#pragma once

#include <unordered_map>
#include "boost/optional.hpp"
#include "boost/preprocessor/comparison/equal.hpp"
#include "boost/preprocessor/stringize.hpp"
#include "boost/preprocessor/tuple/rem.hpp"
#include "boost/preprocessor/seq/for_each.hpp"
#include "boost/preprocessor/wstringize.hpp"

#if defined(_UNICODE)
#define BOOST_PP_TSTRINGIZE(text)   BOOST_PP_WSTRINGIZE(text)
#else
#define BOOST_PP_TSTRINGIZE(text)   BOOST_PP_STRINGIZE(text)
#endif

#define BELU_ENUM_NAME(elem)	BOOST_PP_TUPLE_ELEM(2, 0, elem)
#define BELU_ENUM_VALUE(elem)	BOOST_PP_TUPLE_ELEM(2, 2, elem)
#define BELU_ENUM_MACRO(r, name, elem)	BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(elem), 2), BOOST_PP_TUPLE_ELEM(0, elem) = BOOST_PP_TUPLE_ELEM(1, elem), BOOST_PP_TUPLE_ELEM(0, elem)),
#define BELU_ENUM_CASE_MACRO(r, name, elem)	case name::BELU_ENUM_NAME(elem): return BOOST_PP_STRINGIZE(BELU_ENUM_NAME(elem));
#define BELU_ENUM_PAIR_MACRO(r, name, elem)	{ BOOST_PP_STRINGIZE(BELU_ENUM_NAME(elem)), name::BELU_ENUM_NAME(elem) },

/*
BELU_ENUM(Name, int,
((None))
((Test, 2))
)
*/
#define BELU_ENUM(Name, Type, SEQ)\
enum class Name : Type\
{\
	BOOST_PP_SEQ_FOR_EACH(BELU_ENUM_MACRO, _, SEQ)\
	MAX\
};\
template<class T> class Enum;\
template<> class Enum<Name>\
{\
public:\
	static constexpr size_t COUNT = BOOST_PP_SEQ_SIZE(SEQ);\
	static const char* ToString(Name e)\
	{\
		switch (e)\
		{\
		BOOST_PP_SEQ_FOR_EACH(BELU_ENUM_CASE_MACRO, Name, SEQ)\
		}\
		return ("");\
	}\
	static boost::optional<Name> From(const std::string& str)\
	{\
		static const std::unordered_map<std::string, Name> __str2values = {\
			BOOST_PP_SEQ_FOR_EACH(BELU_ENUM_PAIR_MACRO, Name, SEQ)\
		};\
		auto itr = __str2values.find(str);\
		return ((itr != __str2values.end()) ?\
			boost::optional<Name>(itr->second) : boost::none);\
	}\
	static boost::optional<Name> From(const char* str)\
	{\
		return From(std::string(str));\
	}\
}

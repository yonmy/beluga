#pragma once

#include <tuple>
#include "boost/optional.hpp"
#include "fmt/format.h"

namespace beluga
{
namespace redis
{

class BulkStringBuilder
{
public:
	static std::string make(const std::string& param) noexcept
	{
		return fmt::format("${}\r\n{}\r\n", param.length(), param);
	}

	static std::string make(const char* param) noexcept
	{
		return (param ?
			fmt::format("${}\r\n{}\r\n", ::strlen(param), param) :
			make(nullptr));
	}

	static std::string make(const std::nullptr_t&) noexcept
	{
		return std::string("$-1\r\n");
	}

	template<class T>
	static std::string make(const T& param) noexcept
	{
		return make(fmt::format("{}", param));
	}

	template<class T>
	static std::string make(const T* param) noexcept
	{
		return (param ? make(*param) : make(nullptr));
	}

	template<class T>
	static std::string make(const boost::optional<T>& param) noexcept
	{
		return (param.is_initialized() ? make(param.get()) : make(nullptr));
	}
};

template<class... T>
class BulkStringArrayBuilder
{
public:
	using Tuple = std::tuple<T...>;
	static constexpr size_t MAX_INDEX = sizeof...(T)-1;

	static std::string make(const Tuple& t) noexcept
	{
		return fmt::format("*{}\r\n{}", sizeof...(T),
			make(t, std::integral_constant<size_t, 0>()));
	}

private:
	template<size_t INDEX>
	static std::string make(const Tuple& t, std::integral_constant<size_t, INDEX>) noexcept
	{
		return make_bulkstring<std::tuple_element<INDEX, Tuple>::type>(std::get<INDEX>(t))
			+ make(t, std::integral_constant<size_t, INDEX + 1>());
	}

	template<>
	static std::string make(const Tuple& t, std::integral_constant<size_t, MAX_INDEX>) noexcept
	{
		return make_bulkstring<std::tuple_element<MAX_INDEX, Tuple>::type>(std::get<MAX_INDEX>(t));
	}
};

template<class T>
std::string make_bulkstring(const T& param) noexcept
{
	return BulkStringBuilder::make(param);
}

template<class... T>
std::string make_bulkstring_array(T&&... params) noexcept
{
	return BulkStringArrayBuilder<typename std::_Unrefwrap<T>::type...>::make(
		std::make_tuple(std::forward<T>(params)...));
}

}
}

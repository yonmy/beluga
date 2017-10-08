#pragma once

#include <vector>
#include "boost/lexical_cast.hpp"
#include "boost/optional.hpp"
#include "boost/variant.hpp"

namespace beluga
{
namespace redis
{

// RESP protocol description
class Resp
{
public:
	using TValue = boost::variant<
		boost::optional<std::string>	// bulk/simple string
		, __int64						// integer
		, std::vector<Resp>				// array
		, std::string					// error string
	>;

public:
	Resp() = default;

	Resp(const Resp& rhs)
		: _value(rhs._value)
	{
	}

	Resp(Resp&& rhs)
		: _value(std::move(rhs._value))
	{
	}

	template<class T>
	Resp(const T& value)
		: _value(value)
	{
	}

	Resp& operator=(const Resp& rhs)
	{
		_value = rhs._value;
		return *this;
	}

	Resp& operator=(Resp&& rhs)
	{
		_value = std::move(rhs._value);
		return *this;
	}

	template<class T>
	Resp& operator=(const T& value)
	{
		_value = value;
		return *this;
	}

	template<class T>
	Resp& operator=(Resp&& value)
	{
		_value = std::move(value);
		return *this;
	}

	void clear()
	{
		_value = decltype(_value)();
	}

	void swap(Resp& rhs)
	{
		swap(rhs._value);
	}

	void swap(TValue& rhs)
	{
		_value.swap(rhs);
	}

private:
	template<class T> friend const typename T* get(const Resp&);
	TValue _value;
};

template<class T>
const typename T* get(const Resp& resp)
{
	return boost::get<T>(&resp._value);
}

}
}

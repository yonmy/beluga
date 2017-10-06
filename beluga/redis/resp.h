#pragma once

#include <vector>
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
	Resp() = default;

	template<class T>
	Resp(const T& value)
		: _value(value)
	{
	}

	Resp(const Resp& rhs)
		: _value(rhs._value)
	{
	}

	Resp(Resp&& rhs)
		: _value(std::move(rhs._value))
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

	void clear()
	{
		_value = boost::optional<std::string>();
	}

	void swap(Resp& rhs)
	{
		_value.swap(rhs._value);
	}

	template<class T>
	T* get()
	{
		return boost::get<T>(&_value);
	}

	template<class T>
	const T* get() const
	{
		return boost::get<T>(&_value);
	}

private:
	boost::variant<
		boost::optional<std::string>
		, __int64
		, std::string
		, std::vector<Resp>
	> _value;
};

}
}

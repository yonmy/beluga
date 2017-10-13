#pragma once

#include <list>
#include <vector>
#include "boost/lexical_cast.hpp"
#include "boost/optional.hpp"
#include "boost/variant.hpp"
#include "beluga/core/function_traits.h"

namespace beluga
{
namespace redis
{

// RESP protocol description
class Resp
{
public:
	static constexpr char SYMBOL_BULKSTRING = '$';
	static constexpr char SYMBOL_ARRAY = '*';
	static constexpr char SYMBOL_SIMPLESTRING = '+';
	static constexpr char SYMBOL_ERROR = '-';
	static constexpr char SYMBOL_INTEGER = ':';

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

	const auto& get() const noexcept { return _value; }

	const char* error() const noexcept
	{
		if (const auto p = boost::get<std::string>(&_value))
		{
			return p->c_str();
		}
		return nullptr;
	}

private:
	TValue _value;
};

template<class T>
const typename T* get(const Resp& resp)
{
	return boost::get<T>(&resp.get());
}

template<class T>
struct RespValueVisitor : public boost::static_visitor<T>
{
	template<class T2>
	T operator()(const T2&) const noexcept
	{
		return T();
	}

	template<>
	T operator()(const boost::optional<std::string>& value) const noexcept
	{
		if (value.is_initialized())
		{
			try
			{
				return boost::lexical_cast<T>(value.get());
			}
			catch (boost::bad_lexical_cast)
			{
			}
		}
		return T();
	}

	template<>
	T operator()(const __int64& value) const noexcept
	{
		return static_cast<T>(value);
	}
};

template<>
struct RespValueVisitor<boost::optional<std::string>>
	: public boost::static_visitor<const boost::optional<std::string>&>
{
	template<class T>
	const auto& operator()(const T&) const noexcept
	{
		static const boost::optional<std::string> DEFAULT_VALUE;
		return DEFAULT_VALUE;
	}

	template<>
	const auto& operator()(const boost::optional<std::string>& value) const noexcept
	{
		return value;
	}
};

template<>
struct RespValueVisitor<const std::string*>
	: public boost::static_visitor<const std::string*>
{
	template<class T>
	const std::string* operator()(const T&) const noexcept
	{
		return nullptr;
	}

	template<>
	const std::string* operator()(const boost::optional<std::string>& value) const noexcept
	{
		return value.get_ptr();
	}
};

template<>
struct RespValueVisitor<std::string>
	: public boost::static_visitor<const std::string&>
{
	template<class T>
	const auto& operator()(const T&) const noexcept
	{
		static const std::string DEFAULT_VALUE;
		return DEFAULT_VALUE;
	}

	template<>
	const auto& operator()(const boost::optional<std::string>& value) const noexcept
	{
		static const std::string DEFAULT_VALUE;
		return (value.is_initialized() ? value.get() : DEFAULT_VALUE);
	}
};

template<>
struct RespValueVisitor<const char*>
	: public boost::static_visitor<const char*>
{
	template<class T>
	const char* operator()(const T&) const noexcept
	{
		return nullptr;
	}

	template<>
	const char* operator()(const boost::optional<std::string>& value) const noexcept
	{
		return (value.is_initialized() ? value.get().c_str() : nullptr);
	}
};

template<class T, size_t SIZE>
struct RespValueVisitor<std::array<T, SIZE>> : public boost::static_visitor<std::array<T, SIZE>>
{
	template<class T2>
	std::array<T, SIZE> operator()(const T2& value) const noexcept
	{
		return std::array<T, SIZE>{ RespValueVisitor<T>()(value) };
	}

	template<>
	std::array<T, SIZE> operator()(const std::vector<Resp>& value) const noexcept
	{
		std::array<T, SIZE> cont;
		for (size_t i = 0; i < cont.size(); ++i)
		{
			if (i < value.size())
			{
				cont[i] = boost::apply_visitor(RespValueVisitor<T>(), value[i].get());
			}
			else
			{
				cont[i] = T();
			}
		}
		return std::move(cont);
	}

	template<>
	std::array<T, SIZE> operator()(const std::string&) const noexcept
	{
		return std::array<T, SIZE>();
	}
};

template<class T>
struct RespValueVisitor<std::list<T>> : public boost::static_visitor<std::list<T>>
{
	template<class T2>
	std::list<T> operator()(const T2& value) const noexcept
	{
		return std::list<T>{ RespValueVisitor<T>()(value) };
	}

	template<>
	std::list<T> operator()(const std::vector<Resp>& value) const noexcept
	{
		std::list<T> cont;
		for (const auto& resp : value)
		{
			cont.push_back(
				boost::apply_visitor(RespValueVisitor<T>(), resp.get()));
		}
		return std::move(cont);
	}

	template<>
	std::list<T> operator()(const std::string&) const noexcept
	{
		return std::list<T>();
	}
};

template<class T>
struct RespValueVisitor<std::vector<T>> : public boost::static_visitor<std::vector<T>>
{
	template<class T2>
	std::vector<T> operator()(const T2& value) const noexcept
	{
		return std::vector<T>{ RespValueVisitor<T>()(value) };
	}

	template<>
	std::vector<T> operator()(const std::vector<Resp>& value) const noexcept
	{
		std::vector<T> cont;
		cont.reserve(value.size());
		for (const auto& resp : value)
		{
			cont.push_back(
				boost::apply_visitor(RespValueVisitor<T>(), resp.get()));
		}
		return std::move(cont);
	}

	template<>
	std::vector<T> operator()(const std::string&) const noexcept
	{
		return std::vector<T>();
	}
};

template<class... T>
struct RespValueVisitor<std::tuple<T...>> : public boost::static_visitor<std::tuple<T...>>
{
	template<class T2>
	std::tuple<T...> operator()(const T2& value) const noexcept
	{
		std::tuple<T...> t;
		std::get<0>(t) = RespValueVisitor<std::tuple_element_t<0, std::tuple<T...>>>()(value);
		return std::move(t);
	}

	template<>
	std::tuple<T...> operator()(const std::vector<Resp>& value) const noexcept
	{
		std::tuple<T...> t;
		apply(t, value, std::integral_constant<size_t, 0>());
		return std::move(t);
	}

	template<size_t INDEX>
	static void apply(std::tuple<T...>& t, const std::vector<Resp>& value, std::integral_constant<size_t, INDEX>) noexcept
	{
		if (INDEX < value.size())
		{
			std::get<INDEX>(t) = boost::apply_visitor(RespValueVisitor<std::tuple_element_t<INDEX, std::tuple<T...>>>(), value[INDEX].get());
			apply(t, value, std::integral_constant<size_t, INDEX + 1>());
		}
	}

	template<>
	static void apply(std::tuple<T...>&, const std::vector<Resp>&, std::integral_constant<size_t, sizeof...(T)>) noexcept
	{
	}

	template<>
	std::tuple<T...> operator()(const std::string&) const noexcept
	{
		return std::tuple<T...>();
	}
};

template<class T>
void visitor(const Resp& resp, const T& callback)
{
	using TArg = std::remove_const_t<std::remove_reference_t<FunctionTraits<T>::TArg<0>>>;
	callback(boost::apply_visitor(RespValueVisitor<TArg>(), resp.get()));
}

}
}

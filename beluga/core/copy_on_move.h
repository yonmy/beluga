#pragma once

namespace beluga
{

template<class T>
struct CopyOnMove
{
	mutable T value;

	CopyOnMove(T&& v)
		: value(std::move(v))
	{
	}

	CopyOnMove(const CopyOnMove& rhs)
		: value(std::move(rhs.value))
	{
	}

	CopyOnMove(CopyOnMove&& rhs)
		: value(std::move(rhs.value))
	{
	}

	CopyOnMove& operator=(const CopyOnMove& rhs)
	{
		value = std::move(rhs.value);
		return *this;
	}

	CopyOnMove& operator=(CopyOnMove&& rhs)
	{
		value = std::move(rhs.value);
		return *this;
	}
};

template<class T>
CopyOnMove<T> make_copy_on_move(T&& value)
{
	return CopyOnMove<T>(std::move(value));
}

}

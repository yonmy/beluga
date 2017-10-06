#pragma once

namespace beluga
{

template<class T>
class ForceMover
{
public:
	ForceMover() = delete;
	ForceMover(const T&) = delete;
	ForceMover& operator=(const T&) = delete;

	ForceMover(const ForceMover& rhs)
		: _value(std::move(rhs._value))
	{
	}

	ForceMover(ForceMover&& rhs)
		: _value(std::move(rhs._value))
	{
	}

	explicit ForceMover(T&& rhs)
		: _value(std::move(rhs))
	{
	}

	ForceMover& operator=(const ForceMover& rhs)
	{
		_value = std::move(rhs._value);
		return *this;
	}

	ForceMover& operator=(ForceMover&& rhs)
	{
		_value = std::move(rhs._value);
		return *this;
	}

	ForceMover& operator=(T&& rhs)
	{
		_value = std::move(rhs);
		return *this;
	}

	T& get()
	{
		return _value;
	}

	const T& get() const
	{
		return _value;
	}

private:
	mutable T _value;
};

template<class T>
ForceMover<T> make_force_mover(T&& rhs)
{
	return ForceMover<T>(std::move(rhs));
}

}

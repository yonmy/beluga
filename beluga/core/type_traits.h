#pragma once

#include <type_traits>

namespace beluga
{

template<class... T>
class IsHave
{
public:
	template<class T2>
	static constexpr bool test() noexcept
	{
		return test<T2>(std::integral_constant<size_t, 0>());
	}

private:
	template<class T2, size_t INDEX>
	static constexpr bool test(std::integral_constant<size_t, INDEX>) noexcept
	{
		return std::is_same<std::tuple_element<INDEX, std::tuple<T...>>::type, T2>::value ||
			test<T2>(std::integral_constant<size_t, INDEX + 1>());
	}

	template<class T2>
	static constexpr bool test(std::integral_constant<size_t, sizeof...(T)>) noexcept
	{
		return false;
	}
};

}

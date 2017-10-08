#pragma once

namespace beluga
{

template<class T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())>
{
};

template<class R, class... Args>
struct FunctionTraits<R(*)(Args...)>
{
	static constexpr size_t ARITY = sizeof...(Args);
	using TReturn = R;

	template<size_t INDEX>
	using TArg = typename std::tuple_element<INDEX, std::tuple<Args...>>::type;
};

template<class C, class R, class... Args>
struct FunctionTraits<R(C::*)(Args...)>
{
	static constexpr size_t ARITY = sizeof...(Args);
	using TReturn = R;

	template<size_t INDEX>
	using TArg = typename std::tuple_element<INDEX, std::tuple<Args...>>::type;
};

template<class C, class R, class... Args>
struct FunctionTraits<R(C::*)(Args...) const>
{
	static constexpr size_t ARITY = sizeof...(Args);
	using TReturn = R;

	template<size_t INDEX>
	using TArg = typename std::tuple_element<INDEX, std::tuple<Args...>>::type;
};

}

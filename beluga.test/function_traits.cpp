#include "pch.h"
#include <functional>
#include "beluga/core/function_traits.h"

TEST(FunctionTraits, Lambda)
{
	using namespace beluga;

	auto Lambda1 = [](int, const float&)
	{
		return false;
	};

	auto Lambda2 = []()
	{
	};

	EXPECT_EQ(2, FunctionTraits<decltype(Lambda1)>::ARITY);
	bool ret = std::is_same<FunctionTraits<decltype(Lambda1)>::TReturn, bool>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<decltype(Lambda1)>::TArg<0>, int>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<decltype(Lambda1)>::TArg<1>, const float&>::value;
	EXPECT_TRUE(ret);

	EXPECT_EQ(0, FunctionTraits<decltype(Lambda2)>::ARITY);
	ret = std::is_same<FunctionTraits<decltype(Lambda2)>::TReturn, void>::value;
	EXPECT_TRUE(ret);
}

TEST(FunctionTraits, LambdaMutable)
{
	using namespace beluga;

	auto Lambda1 = [](int, const float&) mutable
	{
		return false;
	};

	auto Lambda2 = []() mutable
	{
	};

	EXPECT_EQ(2, FunctionTraits<decltype(Lambda1)>::ARITY);
	bool ret = std::is_same<FunctionTraits<decltype(Lambda1)>::TReturn, bool>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<decltype(Lambda1)>::TArg<0>, int>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<decltype(Lambda1)>::TArg<1>, const float&>::value;
	EXPECT_TRUE(ret);

	EXPECT_EQ(0, FunctionTraits<decltype(Lambda2)>::ARITY);
	ret = std::is_same<FunctionTraits<decltype(Lambda2)>::TReturn, void>::value;
	EXPECT_TRUE(ret);
}

TEST(FunctionTraits, LegacyFunction)
{
	using namespace beluga;
	using Type = int(*)(const char*, size_t);
	EXPECT_EQ(2, FunctionTraits<Type>::ARITY);
	bool ret = std::is_same<FunctionTraits<Type>::TReturn, int>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<0>, const char*>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<1>, size_t>::value;
	EXPECT_TRUE(ret);
}

TEST(FunctionTraits, StdFunction)
{
	using namespace beluga;
	using Type = std::function<double(short, float, double)>;
	EXPECT_EQ(3, FunctionTraits<Type>::ARITY);
	bool ret = std::is_same<FunctionTraits<Type>::TReturn, double>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<0>, short>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<1>, float>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<2>, double>::value;
	EXPECT_TRUE(ret);
}

TEST(FunctionTraits, MemberFunction)
{
	using namespace beluga;
	using Type = void (std::less<int>::*)();
	EXPECT_EQ(0, FunctionTraits<Type>::ARITY);
	bool ret = std::is_same<FunctionTraits<Type>::TReturn, void>::value;
	EXPECT_TRUE(ret);
}

TEST(FunctionTraits, Functor)
{
	using namespace beluga;
	using Type = std::less<int>;
	EXPECT_EQ(2, FunctionTraits<Type>::ARITY);
	bool ret = std::is_same<FunctionTraits<Type>::TReturn, bool>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<0>, const int&>::value;
	EXPECT_TRUE(ret);
	ret = std::is_same<FunctionTraits<Type>::TArg<1>, const int&>::value;
	EXPECT_TRUE(ret);
}

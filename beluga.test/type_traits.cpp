#include "pch.h"
#include "beluga/core/type_traits.h"

TEST(TypeTraits, IsHave)
{
	using namespace beluga;

	bool ret = IsHave<char, int, unsigned int, std::string>::test<char>();
	EXPECT_TRUE(ret);
	ret = IsHave<char, int, unsigned int, std::string>::test<int>();
	EXPECT_TRUE(ret);
	ret = IsHave<char, int, unsigned int, std::string>::test<unsigned int>();
	EXPECT_TRUE(ret);
	ret = IsHave<char, int, unsigned int, std::string>::test<std::string>();
	EXPECT_TRUE(ret);
	ret = IsHave<char, int, unsigned int, std::string>::test<float>();
	EXPECT_FALSE(ret);
	ret = IsHave<char, int, unsigned int, std::string>::test<char*>();
	EXPECT_FALSE(ret);
}

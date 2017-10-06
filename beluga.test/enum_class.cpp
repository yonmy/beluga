#include "pch.h"
#include "beluga/core/enum_class.h"

BELU_ENUM(eTest, unsigned __int16,
((None))
((Value1))
((Value5, 5))
);

BELU_ENUM(eTest2, unsigned __int16,
((None))
((Value1))
((Value5, 5))
);

TEST(EnumClass, Test)
{
	EXPECT_EQ(eTest(0), eTest::None);
	EXPECT_EQ(eTest(1), eTest::Value1);
	EXPECT_EQ(eTest(5), eTest::Value5);
	EXPECT_EQ(eTest(6), eTest::MAX);

	EXPECT_STREQ("None", Enum<eTest>::ToString(eTest::None));
	EXPECT_STREQ("Value1", Enum<eTest>::ToString(eTest::Value1));
	EXPECT_STREQ("Value5", Enum<eTest>::ToString(eTest::Value5));
	EXPECT_STREQ("", Enum<eTest>::ToString(eTest::MAX));

	EXPECT_EQ(eTest2::None, Enum<eTest2>::From("None").get());
	EXPECT_EQ(eTest2::Value1, Enum<eTest2>::From("Value1").get());
	EXPECT_EQ(eTest2::Value5, Enum<eTest2>::From("Value5").get());
	EXPECT_FALSE(Enum<eTest2>::From("Max").is_initialized());
}

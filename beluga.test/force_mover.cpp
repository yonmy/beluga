#include "pch.h"
#include "beluga/core/force_mover.h"

using namespace beluga;

namespace
{

class TestClass
{
public:
	explicit TestClass(int value)
		: _value(value)
	{
	}

	TestClass(const TestClass& rhs)
		: _value(rhs._value + 1)
	{
	}

	TestClass(TestClass&& rhs)
		: _value(std::move(rhs._value))
	{
	}

	TestClass& operator=(const TestClass& rhs)
	{
		_value = rhs._value + 1;
		return *this;
	}

	TestClass& operator=(TestClass&& rhs)
	{
		_value = std::move(rhs._value);
		return *this;
	}

	int get() const { return _value; }

private:
	int _value;
};

}

TEST(ForceMover, Test)
{
	TestClass tc(1);

	EXPECT_EQ(1, tc.get());
	EXPECT_EQ(1, TestClass(TestClass(1)).get());
	EXPECT_EQ(2, TestClass(tc).get());

	auto fm = make_force_mover(std::move(tc));
	EXPECT_EQ(1, fm.get().get());

	auto fm2 = fm;
	EXPECT_EQ(1, fm2.get().get());

	fm = std::move(fm2);
	EXPECT_EQ(1, fm.get().get());

	EXPECT_EQ(1, ForceMover<TestClass>(fm).get().get());
}

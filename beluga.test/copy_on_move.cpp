#include "pch.h"
#include "beluga/core/copy_on_move.h"

using namespace beluga;

TEST(CopyOnMove, Test)
{
	auto o = std::make_unique<int>(10);
	auto p = o.get();
	auto v = beluga::make_copy_on_move(std::move(o));
	EXPECT_EQ(nullptr, o);
	EXPECT_EQ(p, v.value.get());
	EXPECT_EQ(10, *v.value);
}

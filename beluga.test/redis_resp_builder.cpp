#include "pch.h"
#include "beluga/redis/resp_builder.h"

using namespace beluga;

TEST(RedisRespBuilder, BulkString)
{
	EXPECT_STREQ("$-1\r\n",
		redis::make_bulkstring(nullptr).c_str());

	EXPECT_STREQ("$10\r\nbulkstring\r\n",
		redis::make_bulkstring("bulkstring").c_str());

	EXPECT_STREQ("$0\r\n\r\n",
		redis::make_bulkstring("").c_str());

	EXPECT_STREQ("$10\r\nbulkstring\r\n",
		redis::make_bulkstring(std::string("bulkstring")).c_str());

	EXPECT_STREQ("$-1\r\n",
		redis::make_bulkstring(boost::optional<std::string>()).c_str());

	EXPECT_STREQ("$10\r\nbulkstring\r\n",
		redis::make_bulkstring(boost::optional<std::string>("bulkstring")).c_str());

	EXPECT_STREQ("$2\r\n10\r\n",
		redis::make_bulkstring(10).c_str());

	EXPECT_STREQ("$4\r\n-120\r\n",
		redis::make_bulkstring(-120).c_str());

	EXPECT_STREQ("*3\r\n$1\r\n1\r\n$1\r\n2\r\n$1\r\n3\r\n",
		redis::make_bulkstring_array(1, 2, 3).c_str());

	EXPECT_STREQ("*3\r\n$4\r\ntest\r\n$2\r\n10\r\n$3\r\nbar\r\n",
		redis::make_bulkstring_array("test", 10, "bar").c_str());
}

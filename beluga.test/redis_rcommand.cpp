#include "pch.h"
#include "beluga/redis/rcommand.h"

using namespace beluga;

TEST(RedisRCmd, GET)
{
	auto cmd = redis::RCmd<redis::RCmdType::GET>::make("test");
	EXPECT_STREQ("*2\r\n$3\r\nGET\r\n$4\r\ntest\r\n", cmd.resp_str().c_str());
}

TEST(RedisRCmd, SET)
{
	auto cmd = redis::RCmd<redis::RCmdType::SET>::make("test", 10);
	EXPECT_STREQ("*3\r\n$3\r\nSET\r\n$4\r\ntest\r\n$2\r\n10\r\n", cmd.resp_str().c_str());
}

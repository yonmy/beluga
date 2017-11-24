#include "pch.h"
#include "beluga/redis/rcommand.h"

using namespace beluga::redis;

TEST(RedisRCmd, APPEND)
{
	EXPECT_EQ("*3\r\n$6\r\nAPPEND\r\n$4\r\ntest\r\n$6\r\nappend\r\n",
		RCmd<RCmdType::APPEND>::make("test", "append").resp_str());
}

TEST(RedisRCmd, PING)
{
	EXPECT_EQ("*1\r\n$4\r\nPING\r\n",
		RCmd<RCmdType::PING>::make().resp_str());
	EXPECT_EQ("*2\r\n$4\r\nPING\r\n$4\r\ntest\r\n",
		RCmd<RCmdType::PING>::make("test").resp_str());
}

TEST(RedisRCmd, GET)
{
	EXPECT_EQ("*2\r\n$3\r\nGET\r\n$4\r\ntest\r\n",
		RCmd<RCmdType::GET>::make("test").resp_str());
}

TEST(RedisRCmd, SET)
{
	EXPECT_EQ("*3\r\n$3\r\nSET\r\n$4\r\ntest\r\n$2\r\n10\r\n",
		RCmd<RCmdType::SET>::make("test", 10).resp_str());
}

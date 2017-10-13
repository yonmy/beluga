#include "pch.h"
#include "beluga/redis/resp.h"

TEST(RedisResp, Integer)
{
	using namespace beluga::redis;

	Resp resp = -1i64;

	visitor(resp, [](const __int64& value)
	{
		EXPECT_EQ(-1i64, value);
	});

	visitor(resp, [](const int& value)
	{
		EXPECT_EQ(-1, value);
	});

	visitor(resp, [](const unsigned int& value)
	{
		EXPECT_EQ(std::numeric_limits<unsigned int>::max(), value);
	});

	visitor(resp, [](const float& value)
	{
		EXPECT_EQ(-1.f, value);
	});

	visitor(resp, [](const boost::optional<std::string>& value)
	{
		EXPECT_FALSE(value.is_initialized());
	});

	visitor(resp, [](const std::string& value)
	{
		EXPECT_STREQ("", value.c_str());
	});

	visitor(resp, [](const std::string* value)
	{
		EXPECT_EQ(nullptr, value);
	});

	visitor(resp, [](const char* value)
	{
		EXPECT_EQ(nullptr, value);
	});

	visitor(resp, [](const std::array<int, 1>& value)
	{
		EXPECT_EQ(-1, value.front());
	});

	visitor(resp, [](const std::vector<int>& value)
	{
		EXPECT_EQ(1, value.size());
		EXPECT_EQ(-1, value.front());
	});

	visitor(resp, [](const std::list<int>& value)
	{
		EXPECT_EQ(1, value.size());
		EXPECT_EQ(-1, value.front());
	});

	visitor(resp, [](const std::tuple<short, int>& value)
	{
		EXPECT_EQ(-1, std::get<0>(value));
		EXPECT_EQ(0, std::get<1>(value));
	});
}

TEST(RedisResp, String)
{
	using namespace beluga::redis;

	Resp resp = boost::optional<std::string>("-1");

	visitor(resp, [](const __int64& value)
	{
		EXPECT_EQ(-1i64, value);
	});

	visitor(resp, [](const int& value)
	{
		EXPECT_EQ(-1, value);
	});

	visitor(resp, [](const unsigned int& value)
	{
		EXPECT_EQ(std::numeric_limits<unsigned int>::max(), value);
	});

	visitor(resp, [](const float& value)
	{
		EXPECT_EQ(-1.f, value);
	});

	visitor(resp, [](const boost::optional<std::string>& value)
	{
		EXPECT_TRUE(value.is_initialized());
		EXPECT_STREQ("-1", value.get().c_str());
	});

	visitor(resp, [](const std::string& value)
	{
		EXPECT_STREQ("-1", value.c_str());
	});

	visitor(resp, [](const std::string* value)
	{
		EXPECT_NE(nullptr, value);
		EXPECT_STREQ("-1", value->c_str());
	});

	visitor(resp, [](const char* value)
	{
		EXPECT_NE(nullptr, value);
		EXPECT_STREQ("-1", value);
	});

	visitor(resp, [](const std::array<int, 1>& value)
	{
		EXPECT_EQ(-1, value.front());
	});

	visitor(resp, [](const std::vector<int>& value)
	{
		EXPECT_EQ(1, value.size());
		EXPECT_EQ(-1, value.front());
	});

	visitor(resp, [](const std::list<int>& value)
	{
		EXPECT_EQ(1, value.size());
		EXPECT_EQ(-1, value.front());
	});

	visitor(resp, [](const std::tuple<short, int>& value)
	{
		EXPECT_EQ(-1, std::get<0>(value));
		EXPECT_EQ(0, std::get<1>(value));
	});

	visitor(resp, [](const std::array<std::string, 1>& value)
	{
		EXPECT_STREQ("-1", value.front().c_str());
	});

	visitor(resp, [](const std::vector<std::string>& value)
	{
		EXPECT_EQ(1, value.size());
		EXPECT_STREQ("-1", value.front().c_str());
	});

	visitor(resp, [](const std::list<std::string>& value)
	{
		EXPECT_EQ(1, value.size());
		EXPECT_STREQ("-1", value.front().c_str());
	});

	visitor(resp, [](const std::tuple<std::string, int>& value)
	{
		EXPECT_STREQ("-1", std::get<0>(value).c_str());
		EXPECT_EQ(0, std::get<1>(value));
	});
}

TEST(RedisResp, Array)
{
	using namespace beluga::redis;

	Resp resp = std::vector<Resp>
	{
		boost::optional<std::string>("99"),
		1000i64,
		boost::optional<std::string>("resptest"),
	};

	visitor(resp, [](const __int64& value)
	{
		EXPECT_EQ(0i64, value);
	});

	visitor(resp, [](const boost::optional<std::string>& value)
	{
		EXPECT_FALSE(value.is_initialized());
	});

	visitor(resp, [](const std::string& value)
	{
		EXPECT_STREQ("", value.c_str());
	});

	visitor(resp, [](const std::string* value)
	{
		EXPECT_EQ(nullptr, value);
	});

	visitor(resp, [](const char* value)
	{
		EXPECT_EQ(nullptr, value);
	});

	visitor(resp, [](const std::array<int, 3>& value)
	{
		EXPECT_EQ(99, value[0]);
		EXPECT_EQ(1000, value[1]);
		EXPECT_EQ(0, value[2]);
	});

	visitor(resp, [](const std::vector<int>& value)
	{
		EXPECT_EQ(3, value.size());
		EXPECT_EQ(99, value[0]);
		EXPECT_EQ(1000, value[1]);
		EXPECT_EQ(0, value[2]);
	});

	visitor(resp, [](const std::array<std::string, 3>& value)
	{
		EXPECT_STREQ("99", value[0].c_str());
		EXPECT_STREQ("", value[1].c_str());
		EXPECT_STREQ("resptest", value[2].c_str());
	});

	visitor(resp, [](const std::vector<std::string>& value)
	{
		EXPECT_EQ(3, value.size());
		EXPECT_STREQ("99", value[0].c_str());
		EXPECT_STREQ("", value[1].c_str());
		EXPECT_STREQ("resptest", value[2].c_str());
	});

	visitor(resp, [](const std::tuple<int, __int64, std::string>& value)
	{
		EXPECT_EQ(99, std::get<0>(value));
		EXPECT_EQ(1000i64, std::get<1>(value));
		EXPECT_STREQ("resptest", std::get<2>(value).c_str());
	});
}

TEST(RedisResp, Error)
{
	using namespace beluga::redis;

	Resp resp = "error";
	EXPECT_NE(nullptr, resp.error());
	EXPECT_STREQ("error", resp.error());
}

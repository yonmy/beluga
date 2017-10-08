#include "pch.h"
#include "beluga/redis/resp_reader.h"

using namespace beluga;

TEST(RespReader, Interger)
{
	const std::string TESTCASE = ":0\r\n";

	redis::RespReader reader;
	redis::Resp resp;
	EXPECT_EQ(TESTCASE.size(), reader.read(TESTCASE.c_str(), TESTCASE.size()));
	EXPECT_FALSE(reader.is_unfinished());
	EXPECT_TRUE(reader.is_done());
	EXPECT_FALSE(reader.is_error());
	EXPECT_TRUE(reader.pop_on_done(resp));
	EXPECT_TRUE(reader.is_unfinished());
	EXPECT_FALSE(reader.is_done());
	EXPECT_FALSE(reader.is_error());
	auto ret = redis::get<__int64>(resp);
	EXPECT_NE(nullptr, ret);
	EXPECT_EQ(0i64, *ret);
}

TEST(RespReader, SimpleString)
{
	auto ResultTest = [](const redis::Resp& resp)
	{
		auto ret = redis::get<boost::optional<std::string>>(resp);
		EXPECT_NE(nullptr, ret);
		EXPECT_TRUE(ret->is_initialized());
		EXPECT_STREQ("simplestring", ret->get().c_str());
	};

	{
		const std::string TESTCASE = "+simplestring\r\n";

		redis::RespReader reader;
		redis::Resp resp;
		EXPECT_EQ(TESTCASE.size(), reader.read(TESTCASE.c_str(), TESTCASE.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 1
		std::string TESTCASE1 = "+simple";
		std::string TESTCASE2 = "string\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 2
		std::string TESTCASE1 = "+simplestring";
		std::string TESTCASE2 = "\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 3
		std::string TESTCASE1 = "+simplestring\r";
		std::string TESTCASE2 = "\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}
}

TEST(RespReader, BulkString)
{
	auto ResultTest = [](const redis::Resp& resp)
	{
		auto ret = redis::get<boost::optional<std::string>>(resp);
		EXPECT_NE(nullptr, ret);
		EXPECT_TRUE(ret->is_initialized());
		EXPECT_STREQ("bulkstring", ret->get().c_str());
	};

	{
		const std::string TESTCASE = "$10\r\nbulkstring\r\n";

		redis::RespReader reader;
		redis::Resp resp;
		EXPECT_EQ(TESTCASE.size(), reader.read(TESTCASE.c_str(), TESTCASE.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 1
		std::string TESTCASE1 = "$10\r\nbulk";
		std::string TESTCASE2 = "string\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 2
		std::string TESTCASE1 = "$10\r\n";
		std::string TESTCASE2 = "bulkstring\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 3
		std::string TESTCASE1 = "$10\r\nbulkstring";
		std::string TESTCASE2 = "\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 4
		std::string TESTCASE1 = "$10\r\nbulkstring\r";
		std::string TESTCASE2 = "\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{ // null string test
		const std::string TESTCASE = "$-1\r\n";

		redis::RespReader reader;
		redis::Resp resp;
		EXPECT_EQ(TESTCASE.size(), reader.read(TESTCASE.c_str(), TESTCASE.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		auto ret = redis::get<boost::optional<std::string>>(resp);
		EXPECT_NE(nullptr, ret);
		EXPECT_FALSE(ret->is_initialized());
	}
}

TEST(RespReader, ErrorString)
{
	auto ResultTest = [](const redis::Resp& resp)
	{
		auto ret = redis::get<std::string>(resp);
		EXPECT_NE(nullptr, ret);
		EXPECT_STREQ("errorstring", ret->c_str());
	};

	{
		const std::string TESTCASE = "-errorstring\r\n";

		redis::RespReader reader;
		redis::Resp resp;
		EXPECT_EQ(TESTCASE.size(), reader.read(TESTCASE.c_str(), TESTCASE.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 1
		std::string TESTCASE1 = "-error";
		std::string TESTCASE2 = "string\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 2
		std::string TESTCASE1 = "-errorstring";
		std::string TESTCASE2 = "\r\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{// partial test 3
		std::string TESTCASE1 = "-errorstring\r";
		std::string TESTCASE2 = "\n";

		redis::RespReader reader;
		redis::Resp resp;

		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}
}

TEST(RespReader, Array)
{
	auto ResultTest = [](const redis::Resp& resp)
	{
		auto ret = redis::get<std::vector<redis::Resp>>(resp);
		EXPECT_NE(nullptr, ret);
		EXPECT_EQ((size_t)4, ret->size());
		auto ret0 = redis::get<__int64>(ret->at(0));
		EXPECT_NE(nullptr, ret0);
		EXPECT_TRUE(10i64 == *ret0);
		auto ret1 = redis::get<boost::optional<std::string>>(ret->at(1));
		EXPECT_NE(nullptr, ret1);
		EXPECT_TRUE(ret1->is_initialized());
		EXPECT_STREQ("simplestring", ret1->get().c_str());
		auto ret2 = redis::get<std::vector<redis::Resp>>(ret->at(2));
		EXPECT_NE(nullptr, ret2);
		EXPECT_EQ((size_t)2, ret2->size());
		auto ret2_0 = redis::get<boost::optional<std::string>>(ret2->at(0));
		EXPECT_NE(nullptr, ret2_0);
		EXPECT_TRUE(ret2_0->is_initialized());
		EXPECT_STREQ("foo", ret2_0->get().c_str());
		auto ret2_1 = redis::get<boost::optional<std::string>>(ret2->at(1));
		EXPECT_NE(nullptr, ret2_1);
		EXPECT_TRUE(ret2_1->is_initialized());
		EXPECT_STREQ("bar", ret2_1->get().c_str());
		auto ret3 = redis::get<boost::optional<std::string>>(ret->at(3));
		EXPECT_NE(nullptr, ret3);
		EXPECT_TRUE(ret3->is_initialized());
		EXPECT_STREQ("bulkstring", ret3->get().c_str());
	};

	{
		const std::string TESTCASE = "*4\r\n:10\r\n+simplestring\r\n*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$10\r\nbulkstring\r\n";

		redis::RespReader reader;
		redis::Resp resp;
		EXPECT_EQ(TESTCASE.size(), reader.read(TESTCASE.c_str(), TESTCASE.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}

	{ // partial test 1
		redis::RespReader reader;
		redis::Resp resp;
		const std::string TESTCASE1 = "*4\r\n";
		const std::string TESTCASE2 = ":10\r\n+simplestring\r\n*2\r\n$3\r\nfo";
		const std::string TESTCASE3 = "o\r\n$3\r\nbar\r\n$10\r\nbulkstring\r\n";
		EXPECT_EQ(TESTCASE1.size(), reader.read(TESTCASE1.c_str(), TESTCASE1.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE2.size(), reader.read(TESTCASE2.c_str(), TESTCASE2.size()));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_FALSE(reader.pop_on_done(resp));
		EXPECT_EQ(TESTCASE3.size(), reader.read(TESTCASE3.c_str(), TESTCASE3.size()));
		EXPECT_FALSE(reader.is_unfinished());
		EXPECT_TRUE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		EXPECT_TRUE(reader.pop_on_done(resp));
		EXPECT_TRUE(reader.is_unfinished());
		EXPECT_FALSE(reader.is_done());
		EXPECT_FALSE(reader.is_error());
		ResultTest(resp);
	}
}

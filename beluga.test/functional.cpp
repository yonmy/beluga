#include "pch.h"
#include "beluga/core//functional.h"

using namespace beluga;

namespace
{

class TestCase
{
public:
	TestCase()
	{
		_utestcases = _testcases;
		for (auto& tc : _utestcases)
		{
			std::transform(tc.begin(), tc.end(), tc.begin(), [](char ch)
			{
				return static_cast<char>(std::toupper(ch));
			});
		}

		_wstr.assign(_str.begin(), _str.end());

		_wtestcases.reserve(_testcases.size());
		for (auto& tc : _testcases)
		{
			_wtestcases.emplace_back(std::wstring(tc.begin(), tc.end()));
		}

		_wutestcases.reserve(_utestcases.size());
		for (auto& tc : _utestcases)
		{
			_wutestcases.emplace_back(std::wstring(tc.begin(), tc.end()));
		}
	}

	const auto& str() const { return _str; }
	const auto& cases() const { return _testcases; }
	const auto& ucases() const { return _utestcases; }

	const auto& wstr() const { return _wstr; }
	const auto& wcases() const { return _wtestcases; }
	const auto& wucases() const { return _wutestcases; }

private:
	std::string _str = "TEST1";
	std::vector<std::string> _testcases =
	{
		"test2",
		"tast1",
		"test1",
		"TeSt1",
		"test11",
		"test",
		"aes",
		"aest123"
	};
	std::vector<std::string> _utestcases;

	std::wstring _wstr;
	std::vector<std::wstring> _wtestcases;
	std::vector<std::wstring> _wutestcases;
};

}

TestCase g_tc;

TEST(Functional, CiEqualTo)
{
	for (size_t i = 0; i < g_tc.cases().size(); ++i)
	{
		EXPECT_EQ(
			std::equal_to<std::string>()(g_tc.str(), g_tc.ucases()[i]),
			ci_equal_to<std::string>()(g_tc.str(), g_tc.cases()[i]));
	}

	for (size_t i = 0; i < g_tc.wcases().size(); ++i)
	{
		EXPECT_EQ(
			std::equal_to<std::wstring>()(g_tc.wstr(), g_tc.wucases()[i]),
			ci_equal_to<std::wstring>()(g_tc.wstr(), g_tc.wcases()[i]));
	}
}

TEST(Functional, CiStringEqual)
{
	using string2 = std::basic_string<char, ci_char_traits<char>>;
	string2 cistr = string2(g_tc.str().begin(), g_tc.str().end());
	for (auto& tc : g_tc.cases())
	{
		string2 cistr2 = string2(tc.begin(), tc.end());
		EXPECT_EQ(ci_equal_to<std::string>()(g_tc.str(), tc), cistr == cistr2);
	}

	using wstring2 = std::basic_string<wchar_t, ci_char_traits<wchar_t>>;
	wstring2 ciwstr = wstring2(g_tc.wstr().begin(), g_tc.wstr().end());
	for (auto& tc : g_tc.wcases())
	{
		wstring2 ciwstr2 = wstring2(tc.begin(), tc.end());
		EXPECT_EQ(ci_equal_to<std::wstring>()(g_tc.wstr(), tc), ciwstr == ciwstr2);
	}
}

TEST(Functional, CiNotEqualTo)
{
	for (size_t i = 0; i < g_tc.cases().size(); ++i)
	{
		EXPECT_EQ(
			std::not_equal_to<std::string>()(g_tc.str(), g_tc.ucases()[i]),
			ci_not_equal_to<std::string>()(g_tc.str(), g_tc.cases()[i]));
	}

	for (size_t i = 0; i < g_tc.wcases().size(); ++i)
	{
		EXPECT_EQ(
			std::not_equal_to<std::wstring>()(g_tc.wstr(), g_tc.wucases()[i]),
			ci_not_equal_to<std::wstring>()(g_tc.wstr(), g_tc.wcases()[i]));
	}
}

TEST(Functional, CiLess)
{
	for (size_t i = 0; i < g_tc.cases().size(); ++i)
	{
		EXPECT_EQ(
			std::less<std::string>()(g_tc.str(), g_tc.ucases()[i]),
			ci_less<std::string>()(g_tc.str(), g_tc.cases()[i]));
	}

	for (size_t i = 0; i < g_tc.wcases().size(); ++i)
	{
		EXPECT_EQ(
			std::less<std::wstring>()(g_tc.wstr(), g_tc.wucases()[i]),
			ci_less<std::wstring>()(g_tc.wstr(), g_tc.wcases()[i]));
	}
}

TEST(Functional, CiGreater)
{
	for (size_t i = 0; i < g_tc.cases().size(); ++i)
	{
		EXPECT_EQ(
			std::greater<std::string>()(g_tc.str(), g_tc.ucases()[i]),
			ci_greater<std::string>()(g_tc.str(), g_tc.cases()[i]));
	}

	for (size_t i = 0; i < g_tc.wcases().size(); ++i)
	{
		EXPECT_EQ(
			std::greater<std::wstring>()(g_tc.wstr(), g_tc.wucases()[i]),
			ci_greater<std::wstring>()(g_tc.wstr(), g_tc.wcases()[i]));
	}
}

TEST(Functional, CiLessEqual)
{
	for (size_t i = 0; i < g_tc.cases().size(); ++i)
	{
		EXPECT_EQ(
			std::less_equal<std::string>()(g_tc.str(), g_tc.ucases()[i]),
			ci_less_equal<std::string>()(g_tc.str(), g_tc.cases()[i]));
	}

	for (size_t i = 0; i < g_tc.wcases().size(); ++i)
	{
		EXPECT_EQ(
			std::less_equal<std::wstring>()(g_tc.wstr(), g_tc.wucases()[i]),
			ci_less_equal<std::wstring>()(g_tc.wstr(), g_tc.wcases()[i]));
	}
}

TEST(Functional, CiGreaterEqual)
{
	for (size_t i = 0; i < g_tc.cases().size(); ++i)
	{
		EXPECT_EQ(
			std::greater_equal<std::string>()(g_tc.str(), g_tc.ucases()[i]),
			ci_greater_equal<std::string>()(g_tc.str(), g_tc.cases()[i]));
	}

	for (size_t i = 0; i < g_tc.wcases().size(); ++i)
	{
		EXPECT_EQ(
			std::greater_equal<std::wstring>()(g_tc.wstr(), g_tc.wucases()[i]),
			ci_greater_equal<std::wstring>()(g_tc.wstr(), g_tc.wcases()[i]));
	}
}

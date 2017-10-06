#pragma once

#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>

namespace beluga
{

template<class TChar>
struct ci_char_traits : public std::char_traits<TChar>
{
	static bool eq(const TChar& c1, const TChar& c2)
	{
		return (std::toupper(c1) == std::toupper(c2));
	}

	static bool lt(const TChar& c1, const TChar& c2)
	{
		return (std::toupper(c1) < std::toupper(c2));
	}

	static int compare(const TChar* s1, const TChar* s2, size_t n)
	{
		for (size_t i = 0; i < n; ++i)
		{
			if (!eq(s1[i], s2[i]))
			{
				return (lt(s1[i], s2[i]) ? -1 : 1);
			}
		}
		return 0;
	}

	static const TChar* find(const TChar* const s, const size_t n, const TChar& a)
	{
		for (size_t i = 0; i < n; ++i)
		{
			if (eq(s[i], a))
			{
				return (s + i);
			}
		}
		return nullptr;
	}
};

template<>
struct ci_char_traits<char> : public std::char_traits<char>
{
	static bool eq(const char& c1, const char& c2)
	{
		return (std::toupper(c1) == std::toupper(c2));
	}

	static int compare(const char* s1, const char* s2, size_t n)
	{
		return _memicmp(s1, s2, n);
	}

	static const char* find(const char* const s, const size_t n, const char& a)
	{
		for (size_t i = 0; i < n; ++i)
		{
			if (eq(s[i], a))
			{
				return (s + i);
			}
		}
		return nullptr;
	}
};

template<>
struct ci_char_traits<wchar_t> : public std::char_traits<wchar_t>
{
	static bool eq(const wchar_t& c1, const wchar_t& c2)
	{
		return (std::toupper(c1) == std::toupper(c2));
	}

	static int compare(const wchar_t* s1, const wchar_t* s2, size_t n)
	{
		return _memicmp(s1, s2, n * sizeof(wchar_t));
	}

	static const wchar_t* find(const wchar_t* const s, const size_t n, const wchar_t& a)
	{
		for (size_t i = 0; i < n; ++i)
		{
			if (eq(s[i], a))
			{
				return (s + i);
			}
		}
		return nullptr;
	}
};

template<class T = void>
struct ci_equal_to : public std::equal_to<T>
{
};

template<class TChar, class TCharTraits, class TAlloc>
struct ci_equal_to< std::basic_string<TChar, TCharTraits, TAlloc> >
{
	constexpr bool operator() (
		const std::basic_string<TChar, TCharTraits, TAlloc>& lhs,
		const std::basic_string<TChar, TCharTraits, TAlloc>& rhs) const
	{
		return ((lhs.size() == rhs.size()) &&
			std::equal(lhs.begin(), lhs.end(), rhs.begin(),
				[](const TChar& lchar, const TChar& rchar)
		{
			return std::toupper(lchar) == std::toupper(rchar);
		}));
	}
};

template<class T = void>
struct ci_not_equal_to
{
	constexpr bool operator() (const T& lhs, const T& rhs) const
	{
		return !ci_equal_to<T>()(lhs, rhs);
	}
};

template<class T = void>
struct ci_less : public std::less<T>
{
};

template<class TChar, class TCharTraits, class TAlloc>
struct ci_less< std::basic_string<TChar, TCharTraits, TAlloc> >
{
	constexpr bool operator() (
		const std::basic_string<TChar, TCharTraits, TAlloc>& lhs,
		const std::basic_string<TChar, TCharTraits, TAlloc>& rhs) const
	{
		const auto cmp = ci_char_traits<TChar>::compare(lhs.data(), rhs.data(),
			std::min(lhs.size(), rhs.size()));
		if (cmp != 0)
		{
			return (cmp < 0);
		}
		return (lhs.size() < rhs.size());
	}
};

template<class T = void>
struct ci_greater : public std::greater<T>
{
};

template<class TChar, class TCharTraits, class TAlloc>
struct ci_greater< std::basic_string<TChar, TCharTraits, TAlloc> >
{
	constexpr bool operator() (
		const std::basic_string<TChar, TCharTraits, TAlloc>& lhs,
		const std::basic_string<TChar, TCharTraits, TAlloc>& rhs) const
	{
		const auto cmp = ci_char_traits<TChar>::compare(lhs.data(), rhs.data(),
			std::min(lhs.size(), rhs.size()));
		if (cmp != 0)
		{
			return (cmp > 0);
		}
		return (lhs.size() > rhs.size());
	}
};

template<class T = void>
struct ci_less_equal : public std::less_equal<T>
{
};

template<class TChar, class TCharTraits, class TAlloc>
struct ci_less_equal< std::basic_string<TChar, TCharTraits, TAlloc> >
{
	constexpr bool operator() (
		const std::basic_string<TChar, TCharTraits, TAlloc>& lhs,
		const std::basic_string<TChar, TCharTraits, TAlloc>& rhs) const
	{
		const auto cmp = ci_char_traits<TChar>::compare(lhs.data(), rhs.data(),
			std::min(lhs.size(), rhs.size()));
		if (cmp != 0)
		{
			return (cmp < 0);
		}
		return ((lhs.size() < rhs.size())
			|| ((lhs.size() == rhs.size()) && (cmp == 0)));
	}
};

template<class T = void>
struct ci_greater_equal : public std::greater_equal<T>
{
};

template<class TChar, class TCharTraits, class TAlloc>
struct ci_greater_equal< std::basic_string<TChar, TCharTraits, TAlloc> >
{
	constexpr bool operator() (
		const std::basic_string<TChar, TCharTraits, TAlloc>& lhs,
		const std::basic_string<TChar, TCharTraits, TAlloc>& rhs) const
	{
		const auto cmp = ci_char_traits<TChar>::compare(lhs.data(), rhs.data(),
			std::min(lhs.size(), rhs.size()));
		if (cmp != 0)
		{
			return (cmp > 0);
		}
		return ((lhs.size() > rhs.size())
			|| ((lhs.size() == rhs.size()) && (cmp == 0)));
	}
};

}

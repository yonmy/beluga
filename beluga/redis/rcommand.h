#pragma once

#include "beluga/core/enum_class.h"
#include "resp_builder.h"

namespace beluga
{
namespace redis
{

BELU_ENUM(RCmdType, int,
((PING))
((GET))
((SET))
);

template<RCmdType T> class RCmd;

class IRCmd
{
public:
	explicit IRCmd(std::string&& resp_str)
		: _resp_str(std::move(resp_str))
	{}
	virtual ~IRCmd() = 0 {}

	virtual RCmdType type() const noexcept = 0;
	const auto& resp_str() const noexcept { return _resp_str; }

private:
	std::string _resp_str;
};

template<RCmdType T>
class RCmdBase : public IRCmd
{
public:
	explicit RCmdBase(std::string&& resp_string)
		: IRCmd(std::move(resp_string))
	{}
	virtual ~RCmdBase() {}
	static constexpr RCmdType TYPE = T;
	virtual RCmdType type() const noexcept override final { return TYPE; }
	static const char* type_str() { return Enum<RCmdType>::ToString(TYPE); }
};

// https://redis.io/commands/ping
template<>
class RCmd<RCmdType::PING> : public RCmdBase<RCmdType::PING>
{
	explicit RCmd(std::string&& resp_string)
		: RCmdBase(std::move(resp_string))
	{}

public:
	virtual ~RCmd() = default;

	static RCmd make() noexcept
	{
		return RCmd(make_bulkstring_array(type_str()));
	}

	template<class TMsg>
	static RCmd make(const TMsg& msg) noexcept
	{
		return RCmd(make_bulkstring_array(type_str(), msg));
	}
};

// https://redis.io/commands/get
template<>
class RCmd<RCmdType::GET> : public RCmdBase<RCmdType::GET>
{
	explicit RCmd(std::string&& resp_string)
		: RCmdBase(std::move(resp_string))
	{}

public:
	virtual ~RCmd() = default;

	template<class TKey>
	static RCmd make(const TKey& key) noexcept
	{
		return RCmd(make_bulkstring_array(type_str(), key));
	}
};

// https://redis.io/commands/set
template<>
class RCmd<RCmdType::SET> : public RCmdBase<RCmdType::SET>
{
	explicit RCmd(std::string&& resp_string)
		: RCmdBase(std::move(resp_string))
	{}

public:
	virtual ~RCmd() = default;

	template<class TKey, class TParam>
	static RCmd make(const TKey& key, const TParam& param) noexcept
	{
		return RCmd(make_bulkstring_array(type_str(), key, param));
	}
};

}
}

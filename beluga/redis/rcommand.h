#pragma once

#include "beluga/core/enum_class.h"
#include "resp_builder.h"

namespace beluga
{
namespace redis
{

BELU_ENUM(RCmdType, int,
((GET))
((SET))
);

template<RCmdType T> class RCmd;

class IRCmd
{
public:
	explicit IRCmd(const std::string& resp_str)
		: _resp_str(resp_str)
	{}
	explicit IRCmd(std::string&& resp_str)
		: _resp_str(std::move(resp_str))
	{}
	virtual ~IRCmd() = 0 {}

	IRCmd(const IRCmd&) = default;
	IRCmd& operator=(const IRCmd&) = default;

	IRCmd(IRCmd&&) = default;
	IRCmd& operator=(IRCmd&&) = default;

	virtual RCmdType type() const noexcept = 0;
	const auto& resp_str() const noexcept { return _resp_str; }

private:
	std::string _resp_str;
};

template<RCmdType T>
class RCmdBase : public IRCmd
{
public:
	explicit RCmdBase(const std::string& resp_string)
		: IRCmd(resp_string)
	{}
	explicit RCmdBase(std::string&& resp_string)
		: IRCmd(std::move(resp_string))
	{}
	virtual ~RCmdBase() {}
	static constexpr RCmdType TYPE = T;
	virtual RCmdType type() const noexcept override final { return TYPE; }
	static const char* type_str() { return Enum<RCmdType>::ToString(TYPE); }
};

// https://redis.io/commands/get
template<>
class RCmd<RCmdType::GET> : public RCmdBase<RCmdType::GET>
{
	explicit RCmd(const std::string& resp_string)
		: RCmdBase(resp_string)
	{}
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
	explicit RCmd(const std::string& resp_string)
		: RCmdBase(resp_string)
	{}
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

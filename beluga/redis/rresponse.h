#pragma once

#include "rcommand.h"
#include "resp.h"

namespace beluga
{
namespace redis
{

template<class T, bool>
struct RResponseVisitorOk : public boost::static_visitor<T>
{
	template<class T2>
	T operator()(const T2&) const noexcept
	{
		return T();
	}

	template<>
	T operator()(const boost::optional<std::string>& value) const noexcept
	{
		return (value.is_initialized() ?
			static_cast<T>(::_stricmp("OK", value.get().c_str()) == 0) :
			T());
	}
};

template<class T>
struct RResponseVisitorOk<T, false> : public RespValueVisitor<T>
{
};

template<RCmdType TYEP, class T>
struct RResponseVisitor : public RespValueVisitor<T> {};

template<class T>
struct RResponseVisitor<RCmdType::SET, T> : public RResponseVisitorOk<T, std::is_arithmetic_v<T>> {};

class IRResponse
{
public:
	IRResponse() {}
	virtual ~IRResponse() = 0 {}

	virtual const std::string& resp_str() const = 0;
	virtual void run(const Resp&) = 0;
};

template<RCmdType TYPE>
class RResponse : public IRResponse
{
public:
	explicit RResponse(RCmd<TYPE>&& cmd)
		: _cmd(std::move(cmd))
	{}
	explicit RResponse(const RCmd<TYPE>& cmd)
		: _cmd(cmd)
	{}
	virtual ~RResponse() {}

	virtual const std::string& resp_str() const override final { return _cmd.resp_str(); }
	virtual void run(const Resp&) override {}

protected:
	RCmd<TYPE> _cmd;
};

template<RCmdType TYPE, class TCallback>
class RResponse1 : public RResponse<TYPE>
{
public:
	explicit RResponse1(RCmd<TYPE>&& cmd, TCallback&& callback)
		: RResponse(std::move(cmd))
		, _callback(std::move(callback))
	{
	}
	explicit RResponse1(const RCmd<TYPE>& cmd, TCallback&& callback)
		: RResponse(cmd)
		, _callback(std::move(callback))
	{
	}

	virtual ~RResponse1() {}

	virtual void run(const Resp& resp) override
	{
		using TArg = std::remove_const_t<std::remove_reference_t<
			FunctionTraits<TCallback>::TArg<0>>>;
		_callback(boost::apply_visitor(
			RResponseVisitor<TYPE, TArg>(), resp.get()));
	}

private:
	TCallback _callback;
};

template<RCmdType TYPE, class TCallback>
class RResponse2 : public RResponse<TYPE>
{
public:
	RResponse2(RCmd<TYPE>&& cmd, TCallback&& callback)
		: RResponse(std::move(cmd))
		, _callback(std::move(callback))
	{
	}

	virtual ~RResponse2() {}

	virtual void run(const Resp& resp) override
	{
		using TArg = std::remove_const_t<std::remove_reference_t<
			FunctionTraits<TCallback>::TArg<1>>>;
		_callback(_cmd, boost::apply_visitor(
			RResponseVisitor<TYPE, TArg>(), resp.get()));
	}

private:
	TCallback _callback;
};

}
}

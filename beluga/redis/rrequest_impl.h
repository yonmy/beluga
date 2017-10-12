#pragma once

#include "rcommand.h"
#include "resp.h"
#include "resp_builder.h"

namespace beluga
{
namespace redis
{

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
		using TArg = std::remove_const_t<std::remove_reference_t<FunctionTraits<TCallback>::TArg<0>>>;
		_callback(boost::apply_visitor(RespValueVisitor<TArg>(), resp.get()));
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
		using TArg = std::remove_const_t<std::remove_reference_t<FunctionTraits<TCallback>::TArg<1>>>;
		_callback(_cmd, boost::apply_visitor(RespValueVisitor<TArg>(), resp.get()));
	}

private:
	TCallback _callback;
};

class RRequestImpl
{
public:
	RRequestImpl() {}
	~RRequestImpl() {}

	size_t size() const noexcept { return _responses.size(); }

	void build_cmd(std::string& out) const
	{
		for (auto& response : _responses)
		{
			out += response->resp_str();
		}
	}

	template<RCmdType TYPE>
	void push(RCmd<TYPE>&& cmd)
	{
		_responses.emplace_back(new RResponse<TYPE>(std::move(cmd)));
	}

	template<RCmdType TYPE>
	void push(const RCmd<TYPE>& cmd)
	{
		_responses.emplace_back(new RResponse<TYPE>(cmd));
	}

	template<RCmdType TYPE, class TCallback>
	void push(RCmd<TYPE>&& cmd, TCallback&& callback)
	{
		push(std::move(cmd), std::move(callback),
			std::integral_constant<size_t, FunctionTraits<TCallback>::ARITY>());
	}

	template<RCmdType TYPE, class TCallback>
	void push(const RCmd<TYPE>& cmd, TCallback&& callback)
	{
		push(cmd, std::move(callback),
			std::integral_constant<size_t, FunctionTraits<TCallback>::ARITY>());
	}

	size_t on_response(Resp& resp)
	{
		if (!_responses.empty())
		{
			_responses.front()->run(resp);
			_responses.pop_front();
		}
		return _responses.size();
	}

private:
	template<RCmdType TYPE, class TCallback>
	void push(RCmd<TYPE>&& cmd, TCallback&& callback, std::integral_constant<size_t, 1>)
	{
		_responses.emplace_back(new RResponse1<TYPE, TCallback>(
			std::move(cmd), std::move(callback)));
	}

	template<RCmdType TYPE, class TCallback>
	void push(const RCmd<TYPE>& cmd, TCallback&& callback, std::integral_constant<size_t, 1>)
	{
		_responses.emplace_back(new RResponse1<TYPE, TCallback>(
			cmd, std::move(callback)));
	}

	template<class TCmd, class TCallback>
	void push(TCmd&& cmd, TCallback&& callback, std::integral_constant<size_t, 2>)
	{
		_responses.emplace_back(new RResponse2<TYPE, TCallback>(
			std::move(cmd), std::move(callback)));
	}

	template<class TCmd, class TCallback>
	void push(const TCmd& cmd, TCallback&& callback, std::integral_constant<size_t, 2>)
	{
		_responses.emplace_back(new RResponse2<TYPE, TCallback>(
			cmd, std::move(callback)));
	}

private:
	std::list<std::unique_ptr<IRResponse>> _responses;
};

}
}

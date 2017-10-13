#pragma once

#include "rresponse.h"
#include "resp_builder.h"

namespace beluga
{
namespace redis
{

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

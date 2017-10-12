#pragma once

#include "rrequest_impl.h"

namespace beluga
{
namespace redis
{

template<class TClient>
class RRequest
{
public:
	explicit RRequest(TClient& client)
        : _client(client)
    {
    }

	~RRequest() = default;
	RRequest(const RRequest&) = delete;
	RRequest& operator=(const RRequest&) = delete;

	template<RCmdType TYPE>
	void push(RCmd<TYPE>&& cmd)
	{
        if (_impl == nullptr)
        {
            _impl = std::make_unique<RRequestImpl>();
        }
        _impl->push(std::move(cmd));
	}

    template<RCmdType TYPE>
    void push(const RCmd<TYPE>& cmd)
    {
        if (_impl == nullptr)
        {
            _impl = std::make_unique<RRequestImpl>();
        }
        _impl->push(cmd);
    }

	template<RCmdType TYPE, class TCallback>
	void push(RCmd<TYPE>&& cmd, TCallback&& callback)
	{
        if (_impl == nullptr)
        {
            _impl = std::make_unique<RRequestImpl>();
        }
        _impl->push(std::move(cmd), std::move(callback));
	}

    template<RCmdType TYPE, class TCallback>
    void push(const RCmd<TYPE>& cmd, TCallback&& callback)
    {
        if (_impl == nullptr)
        {
            _impl = std::make_unique<RRequestImpl>();
        }
        _impl->push(cmd, std::move(callback));
    }

    bool commit()
    {
        if (_impl && (_impl->size() > 0))
        {
            _client.push(std::move(_impl));
            return true;
        }
        return false;
    }

private:
    TClient&                        _client;
    std::unique_ptr<RRequestImpl>   _impl;
};

}
}

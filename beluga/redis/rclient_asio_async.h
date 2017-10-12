#pragma once

#include <mutex>
#include <queue>
#include "boost/asio.hpp"
#include "resp_reader.h"

namespace beluga
{
namespace redis
{

class RClientAsioAsnyc
{
public:
    using RequestPtr = std::unique_ptr<class RRequestImpl>;
	using RequestQueue = std::queue<RequestPtr>;

public:
    explicit RClientAsioAsnyc(boost::asio::io_service& iosvc)
		: _socket(iosvc)
		, _handler(nullptr)
	{
	}

    ~RClientAsioAsnyc() {}

	template<class THandler>
	void open(boost::asio::ip::tcp::resolver::iterator ep_itr,
		THandler&& handler = [](boost::system::error_code){})
	{
		using namespace boost::asio;

		decltype(_rqueue) trash;

		{
			std::lock_guard<std::mutex> guard(_mutex);
			if (_handler != nullptr)
			{
				return;
			}

			_sbuf.clear();
			_sendingbuf.clear();
			_recvbuf.consume(_recvbuf.size());
			_reader.clear();
			_request.reset();
			_rqueue.swap(trash);
			_handler = std::move(handler);

			async_connect(_socket, ep_itr, [this](
				boost::system::error_code err, ip::tcp::resolver::iterator)
			{
				_handler(err);
				if (err)
				{
					_socket.close();
				}
				else
				{
					do_read();
				}
			});
		}
	}

	void close()
	{
		_socket.close();
	}

	void push(RequestPtr&& request)
	{
        if (request->size() == 0)
        {
            return;
        }

		std::lock_guard<std::mutex> guard(_mutex);
        request->build_cmd(_sbuf);
        _rqueue.emplace(std::move(request));
		do_send();
	}

private:
	void do_read()
	{
		using namespace boost::asio;

		async_read_until(_socket, _recvbuf, std::string("\r\n"),
			[this](boost::system::error_code err, size_t)
		{
			if (err)
			{
				_socket.close();
				decltype(_handler) handler = std::move(_handler);
				handler(err);
				return;
			}

            Resp resp;

            auto bufsize = _recvbuf.size();
            while (bufsize > 0)
            {
                _recvbuf.consume(_reader.read(reinterpret_cast<const char*>(
                    detail::buffer_cast_helper(_recvbuf.data())), bufsize));
                if (!_reader.pop_on_done(resp))
                {
                    if (_reader.is_error())
                    {
                        _socket.close();
                    }
                    break;
                }

                if (_request == nullptr)
                {
                    std::lock_guard<std::mutex> guard(_mutex);
                    if (_rqueue.empty())
                    {
                        _socket.close();
                        break;
                    }

                    _request = std::move(_rqueue.front());
                    _rqueue.pop();
                }

                if (!_request->on_response(resp))
                {
                    _request.reset();
                }

                bufsize = _recvbuf.size();
            }

            do_read();
		});
	}

	void do_send()
	{
		using namespace boost::asio;

		if (!_sendingbuf.empty() || _sbuf.empty())
		{
			return;
		}

        static const size_t MAXBYTES = 8192 * 2;
        _sbuf.swap(_sendingbuf);
        if (_sendingbuf.size() > MAXBYTES)
        {
            _sbuf.append(_sendingbuf, MAXBYTES);
            _sendingbuf.resize(MAXBYTES);
        }

		async_write(_socket,
			buffer(_sendingbuf.data(), _sendingbuf.size()),
			[this](boost::system::error_code err, size_t)
		{
			if (err)
			{
				_socket.close();
			}
			else
			{
				std::lock_guard<std::mutex> guard(_mutex);
				_sendingbuf.clear();
				do_send();
			}
		});
	}

private:
	std::function<void(boost::system::error_code)> _handler;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::streambuf _recvbuf;

	std::mutex _mutex;
    std::string _sbuf;
	std::string _sendingbuf;
    RespReader _reader;
    RequestPtr _request;
	RequestQueue _rqueue;
};

}
}

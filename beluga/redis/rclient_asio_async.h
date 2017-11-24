#pragma once

#include <mutex>
#include <queue>
#include "boost/asio.hpp"
#include "resp_reader.h"

namespace beluga
{
namespace redis
{

class RClientAsioAsync
{
public:
    using RequestPtr = std::unique_ptr<class RRequestImpl>;
	using RequestQueue = std::queue<RequestPtr>;

private:
	struct Session
	{
		explicit Session(boost::asio::io_service& iosvc)
			: socket(iosvc)
		{}

		void close()
		{
			boost::system::error_code ec;
			socket.close(ec);
		}

		std::function<void(boost::system::error_code)> handler = nullptr;
		boost::asio::ip::tcp::socket socket;
		boost::asio::streambuf recvbuf;

		std::mutex mutex;
		std::string sbuf;
		std::string sendingbuf;
		RespReader reader;
		RequestPtr request;
		RequestQueue rqueue;
	};
	std::shared_ptr<Session> _session;

public:
    explicit RClientAsioAsync(boost::asio::io_service& iosvc)
		: _session(std::make_shared<Session>(iosvc))
	{
	}

    ~RClientAsioAsync() {}

	template<class THandler>
	void open(boost::asio::ip::tcp::resolver::iterator ep_itr,
		THandler&& handler = nullptr)
	{
		using namespace boost::asio;

		decltype(_session->rqueue) trash;

		{
			std::lock_guard<std::mutex> guard(_session->mutex);
			if (_session->handler != nullptr)
			{
				return;
			}

			_session->sbuf.clear();
			_session->sendingbuf.clear();
			_session->recvbuf.consume(_session->recvbuf.size());
			_session->reader.clear();
			_session->request.reset();
			_session->rqueue.swap(trash);
			_session->handler = std::move(handler);
			if (_session->handler == nullptr)
			{
				_session->handler = [](boost::system::error_code) {};
			}

			async_connect(_session->socket, ep_itr, [session = _session](
				boost::system::error_code err, ip::tcp::resolver::iterator) mutable
			{
				{
					std::lock_guard<std::mutex> guard(session->mutex);
					if (err)
					{
						while (!session->rqueue.empty())
						{
							Resp resp("disconnected");
							while (session->rqueue.front()->on_response(resp) > 0) {}
							session->rqueue.pop();
						}
					}
					else
					{
						do_read(session);
					}
				}

				if (session->handler != nullptr)
				{
					session->handler(err);
					if (err)
					{
						session->handler = nullptr;
					}
				}
			});
		}
	}

	void close()
	{
		_session->close();
	}

	void push(RequestPtr&& request)
	{
        if (request->size() == 0)
        {
            return;
        }

		{
			std::lock_guard<std::mutex> guard(_session->mutex);
			if (_session->handler != nullptr)
			{
				request->build_cmd(_session->sbuf);
				_session->rqueue.emplace(std::move(request));
				do_send(_session);
				return;
			}
		}
        
		Resp resp("disconnected");
		request->on_response(resp);
	}

private:
	static void do_read(std::shared_ptr<Session>& session)
	{
		using namespace boost::asio;

		async_read_until(session->socket, session->recvbuf, std::string("\r\n"),
			[session](boost::system::error_code err, size_t) mutable
		{
			if (err)
			{
				session->close();
				return;
			}

            Resp resp;

            auto bufsize = session->recvbuf.size();
            while (bufsize > 0)
            {
				session->recvbuf.consume(session->reader.read(reinterpret_cast<const char*>(
                    detail::buffer_cast_helper(session->recvbuf.data())), bufsize));
                if (!session->reader.pop_on_done(resp))
                {
                    if (session->reader.is_error())
                    {
						session->close();
                    }
                    break;
                }

                if (session->request == nullptr)
                {
                    std::lock_guard<std::mutex> guard(session->mutex);
                    if (session->rqueue.empty())
                    {
						session->close();
                        break;
                    }

					session->request = std::move(session->rqueue.front());
					session->rqueue.pop();
                }

                if (session->request->on_response(resp) == 0)
                {
					session->request.reset();
                }

                bufsize = session->recvbuf.size();
            }

            do_read(session);
		});
	}

	static void do_send(std::shared_ptr<Session>& session)
	{
		using namespace boost::asio;

		if (!session->sendingbuf.empty() || session->sbuf.empty())
		{
			return;
		}

        static const size_t MAXBYTES = 8192 * 2;
		session->sbuf.swap(session->sendingbuf);
        if (session->sendingbuf.size() > MAXBYTES)
        {
			session->sbuf.append(session->sendingbuf, MAXBYTES);
			session->sendingbuf.resize(MAXBYTES);
        }

		async_write(session->socket,
			buffer(session->sendingbuf.data(), session->sendingbuf.size()),
			[session](boost::system::error_code err, size_t) mutable
		{
			if (err)
			{
				session->close();
			}
			else
			{
				std::lock_guard<std::mutex> guard(session->mutex);
				session->sendingbuf.clear();
				do_send(session);
			}
		});
	}
};

}
}



#include "pch.h"
#include <iostream>
#include <future>
#include <thread>

#include "beluga/redis/rrequest.h"
#include "beluga/redis/rclient_asio_async.h"

int main(int argc, char* argv[])
{
	using namespace boost::asio;
	using namespace beluga::redis;

	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: <redis-host> <port>" << std::endl;
			return 1;
		}

		io_service iosvc;
		RClientAsioAsnyc client(iosvc);
		client.open(ip::tcp::resolver(iosvc).resolve({ argv[1], argv[2] }),
			[&client](boost::system::error_code err)
		{
			std::cout << "client " << err << std::endl;
			if (err == 0)
			{
				RRequest<RClientAsioAsnyc> request(client);
				request.push(RCmd<RCmdType::PING>::make(), [](const std::string& ret)
				{
					std::cout << ret << std::endl;
				});
				request.commit();
				request.push(RCmd<RCmdType::PING>::make("hi"), [](const std::string& ret)
				{
					std::cout << ret << std::endl;
				});
				request.commit();
				request.push(RCmd<RCmdType::SET>::make("a", 10), [](bool ret)
				{
					std::cout << ret << std::endl;
				});
				request.push(RCmd<RCmdType::SET>::make("b", 20), [&client](const std::string& ret)
				{
					std::cout << ret << std::endl;
				});
				request.commit();
				request.push(RCmd<RCmdType::GET>::make("a"), [](const std::string& ret)
				{
					std::cout << ret << std::endl;
				});
				request.push(RCmd<RCmdType::GET>::make("b"), [&client](const int& ret)
				{
					std::cout << ret << std::endl;
					client.close();
				});
				request.commit();
			}
		});

		std::thread([&iosvc]()
		{
			iosvc.run();
		}).join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	::system("pause");
    return 0;
}

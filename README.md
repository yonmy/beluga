# beluga
yonmy's beluga library

# beluga/redis
쉬운 활용을 목표로 개발중인 비동기 redis 라이브러리 입니다.

## write some samples
### simple example for redis STRING and redis KEY(with boost):

```c++
#include <assert.h>
#include <iostream>
#include <future>
#include <thread>
#include <string>
#include "beluga/redis/rrequest.h"
#include "beluga/redis/rclient_asio_async.h"

using namespace beluga::redis;

std::string test_redis_string(RClientAsioAsnyc& client, const char* key)
{
	// make redis request
	RRequest<RClientAsioAsnyc> request(client);

	// push redis command: SET key value
	request.push(RCmd<RCmdType::SET>::make(key, "test_value"));

	// commit redis command
	request.commit();

	std::promise<std::string> promise;
	auto future = promise.get_future();

	// push redis command with callback: GET key 
	request.push(RCmd<RCmdType::GET>::make(key), [&promise](const std::string& ret)
	{
		// called after commit call
		assert(ret == std::string("test_value"));
		promise.set_value(ret);
	});

	// commit redis command
	request.commit();

	future.wait();
	return future.get();
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: <redis-host> <port>" << std::endl;
			return 1;
		}

		boost::asio::io_service iosvc;
		RClientAsioAsnyc client(iosvc);
		std::promise<boost::system::error_code> promise;
		auto future = promise.get_future();
		client.open(boost::asio::ip::tcp::resolver(iosvc).resolve({ argv[1], argv[2] }),
			[&promise](boost::system::error_code err)
		{
			promise.set_value(err);
			std::promise<boost::system::error_code>().swap(promise);
		});

		std::thread thread([&iosvc]()
		{
			iosvc.run();
		});

		future.wait();
		if (!future.get())
		{
			std::cout << test_redis_string(client, "test") << std::endl;
		}

		client.close();

		thread.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}


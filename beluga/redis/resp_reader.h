#pragma once

#include <functional>
#include "resp.h"

namespace beluga
{
namespace redis
{

class RespReader
{
	enum class State : unsigned char
	{
		Partial,
		Done,
		Error
	};

public:
	RespReader() = default;

	RespReader(const RespReader&) = delete;
	RespReader& operator=(const RespReader&) = delete;

	RespReader(RespReader&& rhs);
	RespReader& operator=(RespReader&& rhs);

	void clear();

	bool is_unfinished() const { return (_state == State::Partial); }
	bool is_done() const { return (_state == State::Done); }
	bool is_error() const { return (_state == State::Error); }

	size_t read(const char* buf, size_t size);
	bool pop_on_done(Resp& out);

private:
	static std::pair<size_t, __int64> read_integer(const char* buf, size_t size);
	size_t read_string(std::string& out, const char* buf, size_t size);

private:
	State _state = State::Partial;
	Resp _resp;
	std::function<size_t(const char*, size_t)> _reader = nullptr;
};

}
}

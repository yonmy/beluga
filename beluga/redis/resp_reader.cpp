#include "pch.h"
#include "resp_reader.h"
#include "core/force_mover.h"

// https://redis.io/topics/protocol

namespace beluga
{
namespace redis
{

namespace
{

class RespArrayReader
{
public:
	RespArrayReader(std::vector<Resp>& out, size_t count)
		: _out(out)
		, _count(0)
	{
		_out.resize(count);
	}

	bool is_unfinished() const { return (!is_done() && _reader.is_unfinished()); }
	bool is_done() const { return (_count == _out.size()); }
	bool is_error() const { return _reader.is_error(); }

	size_t read(const char* buf, size_t size)
	{
		if (!is_unfinished())
		{
			return 0;
		}

		const size_t readsize = _reader.read(buf, size);
		if (readsize == 0)
		{
			return 0;
		}

		if (!_reader.pop_on_done(_out[_count]))
		{
			return readsize;
		}

		++_count;
		return (readsize + read(buf + readsize, size - readsize));
	}

	RespArrayReader(RespArrayReader&&) = default;

private:
	std::vector<Resp>&	_out;
	size_t				_count;
	RespReader			_reader;
};

}

RespReader::RespReader(RespReader&& rhs)
	: _state(std::move(rhs._state))
	, _value(std::move(rhs._value))
	, _reader(std::move(rhs._reader))
{
}

RespReader& RespReader::operator=(RespReader&& rhs)
{
	_state = std::move(rhs._state);
	_value = std::move(rhs._value);
	_reader = std::move(rhs._reader);
	return (*this);
}

void RespReader::clear()
{
	_state = State::Partial;
	_value = decltype(_value)();
	_reader = nullptr;
}

size_t RespReader::read(const char* buf, size_t size)
{
	if((_state != State::Partial) || (size == 0))
	{
		return 0;
	}

	if (_reader == nullptr)
	{
		size_t readsize = 1;
		switch (*buf)
		{
		case '$':
		{
			auto ret = read_integer(buf + readsize, size - readsize);
			if (ret.first == 0)
			{
				return 0;
			}

			readsize += ret.first;
			if (ret.second < 0)
			{
				_value = boost::optional<std::string>();
				_state = State::Done;
				return readsize;
			}

			std::string str;
			str.reserve(static_cast<size_t>(ret.second));
			_value = boost::optional<std::string>(std::move(str));
			_reader = [this](const char* buf, size_t size)
			{
				return read_string(boost::get<boost::optional<std::string>>(_value).get(),
					buf, size);
			};
		} break;
		case '*':
		{
			auto ret = read_integer(buf + readsize, size - readsize);
			if (ret.first == 0)
			{
				return 0;
			}

			readsize += ret.first;
			_value = std::vector<Resp>();
			if (ret.second <= 0)
			{
				_state = State::Done;
				return readsize;
			}

			auto r = make_force_mover(RespArrayReader(
				boost::get<std::vector<Resp>>(_value),
				static_cast<size_t>(ret.second)));
			_reader = [this, r](const char* buf, size_t size) mutable
			{
				const size_t readsize = r.get().read(buf, size);
				if (r.get().is_done())
				{
					_state = State::Done;
				}
				else if (r.get().is_error())
				{
					_state = State::Error;
				}
				return readsize;
			};
		} break;
		case '+':
		{
			_value = boost::optional<std::string>(std::string());
			_reader = [this](const char* buf, size_t size)
			{
				return read_string(boost::get<boost::optional<std::string>>(_value).get(),
					buf, size);
			};
		} break;
		case '-':
		{
			_value = std::string();
			_reader = [this](const char* buf, size_t size)
			{
				return read_string(boost::get<std::string>(_value), buf, size);
			};
		} break;
		case ':':
		{
			auto ret = read_integer(buf + readsize, size - readsize);
			if (ret.first == 0)
			{
				return 0;
			}

			readsize += ret.first;
			_value = ret.second;
			_state = State::Done; return readsize;
		} break;
		default:
		{
			_state = State::Error;
			return 0;
		} break;
		}

		return (readsize + _reader(buf + readsize, size - readsize));
	}

	return _reader(buf, size);
}

std::pair<size_t, __int64> RespReader::read_integer(const char* buf, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		if (buf[i] == '\n')
		{
			char* pEnd;
			return { i + 1, std::strtoll(buf, &pEnd, 10) };
		}
	}
	return { 0, 0 };
}

size_t RespReader::read_string(std::string& out, const char* buf, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		if (buf[i] == '\n')
		{
			const size_t len = ((i > 0 && buf[i - 1] == '\r') ? (i - 1) : i);
			if (len > 0)
			{
				out.append(buf, len);
			}
			_state = State::Done;
			return (i + 1);
		}
	}

	const size_t len = ((size > 0 && buf[size - 1] == '\r') ? (size - 1) : size);
	if (len > 0)
	{
		out.append(buf, len);
	}
	return (size);
}

bool RespReader::pop_on_done(Resp& out)
{
	if (is_done())
	{
		out.swap(_value);
		clear();
		return true;
	}
	return false;
}

}
}

#pragma once

#include <atomic>

#define DJSW_DEFAULT_MESSAGE_QUEUE_CAPACITY 1024

template <typename T_Message>
class djMessageQueue
{
public:
	bool bypass;

	djMessageQueue() :
		djMessageQueue(DJSW_DEFAULT_MESSAGE_QUEUE_CAPACITY)
	{

	}

	djMessageQueue(int capacity) :
		bypass(false),
		_capacity(capacity),
		_queue(new T_Message[capacity]),
		_head(0),
		_tail(0)
	{

	}

	~djMessageQueue()
	{
		delete[] _queue;
	}

	// Single Producer
	/*bool Push(T_Message* message)
	{
		int t = _tail.load(std::memory_order_relaxed);
		int next = (t + 1) % _capacity;

		if (next == _head.load(std::memory_order_acquire))
			return false;

		_queue[t] = *message;
		_tail.store(next, std::memory_order_release);

		return true;
	}*/

	// Multiple Producer
	bool Push(T_Message* message)
	{
		int t = _tail.load(std::memory_order_relaxed);
		int next;

		do
		{
			next = (t + 1) % _capacity;

			if (next == _head.load(std::memory_order_acquire))
				return false;
		} while (!_tail.compare_exchange_weak(t, next, std::memory_order_release, std::memory_order_relaxed));

		_queue[t] = *message;
		return true;
	}

	bool Pop(T_Message* message)
	{
		int h = _head.load(std::memory_order_relaxed);

		if (h == _tail.load(std::memory_order_acquire))
			return false;

		*message = _queue[h];
		_head.store((h + 1) % _capacity, std::memory_order_release);

		return true;
	}

private:
	int _capacity;
	T_Message* _queue;

	std::atomic<int> _head;
	std::atomic<int> _tail;
};
#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t s)
	:stop(false)
{
	for (size_t i = 0; i < s; ++i)
	{
		wokers.emplace_back([this] {

			for (;;)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(this->queue_mutex);
					cond.wait(lock, [this] {
						return stop || !this->tasks.empty();
					});
					if (stop && this->tasks.empty())
					{
						return;
					}
					task = std::move(tasks.front());
					tasks.pop();
				}
				task();
			}
		});
	}
}


ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(this->queue_mutex);
		stop = true;
	}
	cond.notify_all();
	for (auto &worker : wokers)
	{
		worker.join();
	}
}

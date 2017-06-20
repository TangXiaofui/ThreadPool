#ifndef ThreadPool_H
#define ThreadPool_H
#include <type_traits>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <memory>
#include <stdexcept>


class ThreadPool
{
public:
	ThreadPool(size_t);
	~ThreadPool();
	template <typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;
private:
	std::vector<std::thread> wokers;
	std::queue<std::function<void()>> tasks;

	bool stop;
	std::mutex queue_mutex;
	std::condition_variable cond;

};

template <typename F, typename ... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args ...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared < std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		if (stop)
			throw std::runtime_error("enqueue on threadpool stop");

		tasks.emplace([task] {
			(*task)();
		});
	}
	cond.notify_one();
	return res;
}


#endif // ThreadPool_H


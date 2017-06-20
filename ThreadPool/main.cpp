#include "ThreadPool.h"
#include <iostream>
#include <thread>


int main()
{
	ThreadPool tp(4);
	std::vector<std::future<int>> res;
	for (int i = 0; i < 10; i++)
	{
		res.emplace_back(tp.enqueue([i] {
			std::cout << "hello";
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << " " << i << std::endl;
			return i*i;
		}));
	}

	for (auto &e : res)
	{
		std::cout << e.get() << std::endl;
	}
	return 0;
}
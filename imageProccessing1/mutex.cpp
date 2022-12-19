#include <iostream>
#include <thread>
#include <mutex>

int value = 0;
std::mutex value_mutex;

void increase_value()
{
	value_mutex.lock();
	value++;
	std::cout << "스레드 실행 : " << value << std::endl;
	value_mutex.unlock();
}

int main()
{
	std::thread t[10];

	for (auto i = 0; i < 10; i++)
	{
		t[i] = std::thread(increase_value);
	}

	for (int i = 0; i < 10; i++)
	{
		t[i].join();
	}

	return 0;
}
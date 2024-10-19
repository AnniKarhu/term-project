#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <string>

#include "tasks_queue.h"

class thread_pool;

class url_processing_thread : public std::thread
{
public:		
	bool in_work = false;
	int thread_num = -1;	
};

//class url_processing_thread
//{
//public:
//	url_processing_thread(std::function<void()> _work_function, int _thread_num)
//	{
//		url_thread = new std::thread(_work_function);
//		thread_num = _thread_num;
//	}
//	std::thread* url_thread;
//	bool in_idle = true;
//	int thread_num = -1;
//
//	~url_processing_thread()
//	{
//		(*url_thread).join();
//		delete url_thread;
//	}
//};


class thread_pool
{
private:
	//std::vector<std::thread> th_vector;
	std::vector<url_processing_thread> th_vector;

	tasks_queue  pool_queue;	
	
	std::atomic<bool> task_generator_finished(); // = false;
	bool start_work = false;
	
	std::mutex start_mutex;
	std::condition_variable start_threads;
	
	void work(int thread_index); //рабочая функция потоков	

public:	
	
	thread_pool();	
	~thread_pool();	

	//void submit(std::function<void()> new_task, std::string func_name, int func_count); //добавление задачи в очередь
	void submit(url_item new_url_item); //добавление адреса в очередь
	void start_threads_work(); //старт рабочих потоков
	//void finish_tasks(); //флаг завершения генерирования задач	
};

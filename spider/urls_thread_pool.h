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

class thread_pool
{
private:
	//std::vector<std::thread> th_vector;
	std::vector<url_processing_thread> th_vector;

	tasks_queue  pool_queue;	
	
	std::atomic<bool> task_generator_finished(); 
	bool start_work = false;
	
	std::mutex start_mutex;
	std::condition_variable start_threads;
	
	void work(const int& thread_index); //рабочая функция потоков
	void pool_queue_pop_next(const int& thread_index);
	void submit(const url_item new_url_item); //добавление адреса в очередь

public:	
	
	thread_pool(const std::string& start_url, unsigned int max_depth);
	~thread_pool();	

	
	void start_threads_work(); //старт рабочих потоков	
};

#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <set>
#include <map>

#include "tasks_queue.h"
#include "html_parser.h"

class thread_pool;

class url_processing_thread : public std::thread
{
public:		
	bool in_work = false;
	url_item thread_task;
};

class thread_pool
{
private:
	unsigned int max_depth = 0;
	int total_pages_processed = 0;
	//std::vector<std::thread> th_vector;
	std::vector<url_processing_thread> th_vector;

	tasks_queue  pool_queue;	
	html_parser my_html_parser;
	
	std::atomic<bool> task_generator_finished(const int max_threads_num);
	bool start_work = false;
	
	std::mutex start_mutex;
	std::condition_variable start_threads;
	void submit(const url_item new_url_item, const int work_thread_num); //добавление адреса в очередь
	
	void work(const int& thread_index); //рабочая функция потоков
	bool process_next_task(const int& thread_index); //pool_queue_pop_next();
	bool work_function(const url_item& new_url_item, std::set<std::string>& new_urls_set, std::map<std::string, unsigned  int>& new_words_map);
	
public:	
	
	bool this_host_only = false;

	//thread_pool(const std::string& start_url, unsigned int _max_depth, const int max_threads_num, const int empty_thread_sleep_time, const int min_word_len, const int max_word_len);
	thread_pool(const std::string& start_url, unsigned int _max_depth, const int max_threads_num, const int empty_thread_sleep_time, const int min_word_len, const int max_word_len);
	~thread_pool();	

	std::string get_queue_state();

	void print_threads_state(); 

	void start_threads_work(); //старт рабочих потоков	
};

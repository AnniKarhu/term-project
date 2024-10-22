#pragma once
#include <iostream>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <string>
#include <set>
//#include <map>

//#include "html_parser.h"

struct url_item
{
	std::string url;
	int url_depth;
};


class tasks_queue
{
private:
	
	std::queue<url_item>   urls_queue;
	//html_parser my_html_parser;
	 
	std::mutex queue_mutex;
	std::condition_variable data_cond;
	//bool work_function(const url_item& new_url_item, std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map);


public:	
	unsigned int empty_sleep_for_time = 100;
	std::set<std::string> list_of_urls;
	//std::map<std::string, std::string> map_of_urls;

	void sq_push(const url_item& new_url_item, const int work_thread_num);
	bool sq_pop(url_item &task, const int work_thread_num);  //(std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map, const int work_thread_num);
	bool not_empty(); // (const int work_thread_num);
	bool is_empty(); // (const int work_thread_num);

	//void set_max_depth(unsigned int _max_depth) { 	max_depth = _max_depth; }

	int get_queue_size(); //{ return urls_queue.size(); }
	
	//std::string get_queue_state();
	
};

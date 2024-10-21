#pragma once
#include <iostream>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <string>
#include <set>
#include <map>

#include "html_parser.h"

struct url_item
{
	std::string url;
	int url_depth;
};


class tasks_queue
{
private:
	unsigned int max_depth = 0;
	std::queue<url_item>   urls_queue;
	html_parser my_html_parser;
	 
	std::mutex queue_mutex;
	std::condition_variable data_cond;
	bool work_function(const url_item& new_url_item, std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map);

	std::map<std::string, unsigned int> get_words_from_html_page();

public:	
	std::set<std::string> list_of_urls;

	void sq_push(const url_item& new_url_item); 
	bool sq_pop(std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map);
	bool not_empty();
	bool is_empty();

	void set_max_depth(unsigned int _max_depth) { 	max_depth = _max_depth; }
	
};

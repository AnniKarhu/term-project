#pragma once
#include <iostream>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <string>



struct url_item
{
	std::string url;
	int url_depth;
};


class tasks_queue
{
private:
	//std::queue<std::function<void()>>   work_queue;	
	std::queue<url_item>   urls_queue;
	std::mutex queue_mutex;
	std::condition_variable data_cond;
	void work_function(url_item new_url_item);  //std::string url_str, int url_depth);

public:	
	
	void sq_push(url_item new_url_item); // , std::string func_name, int func_count);
	void sq_pop();
	bool not_empty();
	bool is_empty();
	
};

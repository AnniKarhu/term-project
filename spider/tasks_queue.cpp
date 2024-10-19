  
#include <chrono>

#include "tasks_queue.h"



using namespace std::chrono_literals;


	void tasks_queue::sq_push(url_item new_url_item)
	{
		std::lock_guard<std::mutex> lk(queue_mutex);

		urls_queue.push(new_url_item);
		//std::cout << "new task added: " << new_url_item.url	<< " depth = " << new_url_item.url_depth << "\n";
		data_cond.notify_all();
	}

	void tasks_queue::sq_pop()
	{
		if (is_empty())
			return;

		std::unique_lock lk(queue_mutex);

		data_cond.wait(lk, [this] {return (!(urls_queue.empty())); });

		url_item task;
		bool task_ready = false;		

		if (!(urls_queue.empty()))
		{
			task = urls_queue.front();
			urls_queue.pop();
			task_ready = true;
			std::cout << "task pop: url = " << task.url << " depth = " << task.url_depth << std::endl;	
			//work_function(task);
			
		}

		lk.unlock();
		if (task_ready)
		{
			work_function(task);
		}
		std::this_thread::sleep_for(1s);
		//std::cout << "end pop: url = \n"; 
	}

	bool tasks_queue::not_empty()
	{
		std::lock_guard<std::mutex> lk(queue_mutex);
		return !urls_queue.empty();
	}

	bool tasks_queue::is_empty()
	{
		std::lock_guard<std::mutex> lk(queue_mutex);
		return urls_queue.empty();
	}

	void tasks_queue::work_function(url_item new_url_item)
	{		
		std::cout << "work_function: " <<  " url = " << new_url_item.url << " depth = " << new_url_item.url_depth << std::endl;
	}



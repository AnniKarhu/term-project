  
#include <chrono>
//#include <map>
//#include <set>

#include "tasks_queue.h"
//#include "https_req.h"
//#include "html_parser.h"

using namespace std::chrono_literals;


void tasks_queue::sq_push(const url_item& new_url_item, const int work_thread_num)
{
		std::lock_guard<std::mutex> lk(queue_mutex);
		//std::cout << "Thread " << work_thread_num << " lock queue_mutex - push\n";

		int queue_length = list_of_urls.size();
		list_of_urls.insert(new_url_item.url);

		if (list_of_urls.size() > queue_length)
		{
			urls_queue.push(new_url_item);
			std::cout << "New url added: url = " << new_url_item.url << " depth = " << new_url_item.url_depth << "\n";
		}
				
		//std::cout << "new task added: " << new_url_item.url	<< " depth = " << new_url_item.url_depth << "\n";
		data_cond.notify_all();
		//std::cout << "Thread " << work_thread_num << " release queue_mutex\n";
}

bool tasks_queue::sq_pop(url_item& task, const int work_thread_num) //(std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map, const int work_thread_num)
{
	if (is_empty())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(empty_sleep_for_time));
		return false;
	}
			

		std::unique_lock lk(queue_mutex);
		//std::cout << "Thread " << work_thread_num << " lock queue_mutex - pop\n";

		data_cond.wait(lk, [this] {return (!(urls_queue.empty())); });

		if (!(urls_queue.empty()))
		{
			task = urls_queue.front();
			urls_queue.pop();
			
			std::cout << "thread " << work_thread_num << ":  task pop : url = " << task.url << " depth = " << task.url_depth << std::endl;							
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(empty_sleep_for_time));			
		}

		//std::cout << "Thread " << work_thread_num << " release queue_mutex\n";
		lk.unlock();
		
		return true;		
}


bool tasks_queue::not_empty() //(const int work_thread_num)
{
		//std::cout << "Thread  lock queue_mutex - not_empty\n";
		std::lock_guard<std::mutex> lk(queue_mutex);
		//std::cout << "Thread  release queue_mutex\n";
		return !urls_queue.empty();
}

bool tasks_queue::is_empty() //(const int work_thread_num)
{
		//std::cout << "Thread  lock queue_mutex - is_empty\n";
		std::lock_guard<std::mutex> lk(queue_mutex);
		//std::cout << "Thread  release queue_mutex\n";
		return urls_queue.empty();
}



int tasks_queue::get_queue_size()
{ 
	//std::unique_lock lk(queue_mutex);
	std::lock_guard<std::mutex> lk(queue_mutex);
	return urls_queue.size(); 
	//lk.unlock();
}



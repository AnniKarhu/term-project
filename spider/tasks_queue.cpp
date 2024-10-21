  
//#include <chrono>
#include <map>
#include <set>

#include "tasks_queue.h"
#include "https_req.h"
#include "html_parser.h"

using namespace std::chrono_literals;


void tasks_queue::sq_push(const url_item& new_url_item)
{
		std::lock_guard<std::mutex> lk(queue_mutex);

		int queue_length = list_of_urls.size();
		list_of_urls.insert(new_url_item.url);

		if (list_of_urls.size() > queue_length)
		{
			urls_queue.push(new_url_item);
		}
				
		//std::cout << "new task added: " << new_url_item.url	<< " depth = " << new_url_item.url_depth << "\n";
		data_cond.notify_all();
}

bool tasks_queue::sq_pop(std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map)
{
		if (is_empty())
			return false;

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
		}

		lk.unlock();
		
		if (task_ready)
		{
			if (work_function(task, new_urls_set, new_words_map))
			{				
				for (auto& el : new_urls_set)
				{
					sq_push(url_item(el, task.url_depth + 1));
				}								
				
				return true;
			}
		}
		
		return false;		
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

bool tasks_queue::work_function(const url_item& new_url_item, std::set<std::string>& new_urls_set, std::map<std::string, unsigned int>& new_words_map)
{		
		//std::cout << "work_function: " <<  " url = " << new_url_item.url << " depth = " << new_url_item.url_depth << std::endl;

		http_req* html_request = new http_req(new_url_item.url);
		 
		if (!html_request->check_url())
		{
			delete html_request;
			html_request = new https_req(new_url_item.url);
			if (!html_request->check_url())
			{
				std::cout << "bad url: " << new_url_item.url << " - mark it as invalid\n";
				delete html_request;
				return false;
			}
		}

		if (html_request->get_page())
		{			
			switch (html_request->get_request_result())
			{
			case request_result::req_res_ok:
			{
				if (new_url_item.url_depth < max_depth)
				{
					std::string base_host = my_html_parser.get_base_host(new_url_item.url);
					new_urls_set = my_html_parser.get_urls_from_html(html_request->get_html_body_str(), base_host);

					if (new_urls_set.size() == 0)
					{
						std::cout << "no links got from page " << new_url_item.url << "\n";
					}
					//удалить после отладки
					for (auto& el : new_urls_set)
					{
						std::cout << "new  url = " << el << "depth = " << new_url_item.url_depth +1 << "\n";
					}
				}

				new_words_map = get_words_from_html_page();
				break;
			}
			case request_result::req_res_redirect:
			{
				std::string redirection_url = html_request->get_redirected_location();
				new_urls_set.insert(redirection_url);  (html_request->get_redirected_location());
				//удалить после отладки
				for (auto& el : new_urls_set)
				{
					std::cout << "new  url redirected = " << el << "depth = " << new_url_item.url_depth + 1 << "\n";
				}
				break;
			}

			default: { return false; }
			}			 
		};
		//std::cout << "html get result = " << int(html_request->get_request_result()) << "\n";

		delete html_request;
		return true;
			
}


std::map<std::string, unsigned int> tasks_queue::get_words_from_html_page()
{
	std::map<std::string, unsigned int> new_word_map;

	//продолжить здесь
	return new_word_map;
}



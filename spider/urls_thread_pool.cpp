#include <regex>

#include "urls_thread_pool.h"
#include "https_req.h"

thread_pool::thread_pool(const std::string& start_url, unsigned int _max_depth, const int max_threads_num,  const int empty_thread_sleep_time)
{	
		std::cout << "main thread id = " << std::this_thread::get_id() << std::endl;
		const auto cores = std::thread::hardware_concurrency();
		std::cout << "Total threads number = " << cores << "\n";
		
		pool_queue.empty_sleep_for_time = empty_thread_sleep_time;

		int cores_num = ((cores - 1) < max_threads_num ? (cores - 1) : max_threads_num);
		std::cout << "Work threads number = " << cores_num << "\n";

		//for (int i = 0; i < cores - 1; ++i) //1 основной поток		
		for (int i = 0; i < cores_num; ++i) 
		{
			th_vector.push_back(url_processing_thread(std::thread(&thread_pool::work, this, i)));
			std::cout << "work thread id = " << th_vector[i].get_id() << " created\n";
		}

			submit(url_item(start_url, 1), -1);
			//pool_queue.set_max_depth(max_depth);
			max_depth = _max_depth;
}

thread_pool::~thread_pool()
{
		for (auto& el : th_vector)
		{				
			auto thr_id = el.get_id();  			
			el.join();			
			std::cout << "work thread id = " << thr_id << " finished\n";
		}		
}

void thread_pool::work(const int& thread_index)
{
		std::unique_lock lk(start_mutex);		
		start_threads.wait(lk, [this] {return start_work; });
		
		std::cout <<  "start working id: " << std::this_thread::get_id() << " thread_index = " << thread_index << " \n";
		lk.unlock();


		while (!task_generator_finished(thread_index))
		{
			if (process_next_task(thread_index)) // pool_queue_pop_next(thread_index);
			{
				lk.lock();
					//std::cout << "Thread " << thread_index << " lock queue_mutex - pop get queue state\n";
					std::cout << get_queue_state();
					//std::cout << "Thread " << thread_index << " release queue_mutex\n";
				lk.unlock();
				print_threads_state(); //удалить после отладки?
			}			
		}		
}

bool thread_pool::process_next_task(const int& thread_index) //  pool_queue_pop_next(const int& thread_index)
{
	std::set<std::string> new_urls_set;
	std::map<std::string, unsigned int> new_words_map;

	bool result = false;

	th_vector[thread_index].in_work = true;
	url_item task;
	if (pool_queue.sq_pop(task, thread_index))// (new_urls_set, new_words_map, thread_index))
	{	
		if (validate_task(task.url))
		{
			th_vector[thread_index].thread_task = task; 

			if (work_function(task, new_urls_set, new_words_map))
			{
				for (auto& el : new_urls_set)
				{
					pool_queue.sq_push(url_item(el, task.url_depth + 1), thread_index);
				}

				std::string out_str = "url " + task.url + " " + std::to_string(task.url_depth) + " thread " + std::to_string(thread_index) + " finished\n";
				std::cout << out_str;
				std::cout << get_queue_state();				
			}
			result = true;
			total_pages_processed++;
		}		
	}
	th_vector[thread_index].in_work = false;

	return result;
}
	
void thread_pool::submit(const url_item new_url_item, const int work_thread_num) //добавление адреса в очередь
{
		pool_queue.sq_push(new_url_item, work_thread_num);
}

//старт рабочих потоков
void thread_pool::start_threads_work()
{
		start_work = true;
		start_threads.notify_all();
}

		
std::atomic<bool> thread_pool::task_generator_finished(const int max_threads_num)
{
		if (pool_queue.not_empty())
			return false;

		for (auto& el : th_vector)
		{
			if (el.in_work)
				return false;			
		}

		return true;
}

void thread_pool::print_threads_state() //удалить после отладки?
{
	std::string res_str = "_________________Threads state:\n";
	
	for (int i = 0; i < th_vector.size(); ++i)
	{
		res_str += "Thread num " + std::to_string(i);
		if (th_vector[i].in_work)
		{
			res_str +=  " - is in work: " + th_vector[i].thread_task.url + " depth = " + std::to_string(th_vector[i].thread_task.url_depth)  + " \n";
		}
		else
		{
			res_str +=  " - is idle\n";
		}
	}

	res_str +=  "\n";
	std::cout << res_str;
}


bool thread_pool::work_function(const url_item& new_url_item, std::set<std::string> &new_urls_set,  std::map<std::string, unsigned int> &new_word_map)
{
	//std::cout << "work_function: " <<  " url = " << new_url_item.url << " depth = " << new_url_item.url_depth << std::endl;


	http_req* html_request = new http_req(new_url_item.url);

	if (!html_request->check_url())
	{
		delete html_request;
		html_request = new https_req(new_url_item.url);
		if (!html_request->check_url())
		{
			std::string out_str = "bad url: " + new_url_item.url + " - mark it as invalid\n";
			std::cout << out_str;
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
			if (new_url_item.url_depth < (max_depth -1))
			{
				std::string base_host = my_html_parser.get_base_host(new_url_item.url);
				new_urls_set.clear();
				new_urls_set = my_html_parser.get_urls_from_html(html_request->get_html_body_str(), base_host);

				std::string text_str = my_html_parser.clear_tags(html_request->get_html_body_str());
				std::cout << text_str << "\n____________________\n";

				text_str - записать все слова в map
					проверить, почему ссылки добавляются с двойными слешами

				if (new_urls_set.size() == 0)
				{
					std::cout << "no links got from page " << new_url_item.url << "\n";
				}
				//удалить после отладки
				//for (auto& el : new_urls_set)
				//{
				//	std::cout << "new  url = " << el << " depth = " << new_url_item.url_depth +1 << "\n";
				//}
			}

			new_word_map.clear();
			new_word_map = get_words_from_html_page();
			
			break;
		}
		case request_result::req_res_redirect:
		{
			std::string redirection_url = html_request->get_redirected_location();
			new_urls_set.insert(redirection_url); // (html_request->get_redirected_location());
			//удалить после отладки
			for (auto& el : new_urls_set)
			{
				std::cout << "new  url redirected = " << el << " depth = " << new_url_item.url_depth + 1 << "\n";
			}
			break;
		}

		default: {
					delete html_request;
					return false; 
		
				}
		}
	};
	//std::cout << "html get result = " << int(html_request->get_request_result()) << "\n";

	delete html_request;
	return true;
}

std::map<std::string, unsigned int> thread_pool::get_words_from_html_page()
{
	std::map<std::string, unsigned int> new_word_map;

	//продолжить здесь
	return new_word_map;
}

std::string thread_pool::get_queue_state()
{
	std::string res_str = "\n________Indexing state:\n";
	res_str += "urls in queue to be processed: " + std::to_string(pool_queue.get_queue_size()) + "\n"; //    urls_queue.size()) + "\n";
	res_str += "urls already have been processed: " + std::to_string(total_pages_processed) + "\n";
	res_str += "total urls in list: " + std::to_string(pool_queue.list_of_urls.size()) + "\n";
	res_str += "\n\n";

	return res_str;
}

bool thread_pool::validate_task(const std::string& new_task) //запретить загружать страницы, не являющиеся html/text
{
	
	//нужно бы сделать проверку на тип файла, чтобы не скачивать лишние архивы, установочные файлы и т.п.
	//но видимо, это выходит за рамки дипломной работы
	return true;
	
	//std::vector<std::string> forbidden_str{ "zip", ".rar", "7z", ".doc", ".docx", ".xls", ".xlsx" }; //		ZIP, ARJ, RAR, CAB, TAR, LZH
	
	std::smatch res;
	std::string regex_str = "";
	std::regex r(regex_str);

	if (regex_search(new_task, res, r))
		return false;
}




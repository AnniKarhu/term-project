#include <regex>

#include "Spider.h"
#include "https_req.h" 

Spider::Spider(Search_parameters spider_data)
{		
		db_connection_string = spider_data.db_connection_string;		

		pool_queue.empty_sleep_for_time = spider_data.empty_thread_sleep_time;
		max_depth = spider_data.search_depth;
		this_host_only = spider_data.this_host_only;
		max_threads_num = spider_data.max_threads_num;
		start_url = spider_data.start_url;
		my_html_parser.min_word_len = spider_data.min_word_length;
		my_html_parser.max_word_len = spider_data.max_word_length;				
}

Spider::~Spider()
{
		for (auto& el : th_vector)
		{				
			auto thr_id = el.get_id();  			
			el.join();			
			std::cout << "work thread id = " << thr_id << " finished\n";
		}	

		delete data_base;

		std::cout << "\nIndexing finished. Total " << pool_queue.list_of_urls.size() << " pages were processed:\n";
		print_urls_list();
}

void Spider::work(const int& thread_index)
{
		std::unique_lock lk(threads_start_mutex);
			start_threads.wait(lk, [this] {return start_work; });		
			std::cout <<  "start working id: " << std::this_thread::get_id() << " thread_index = " << thread_index << " \n";
		lk.unlock();

		while (!task_generator_finished(thread_index))
		{
			if (process_next_task(thread_index)) 
			{
				lk.lock();					
					std::cout << get_queue_state();					
				lk.unlock();
				std::cout << get_threads_state();
			}			
		}		
}

bool Spider::process_next_task(const int& thread_index) 
{
	std::set<std::string> new_urls_set;
	std::map<std::string, unsigned int> new_words_map;

	bool result = false;	

	url_item task;
	if (pool_queue.sq_pop(task, thread_index)) //если взято новое задание
	{	
			th_vector[thread_index].in_work = true;  //флаг, что поток в процессе работы
			th_vector[thread_index].thread_task = task; 

			if (work_function(task, new_urls_set, new_words_map))
			{
				add_url_words_to_database(task.url, new_words_map); //добавить пройденный url и список слов в базу данных
				
				//добавить список полученных страниц в очередь на сканирование
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
	
	th_vector[thread_index].in_work = false;

	return result;
}
	
void Spider::submit(const url_item new_url_item, const int work_thread_num) //добавление адреса в очередь
{
		pool_queue.sq_push(new_url_item, work_thread_num);
}

void Spider::start_spider()
{
	data_base = new Data_base(db_connection_string);

	if (!data_base->start_db())
	{
		std::cout << "Database not started. Further work is impossible. \n";

		data_base->print_last_error(); //вывести информацию о последней ошибке		
		return;
	}
	
	start_work_threads(); //создать и запустить рабочие потоки

}

void Spider::start_work_threads() //создать и запустить рабочие потоки
{
	const auto cores = std::thread::hardware_concurrency();
	std::cout << "Total threads number = " << cores << "\n";
	int cores_num = ((cores - 1) < max_threads_num ? (cores - 1) : max_threads_num); //1 основной поток
	std::cout << "Work threads number = " << cores_num << "\n";

	for (int i = 0; i < cores_num; ++i) 
	{
		th_vector.push_back(url_processing_thread(std::thread(&Spider::work, this, i)));
		std::cout << "work thread id = " << th_vector[i].get_id() << " created\n";
	}

	submit(url_item(start_url, 1), -1);

	start_work = true;
	start_threads.notify_all();
}

		
std::atomic<bool> Spider::task_generator_finished(const int thread_num)
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

bool Spider::work_function(const url_item& new_url_item, std::set<std::string> &new_urls_set,  std::map<std::string, unsigned int>& new_words_map)
{
		
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
			if (new_url_item.url_depth <= (max_depth - 1))
			{
				std::string base_host = my_html_parser.get_base_host(new_url_item.url);
				new_urls_set.clear();
				new_urls_set = my_html_parser.get_urls_from_html(html_request->get_html_body_str(), base_host, this_host_only, new_url_item.url);
				
				if (new_urls_set.size() == 0)
				{
					std::string out_str = "no links got from page " + new_url_item.url + "\n";
					std::cout << out_str;
				}				
			}
			
			std::string text_str = my_html_parser.clear_tags(html_request->get_html_body_str());				
			new_words_map.clear();
			new_words_map = my_html_parser.collect_words(text_str);
			break;
		}
		case request_result::req_res_redirect:
		{
			std::string redirection_url = html_request->get_redirected_location();
			new_urls_set.insert(redirection_url); 
			
			break;
		}

		default: {
					delete html_request;
					return false; 		
				}
		}
	};
	
	delete html_request;
	return true;
}

std::string Spider::get_queue_state()
{
	std::string res_str = "\n________Indexing state:\n";
	res_str += "urls in queue to be processed: " + std::to_string(pool_queue.get_queue_size()) + "\n"; //    urls_queue.size()) + "\n";
	res_str += "urls already have been processed: " + std::to_string(total_pages_processed) + "\n";
	res_str += "total urls in list: " + std::to_string(pool_queue.list_of_urls.size()) + "\n";
	res_str += "\n\n";

	return res_str;
}

std::string Spider::get_threads_state()
{
	std::string res_str = "\n________Threads state:\n";

	for (int i = 0; i < th_vector.size(); ++i)
	{
		res_str += "Thread num " + std::to_string(i);
		if (th_vector[i].in_work)
		{
			res_str += " - is in work: " + th_vector[i].thread_task.url + " depth = " + std::to_string(th_vector[i].thread_task.url_depth) + " \n";
		}
		else
		{
			res_str += " - is idle\n";
		}
	}

	res_str += "\n";
	return res_str;
}

void Spider::print_urls_list()
{
	for (auto& el : pool_queue.list_of_urls)
	{
		std::cout << el << "\n";
	}
}

bool Spider::add_url_words_to_database(const std::string& url_str, const std::map<std::string, unsigned  int>& words_map)
{
	data_base->add_new_url(url_str);	
	int url_id = data_base->get_url_id(url_str);
	if (url_id < 0)
	{
		return false;	
	}

	for (auto& el : words_map)
	{
		data_base->add_new_word(el.first);
		int word_id = data_base->get_word_id(el.first);
		if (word_id < 0)
		{
			return false;
		}

		if (data_base->get_word_url_exist(url_id, word_id))//узнать, существует ли запись с id слова и урла
		{
			 data_base->update_word_url_pair(url_id, word_id, el.second); //	существует - запрос на изменение
		}
		else
		{
			data_base->add_new_word_url_pair(url_id, word_id, el.second); //	если не существует - запрос на вставку
		}	
		
	}

	return true;
}







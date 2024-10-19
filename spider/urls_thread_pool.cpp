#include "urls_thread_pool.h"

	thread_pool::thread_pool()
	{	
		std::cout << "main thread id = " << std::this_thread::get_id() << std::endl;
		const auto cores = std::thread::hardware_concurrency();
		std::cout << "Total threads number = " << cores << "\n";
		std::cout << "Work threads number = " << cores - 1 << "\n";

		//for (int i = 0; i < cores - 1; ++i) //1 основной поток
			for (int i = 0; i < 4; ++i) //1 основной поток
		{
			th_vector.push_back(url_processing_thread(std::thread(&thread_pool::work, this, i)));
			std::cout << "work thread id = " << th_vector[i].get_id() << " created\n";

			/*th_vector.push_back(std::thread(&thread_pool::work, this));
			std::cout << "work thread id = " << th_vector[i].get_id() << " created\n";*/

		}

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

	void thread_pool::work(int thread_index)
	{
		std::unique_lock lk(start_mutex);		
		start_threads.wait(lk, [this] {return start_work; });
		std::cout << "start working id: " << std::this_thread::get_id() << " thread_index" << thread_index << " \n";
		lk.unlock();

		while (!task_generator_finished())
		{
			th_vector[thread_index].in_work = true;
			pool_queue.sq_pop();
			th_vector[thread_index].in_work = false;
		}
		while (pool_queue.not_empty())
		{
			th_vector[thread_index].in_work = true;
			pool_queue.sq_pop();
			th_vector[thread_index].in_work = false;
		}
	}	

	
	//void thread_pool::submit(std::function<void()> new_task, std::string func_name, int func_count)//добавление задачи в очередь
	void thread_pool::submit(url_item new_url_item) //добавление адреса в очередь
	{
		pool_queue.sq_push(new_url_item);
	}

	//старт рабочих потоков
	void thread_pool::start_threads_work()
	{
		start_work = true;
		start_threads.notify_all();

		//finish_tasks(); //флаг завершения генерирования задач

	}

	//флаг завершения генерирования задач
	/*void thread_pool::finish_tasks()
	{
		task_generator_finished = true;
	}*/


	
	std::atomic<bool> thread_pool::task_generator_finished()
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

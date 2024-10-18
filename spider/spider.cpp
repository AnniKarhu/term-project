#include "spider.h"

//#include "http_req.h"
#include "https_req.h"

//Spider::Spider(Data_base* _data_base)
//{
//	if (!_data_base)
//	{
//		spider_invalid = true;
//		return;
//	}
//		
//	data_base = _data_base;
//	urls_queue = new std::set<std::string>;
//}

Spider::Spider()  noexcept
{	
	
}

Spider::Spider(const Spider& other) // конструктор копирования
{
	spider_invalid = other.spider_invalid; //есть ли проблемы с парсером
	data_base = other.data_base;

	urls_queue = new std::set<std::string>;
	*urls_queue = *(other.urls_queue);	
}

	
Spider& Spider::operator = (const Spider& other)	// оператор копирующего присваивания
{
	if (this != &other)
	{
		return *this = Spider(other);
	}

	return *this;
}

Spider::Spider(Spider&& other) noexcept			// конструктор перемещения
{
	spider_invalid = other.spider_invalid; //есть ли проблемы с пауком
	
	urls_queue = other.urls_queue;
	other.urls_queue = nullptr;	

	data_base = other.data_base;
	other.data_base = nullptr;

}

Spider& Spider::operator=(Spider&& other) noexcept // оператор перемещающего присваивания
{
	return *this = Spider(other);
}

Spider::~Spider()
{
	delete urls_queue;
	delete data_base;
}

bool Spider::prepare_spider(Spider_data start_data) //старт паука
{
	urls_queue = new std::set<std::string>;
	
	search_depth = start_data.search_depth;
	min_word_length = start_data.min_word_length;
	max_word_length = start_data.max_word_length;

	data_base = new Data_base(start_data.db_connection_string);
	if (data_base->start_db())
	{
		std::cout << "db started" << "\n";
	}
	else
	{
		std::cout << "db not started" << "\n";
		data_base->print_last_error(); //вывести информацию о последней ошибке
		spider_invalid = true; //есть проблемы с пауком
		std::cout << "Further work is impossible. The program will be closed.\n";
		return false;
	}

	try
	{		
		if (!data_base->test_insert())
		{
			std::cout << "test insert failed: " << data_base->get_last_error_desc();
		};

	}
	catch (...)
	{
		std::cout << "test insert failed, unknown reason" << "\n";
	}

	//urls_queue->insert(start_data.start_url);

	//только для отладки - удалить
	urls_queue->insert("www.1werwww.rt/");
	urls_queue->insert("https://www.google.com/");
	urls_queue->insert("https://example.com/");
	urls_queue->insert("http:///example.com");
	urls_queue->insert("https://example.com/hjlkj");
	
	return true;
}

void Spider::start_spider() //старт паука
{
	if (!urls_queue)
	{
		spider_invalid = true; //есть проблемы с пауком
		return;
	}

	for (auto const& url : *urls_queue)
	{		
		std::cout <<  "\n\nnext url = " << url << "\n";
		http_req* html_request = new http_req(url);
		if (!html_request->check_url())
		{
			delete html_request;
			html_request = new https_req(url);
			if (!html_request->check_url())
			{
				std::cout << "bad url: " << url << " - mark it as invalid\n";
				delete html_request;
				continue;
			}
		}

		html_request->get_page();
		std::cout << "html get result = " << int(html_request->get_request_result()) << "\n";

		delete html_request;
	}

	теперь добавить больше адресов в очередь и реализовать очередь потоков
		

}

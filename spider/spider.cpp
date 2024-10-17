#include "spider.h"

#include "http_req.h"

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

Spider::Spider(const Spider& other) // ����������� �����������
{
	spider_invalid = other.spider_invalid; //���� �� �������� � ��������
	data_base = other.data_base;

	urls_queue = new std::set<std::string>;
	*urls_queue = *(other.urls_queue);	
}

	
Spider& Spider::operator = (const Spider& other)	// �������� ����������� ������������
{
	if (this != &other)
	{
		return *this = Spider(other);
	}

	return *this;
}

Spider::Spider(Spider&& other) noexcept			// ����������� �����������
{
	spider_invalid = other.spider_invalid; //���� �� �������� � ������
	
	urls_queue = other.urls_queue;
	other.urls_queue = nullptr;	

	data_base = other.data_base;
	other.data_base = nullptr;

}

Spider& Spider::operator=(Spider&& other) noexcept // �������� ������������� ������������
{
	return *this = Spider(other);
}

Spider::~Spider()
{
	delete urls_queue;
	delete data_base;
}

bool Spider::prepare_spider(Spider_data start_data) //����� �����
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
		data_base->print_last_error(); //������� ���������� � ��������� ������
		spider_invalid = true; //���� �������� � ������
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

	urls_queue->insert(start_data.start_url);

	return true;

}

void Spider::start_spider() //����� �����
{
	if (!urls_queue)
	{
		spider_invalid = true; //���� �������� � ������
		return;
	}

	for (auto const& url : *urls_queue)
	{
		std::cout << "next url = " << url << "\n";
	}
		

}

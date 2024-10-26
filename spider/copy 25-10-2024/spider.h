#pragma once

#include <iostream>
#include <string>
#include <map>
#include <set>

#include "data_base.h"
#include "spider_data.h"

struct Search_parameters
{
	std::string start_url;
	int	search_depth = 1;
	int	min_word_length = 3;
	int	max_word_length = 32;
	int max_threads_num = 0;
	int empty_thread_sleep_time = 100;
	bool this_host_only = false;
};


class Spider
{
private:
	//������������ ������� ��������
	bool spider_invalid = false; //���� �������� � ������
	Data_base* data_base = nullptr;

	// information on urls
	//std	start_url = https://www.google.com/
	
	
	std::string db_connection_string;
	


private:
	//������ ����� ��� ����������
	std::set<std::string>* urls_queue = nullptr;

	bool test_database(); //������ ��� ������� - �������
	void test_get_html(); //������ ��� ������� - �������
	
public:
	
	Spider()  noexcept;
	Spider(const Spider& other);				// ����������� �����������	
	Spider& operator = (const Spider& other);	// �������� ����������� ������������
	Spider(Spider&& other) noexcept;			// ����������� �����������
	Spider& operator=(Spider&& other) noexcept; // �������� ������������� ������������
	~Spider();
	
	Search_parameters search_parameters;

	bool prepare_spider(Spider_data start_data); //����������  ����� � ������
	void start_spider_threads(); //����� ���� ������� �����

	//������� 1 ��� 2 ������ ��� ���������� ������� �� http � https
	//	�� ������ ������������� ��������� � ��������� ���������
	//	����� ������ https ://www.cyberforum.ru/boost-cpp/thread2383592.html

};
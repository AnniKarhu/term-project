#pragma once

#include <iostream>
#include <string>
#include <map>
#include <set>

#include "data_base.h"
#include "spider_data.h"


class Spider
{
private:
	//корректность запуска спайдера
	bool spider_invalid = false; //есть проблемы с пауком
	Data_base* data_base = nullptr;

	// information on urls
	//std	start_url = https://www.google.com/

	int	search_depth = 1;
	int	min_word_length = 3;
	int	max_word_length = 32;

private:
	//список урлов для индексации
	std::set<std::string>* urls_queue = nullptr;
	
public:
	
	Spider()  noexcept;
	Spider(const Spider& other);				// конструктор копирования	
	Spider& operator = (const Spider& other);	// оператор копирующего присваивания
	Spider(Spider&& other) noexcept;			// конструктор перемещения
	Spider& operator=(Spider&& other) noexcept; // оператор перемещающего присваивания
	~Spider();

	bool prepare_spider(Spider_data start_data); //подготовка  паука к работе
	void start_spider(); //старт паука

	//создать 1 или 2 класса для скачивания страниц по http и https
	//	не забыть анализировать заголовки и возможные редиректы
	//	поиск ссылок https ://www.cyberforum.ru/boost-cpp/thread2383592.html

};
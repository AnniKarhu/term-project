#include <iostream>
#include <pqxx/pqxx> 

//#include "Windows.h"
#include "ini_parser.h"
#include "data_base.h"
#include "server.cpp"
#include "data_base.h"

const std::string ini_file_name = "search_server.ini";
const std::string search_not_ready_str = "Search server is not ready to work.";

//------------------------------------------------------------------------------
int main()
{
    ini_parser parser;
    parser.fill_parser(ini_file_name);

    std::string address_str;
    std::string port_str;
	std::string db_connection_string;
	int search_results;

	try
	{
		address_str = parser.get_value<std::string>("Server.host");
		port_str = parser.get_value<std::string>("Server.port");

		db_connection_string = "host=" + parser.get_value<std::string>("Database.host") + " ";	//"host=127.0.0.1 "
		db_connection_string += "port=" + parser.get_value<std::string>("Database.port") + " ";	//"port=5432 "
		db_connection_string += "dbname=" + parser.get_value<std::string>("Database.dbname") + " ";	//"dbname=db_lesson05 "
		db_connection_string += "user=" + parser.get_value<std::string>("Database.user") + " ";	//"user=lesson05user "
		db_connection_string += "password=" + parser.get_value<std::string>("Database.password") + " ";//"password=lesson05user");

		search_results = parser.get_value<int>("Search_settings.search_results");
		
	}
	catch (const ParserException_no_file& ex)
	{
		std::cout << ex.what() << "\n";
		std::cout << search_not_ready_str << "\n";
		return 0;
	}
	catch (const ParserException_no_section& ex)
	{
		std::cout << ex.what() << "\n";
		std::cout << search_not_ready_str << "\n";
		return 0;
	}
	catch (const ParserException_no_variable& ex)
	{
		std::cout << ex.what() << "\n";
		std::cout << search_not_ready_str << "\n";
		return 0;
	}

	Data_base data_base(db_connection_string);
	if (!data_base.start_db())
	{
		std::cout << "Database not started. Further work is impossible. \n";

		data_base.print_last_error(); //вывести информацию о последней ошибке		
		return 0;
	}
    
    auto const address = net::ip::make_address(address_str);
    auto const port = static_cast<unsigned short>(std::stoi(port_str));

    auto const doc_root = std::make_shared<std::string>(".");
    auto const threads = 1; // std::max<int>(1, std::atoi("1"));

    // The io_context is required for all I/O
    net::io_context ioc{ threads };

    // Create and launch a listening port
    std::make_shared<listener>(
        ioc,
        tcp::endpoint{ address, port },
        doc_root,
		search_results,
		&data_base)->run();

	/*		или всегда держать базу открытой, а в листенер и сессион передавать только указатель на открытую базу данных
	5) не забыть предусмотреть закрытие базы данных и delete указателя
	6) или базу данных открыть в main без динамического выделения памяти? а в листенер и сессион передавать по ссылке?
	7) libpqx вынести отдельно, чтобы не иметь две папки
	8) модуль с базой данных и парсер ini вынести и оформит в dll, чтобы не править код в двух местах*/

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(
            [&ioc]
            {
                ioc.run();
            });
    ioc.run();

    return EXIT_SUCCESS;
}

//запрос из БД
//
//1. такая выборка включает только список адресов (уникальных), в которых встречаются искомые слова
////select distinct url from(select* from urls_words
////	inner  join  documents on  urls_words.url_id = documents.id) where word_id = 1 or word_id = 5 or word_id = 6;
//select distinct url from
//(select * from urls_words
//	inner  join
//	documents on  urls_words.url_id = documents.id) as Table_A
//	inner join
//	words on Table_A.word_id = words.id where word = 'example' or word = 'domain' or word = 'and';
//
//
//этот список сохранить в виде std::map map_urls_list <string, int> string - url, int multiplier = 0
//
//
//2. такая выборка содержит число - количество строк с урл = заданному
////select count(*) from(select * from(select * from urls_words
////	inner  join  documents on  urls_words.url_id = documents.id) where word_id = 1 or word_id = 5 or word_id = 6) where url = 'https://example.com/';
//
//select count(*) from
//(select* from urls_words
//	inner  join
//	documents on  urls_words.url_id = documents.id) as Table_A
//	inner join
//	words on Table_A.word_id = words.id where(word = 'example' or word = 'domain' or word = 'for') and url = 'https://example.com/';
//
//если это число = числу слов в запросе пользователя, установить multiplier = 1 для записи этого урла в std::map map_urls_list 
//
//3.
//такая выборка включает все поля и все строки	
////select* from(select* from urls_words
////	inner  join  documents on  urls_words.url_id = documents.id) where word_id = 1 or word_id = 5 or word_id = 6;
//select * from
//(select * from urls_words
//	inner  join
//	documents on  urls_words.url_id = documents.id) as Table_A
//	inner join
//	words on Table_A.word_id = words.id where(word = 'example' or word = 'domain' or word = 'for');
//
//такая запись хранить урлы и количество вхождений слов - урлы только те, в которых встречаются все слова
//select quantity, url from
//(select* from urls_words
//	inner  join
//	documents on  urls_words.url_id = documents.id) as Table_A
//	inner join
//	words on Table_A.word_id = words.id where(word = 'example' or word = 'domain' or word = 'for');
//
//создать новый результирующий std::map map_result
//
//перебрать все записи из этого результата:
//3.1 найти запись в std::map map_urls_list с соответсвующим url. Если его multiplier =0, перейти к следующей записи
//3.2 найти запись в std::map map_result с соответсвующим url и сохранить из него значение количества слов count.
//3.3. к количеству слов добавить значение quantity из выборки
//3.4. сохранить новое значение count для этого урл в map_result
//
//4. результат после обработки всех записей - std::map map_result
//5. std::map map_result нужно переписать в вектор tuple<string url, int count>
//6. отсортировать новый вектор по значению count
//сортировка вектора компаратором https ://ejudge.179.ru/tasks/cpp/total/242.html
//
//Пример реализации такой функции для сортировки значений по последней цифре :
//
//bool cmp(int a, int b) вместо a и b будут tuple<string, int>
//{
//	return a % 10 < b % 10; сравнивать буду int из туплов
//}
//
//Эта функция передается в функцию sort третьим параметром :
//vector a 
//sort(a.begin(), a.end(), cmp);
//
//
//
//
//объединенная выборка из базы по нужным словам запроса
//select* from
//(select* from urls_words
//	inner  join
//	documents on  urls_words.url_id = documents.id) as Table_A
//	inner join
//	words on Table_A.word_id = words.id where word = 'example' or word = 'domain' or word = 'and';





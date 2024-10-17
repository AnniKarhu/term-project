#include <iostream>
#include <pqxx/pqxx> 
#include "Windows.h"

#include "data_base.h"
#include "user_interface.h"

#include "ini_parser.h"
#include "spider.h"
#include "spider_data.h"

const std::string ini_file_name = "spider.ini";
const std::string spider_not_ready_str = "Spider is not ready to work.";

int main()
{
	std::cout << "Hello!\n";

	setlocale(LC_ALL, "rus");
	/*SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	system("chcp 1251");*/

	ini_parser parser;
	parser.fill_parser(ini_file_name);

	Spider_data spider_data;
	try
	{
		spider_data.start_url = parser.get_value<std::string>("URLs.start_url");
		
		spider_data.db_connection_string = "host="		+ parser.get_value<std::string>("Database.host") + " ";	//"host=127.0.0.1 "
		spider_data.db_connection_string += "port="		+ parser.get_value<std::string>("Database.port") + " ";	//"port=5432 "
		spider_data.db_connection_string += "dbname="	+ parser.get_value<std::string>("Database.dbname") + " ";	//"dbname=db_lesson05 "
		spider_data.db_connection_string += "user="		+ parser.get_value<std::string>("Database.user") + " ";	//"user=lesson05user "
		spider_data.db_connection_string += "password=" + parser.get_value<std::string>("Database.password") + " ";//"password=lesson05user");

		spider_data.search_depth = parser.get_value<int>("Search_settings.search_depth");
		spider_data.max_word_length = parser.get_value<int>("Search_settings.max_word_length");
		spider_data.min_word_length = parser.get_value<int>("Search_settings.min_word_length");		
		
	}
	catch (const ParserException_no_file& ex)
	{
		std::cout << ex.what() << "\n";
		std::cout << spider_not_ready_str << "\n";
		return 0;
	}
	catch (const ParserException_no_section& ex)
	{
		std::cout << ex.what() << "\n";
		std::cout << spider_not_ready_str << "\n";
		return 0;
	}
	catch (const ParserException_no_variable& ex)
	{
		std::cout << ex.what() << "\n";
		std::cout << spider_not_ready_str << "\n";
		return 0;
	}

			
	std::cout << "URLs information: " << std::endl;
	std::cout << "Start url = " << spider_data.start_url << "\n\n";
	std::cout << "Indexing settings: "  << std::endl;
	std::cout << "Search_depth = " << spider_data.search_depth << "\n";
	std::cout << "Minimum word length for indexing = " << spider_data.min_word_length << "\n";
	std::cout << "Maximum word length for indexing = " << spider_data.max_word_length << "\n\n";	
	std::cout << "Database settings: " << spider_data.db_connection_string << std::endl;		
	
	Spider spider;
	if (!spider.prepare_spider(spider_data))
	{
		std::cout << spider_not_ready_str << "\n";
		return 0;
	}
	else
	{
		std::cout << "Start indexing from " << spider_data.start_url << std::endl;
		spider.start_spider(); //старт паука
	}
				

	

return 0;
}



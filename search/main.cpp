#include <iostream>
#include <pqxx/pqxx> 

//#include "Windows.h"
#include "ini_parser.h"
#include "server.cpp"

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
        doc_root)->run();

	1) попробовать параметры базы данных и количетсов результатов на странице передавать через listener->run(db_connection_str, res_num)
    2) через run листенера добраться до session (метод on_accept)
	3) session->run - сюда нужно передать параметры базы данных и количество результатов поиска
	4) вопрос - где подключаться к базе данных - в листенер или в session?
		вероятно, коннект можно делать в session при обработке запроса get

		или всегда держать базу открытой, а в листенер и сессион передавать только указатель на открытую базу данных
	5) не забыть предусмотреть закрытие базы данных и delete указателя
	6) или базу данных открыть в main, статически без динамического выделения памяти? а в листенер и сессион передавать по ссылке?


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
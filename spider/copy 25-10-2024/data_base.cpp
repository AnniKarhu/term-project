#include <map>
#include <algorithm>
#include <tuple>

#include "Windows.h"

#include "data_base.h"


Data_base::Data_base(const std::string params_str) noexcept  : connection_str{ params_str } 
{

}

// конструктор перемещения
Data_base::Data_base(Data_base&& other) noexcept : connection_str{ other.connection_str } 	
{	
	connection_ptr = other.connection_ptr; //объект подключения
	last_error = other.last_error;  //описание последней возникшей ошибки

	other.connection_ptr = nullptr;	 
}

Data_base& Data_base::operator=(Data_base&& other) noexcept   // оператор перемещающего присваивания
{
	connection_ptr = other.connection_ptr; //объект подключения
	last_error = other.last_error;  //описание последней возникшей ошибки

	other.connection_ptr = nullptr;	
	return *this; 
}

Data_base::~Data_base()
{
	delete connection_ptr;
}

bool Data_base::connect_db() //выполнить подключение к БД
{
	try //при проблеме с подключением выбрасывает исключение
	{
		connection_ptr = new pqxx::connection(connection_str);
		return true;
	}
	catch (const pqxx::sql_error& e)
	{	
		//в этот блок не попадаем, все исключения ловит std::exception  
		last_error = e.what();
		return false;
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		return false;
	}
}

std::string Data_base::get_last_error_desc() //получить описание последней возникшей ошибки
{
	return last_error;
}

void Data_base::print_last_error() //вывести информацию о последней ошибке
{
	
	//все равно некоторые ошибки отображаются кракозябрами - вопрос с кодировками при работе с базой остается открытым
	
	std::cout << "Last error: " << last_error << "\n";

	//SetConsoleOutputCP(1251);
	//SetConsoleCP(1251);
}

bool Data_base::create_tables() //создать необходимые таблицы
{
	if ((connection_ptr == nullptr) || (!(connection_ptr->is_open())))
	{
		last_error = "Ошибка при создании таблиц базы данных - нет подключения к базе данных";
		return false;
	}

	try
	{
		pqxx::work tx{ *connection_ptr };

		//таблица urls
		tx.exec(
			"CREATE table IF NOT EXISTS documents ( "
			"id serial primary KEY, "
			"url varchar(250) NOT NULL  UNIQUE, "
			"constraint url_unique unique(url)); ");  

		//таблица слов
		tx.exec(
			"CREATE table IF NOT EXISTS words ( "
			"id serial primary KEY, "
			"word varchar(32) NOT NULL  UNIQUE, "
			"constraint word_unique unique(word)); ");

		//таблица mid		
		tx.exec(
			"CREATE table IF NOT EXISTS urls_words ( "
			"word_id integer references words(id), "
			"url_id integer references documents(id), "
			"quantity integer NOT NULL,"
			"constraint pk primary key(word_id, url_id)); ");		

		tx.commit();
		return true;
	}
	catch (...)
	{
		last_error = "Ошибка при создании таблиц базы данных";
		return false;
	}	
}

bool Data_base::create_templates() //создать шаблоны для работы
{
	if ((connection_ptr == nullptr) || (!(connection_ptr->is_open())))
	{
		last_error = "Ошибка при создании шаблонов запросов - нет подключения к базе данных";
		return false;
	}

	try
	{
		//добавление url
		connection_ptr->prepare("insert_url", "insert into documents (url) values ($1)");
		
		//добавление word
		//connection_ptr->prepare("insert_word", "insert into words (word) values ($1)");

		//добавление частоты
		//connection_ptr->prepare("insert_quantity", "insert into urls_words (word_id, url_id, quantity) values ($1, $2, $3)");

		//изменение частоты
		//connection_ptr->prepare("update_quantity", "update urls_words set quantitye = $3 where word_id = $1 and url_id = $2");
		
		//изменение клиента
		//connection_ptr->prepare("update_user", "update users set username = $2, userlastname = $3, email = $4 where id = $1");

		//удаление телефона клиента
		//connection_ptr->prepare("delete_phone", "delete from phones where user_id = $1 and phone = $2");

		//удаление всех телефонов клиента
		//connection_ptr->prepare("delete_user_phones", "delete from phones where user_id = $1");

		//удаление клиента
		//connection_ptr->prepare("delete_user", "delete from users where id = $1");

		//поиск клиента по его id
		//connection_ptr->prepare("search_user_by_id", "select * from users where id = $1");

		//поиск клиента по его имени
		//connection_ptr->prepare("search_user_by_name", "select * from users where username = $1");

		//поиск клиента по его фамилии
		//connection_ptr->prepare("search_user_by_lastname", "select * from users where userlastname = $1");

		//поиск клиента по его email
		//connection_ptr->prepare("search_user_by_email", "select * from users where email = $1");

		//id клиента по его номеру телефона
		//connection_ptr->prepare("search_user_id_by_phone", "select user_id from phones where phone = $1");

		//поиск номера телефона по id клиента
		//connection_ptr->prepare("select_phones_by_id", "select phone from phones where user_id = $1");		
		
		return true;
	}
	
	catch (...)
	{
		last_error = "Ошибка при создании шаблонов запросов";
		return false;
	}
}

//начало работы с базой данных
bool Data_base::start_db()
{
	bool result = false;

	if (connect_db()) //подключиться к базе
	{
		result = create_tables() and //создать необходимые таблицы
				 create_templates(); //создать шаблоны для работы; 
	}
		
	return result;
}

bool Data_base::test_insert() //убрать после отладки
{
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе данных";
		return false; 
	}

	try
	{
		if  (!(connection_ptr->is_open()))
		{
			return false; 
		}

		pqxx::work tx{ *connection_ptr };

		//добавление в таблицу пользователей
		tx.exec(
			"insert into documents (url) values "
			"('http://google.com/'), "
			"('http://google2.com/'), "
			"('http://google2.com/'); ");		

		tx.commit();		
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		return false;
	}
	catch (...)
	{
		last_error = "Ошибка при добавлении данных";
		return false;
	}

	return true;
	

}

/*взаимодействие пользователя с базой данных*/

//добавить нового пользователя
bool Data_base::add_new_user(std::string UserName, std::string UserLastName, std::string UserEmail) 
{
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе данных";
		return false;
	}
	
	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		tx.exec_prepared("insert_user", UserName, UserLastName, UserEmail);
		tx.commit();

		return true;
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		return false;
	}
}

//поиск клиентов по произвольной строке - имя, фамилия, email, телефон
std::map<int, std::tuple<std::string, std::string, std::string>> Data_base::get_users_list_by_string(std::string DefaultStr)
{
	//все записи с совпадением по имени
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map_name;
	result_map_name = get_users_list_by_name(DefaultStr);
	
	//все записи с совпадением по фамилии
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map_lastname;
	result_map_lastname = get_users_list_by_lastname(DefaultStr);
	
	//все записи с совпадением по email
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map_email;
	result_map_email = get_users_list_by_email(DefaultStr);
	
	//пользователь с совпадением по телефону
	std::tuple<int, std::string, std::string, std::string> result_tuple_phone;
	int res_id = get_user_id_by_phone(DefaultStr);	 
	result_tuple_phone = get_user_by_id(res_id);
	
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map;

	//объединить полученные мапы
	std::merge(result_map_name.begin(), result_map_name.end(), result_map_lastname.begin(), result_map_lastname.end(), std::inserter(result_map, result_map.begin()));
	std::merge(result_map.begin(), result_map.end(), result_map_email.begin(), result_map_email.end(), std::inserter(result_map, result_map.begin()));
	result_map[std::get<0>(result_tuple_phone)] = std::make_tuple(std::get<1>(result_tuple_phone), std::get<2>(result_tuple_phone), std::get<3>(result_tuple_phone));

	//удалить из результата вариант с id=-1, если он там есть
	result_map.erase(-1);

	return result_map;
}
 
//все пользователи с заданным именем
std::map<int, std::tuple<std::string, std::string, std::string>> Data_base::get_users_list_by_name(std::string UserName) 
{	
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map;
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе  данных";
		return result_map;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		auto query_res = tx.exec_prepared("search_user_by_name", UserName);	
		for (auto row : query_res)
		{
				auto temp_tuple =  std::make_tuple(row["username"].as<std::string>(), row["userlastname"].as<std::string>(), row["email"].as<std::string>());
			int temp_int = row["id"].as<int>();
		
			result_map[temp_int] =  temp_tuple;
		}
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();		
	}

	return result_map;		
}

//все пользователи с заданной фамилией
std::map<int, std::tuple<std::string, std::string, std::string>> Data_base::get_users_list_by_lastname(std::string UserLastName)
{
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map;
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе  данных";
		return result_map;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		auto query_res = tx.exec_prepared("search_user_by_lastname", UserLastName);
		for (auto row : query_res)
		{
			auto temp_tuple = std::make_tuple(row["username"].as<std::string>(), row["userlastname"].as<std::string>(), row["email"].as<std::string>());
			int temp_int = row["id"].as<int>();

			result_map[temp_int] = temp_tuple;
		}
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
	}

	return result_map;
}

//все пользователи с заданным email
std::map<int, std::tuple<std::string, std::string, std::string>> Data_base::get_users_list_by_email(std::string UserEmail)
{	
	std::map<int, std::tuple<std::string, std::string, std::string>> result_map;
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе  данных";
		return result_map;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		auto query_res = tx.exec_prepared("search_user_by_email", UserEmail);
		for (auto row : query_res)
		{
			auto temp_tuple = std::make_tuple(row["username"].as<std::string>(), row["userlastname"].as<std::string>(), row["email"].as<std::string>());
			int temp_int = row["id"].as<int>();

			result_map[temp_int] = temp_tuple;
		}
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
	}

	return result_map;
}

// id клиента с заданным телефоном
int Data_base::get_user_id_by_phone(std::string UserPhone)
{
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе  данных";
		return -1;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
	
		auto query_res = tx.exec_prepared("search_user_id_by_phone", UserPhone);
		if (query_res.empty())
		{
			return -1;
		}

		auto row = query_res.begin();
		int res_int = row["user_id"].as<int>();
		return res_int; //вернуть первый (он же должен быть и единственным) результат
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		return -1;
	}	
}

//получить информацию о клиенте по его id	
std::tuple<int, std::string, std::string, std::string> Data_base::get_user_by_id(int UserId)
{
	auto temp_tuple = std::make_tuple(-1, "", "", "");
	if (connection_ptr == nullptr)
	{
		last_error = "Нет подключения к базе  данных";
		return temp_tuple;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };

		auto query_res = tx.exec_prepared("search_user_by_id", UserId);	

		if (query_res.empty()) //|| //пустой результат
		//	(query_res.size() > 1)) //больше одной строки результат - значит, в базе  некорректные данные
		{
			return temp_tuple;
		}
		auto row = query_res.begin();
		auto result_tuple = std::make_tuple(row["id"].as<int>(), row["username"].as<std::string>(), row["userlastname"].as<std::string>(), row["email"].as<std::string>());
		return result_tuple; //вернуть первый (он же должен быть и единственным) результат	
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		return temp_tuple;
	}	
}

//получить список номеров телефонов по id клиента
std::vector<std::string> Data_base::get_phones_list_by_user_id(int UserId)
{
	std::vector<std::string> phones_vector;	  

	if (connection_ptr == nullptr)	{
		last_error = "Нет подключения к базе  данных";
		return  phones_vector;		
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		auto query_res = tx.exec_prepared("select_phones_by_id", UserId);

		for (auto row : query_res)
		{
			phones_vector.push_back(row["phone"].as<std::string>());		
		}
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();		
	}

	return  phones_vector;
}

//добавление телефона пользователю с известным id
bool Data_base::add_user_phone(int user_id, std::string user_phone)
{
	bool result = false;
	if (connection_ptr == nullptr) {
		last_error = "Нет подключения к базе  данных";
		return  false;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		tx.exec_prepared("insert_phone", user_id, user_phone);
		tx.commit();
		result = true;
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		result = false;
	}

	return result;	
}

//изменение данных пользователя с заданным id
bool Data_base::update_user_data(int user_id, std::string new_name, std::string new_lastname, std::string new_email)
{
	bool result = false;
	if (connection_ptr == nullptr) {
		last_error = "Нет подключения к базе  данных";
		return  false;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		tx.exec_prepared("update_user", user_id, new_name, new_lastname, new_email);		
		tx.commit();
		result = true;
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		result = false;
	}

	return result;	
}

//удаление известного номера телефона  клиента с заданным id
bool Data_base::delete_user_phone(int user_id, std::string user_phone)
{
	bool result = false;
	if (connection_ptr == nullptr) {
		last_error = "Нет подключения к базе  данных";
		return  false;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		tx.exec_prepared("delete_phone", user_id, user_phone);
		tx.commit();
		result = true;
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		result = false;
	}

	return result;
}

//удалить  клиента с заданным id
bool Data_base::delete_user(int user_id)
{
	bool result = false;
	if (connection_ptr == nullptr) {
		last_error = "Нет подключения к базе  данных";
		return  false;
	}

	last_error = "";
	try
	{
		pqxx::work tx{ *connection_ptr };
		tx.exec_prepared("delete_user_phones", user_id); //удаление всех номеров телефонов
		tx.exec_prepared("delete_user", user_id); //удаление записи о клиенте
		tx.commit();
		result = true;
	}
	catch (const std::exception& ex)
	{
		last_error = ex.what();
		result = false;
	}

	return result;
}


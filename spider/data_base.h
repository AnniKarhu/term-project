#pragma once
#include <iostream>
#include <pqxx/pqxx> 

class Data_base
{
private:
	const std::string connection_str; //строка параметров для подключения
	pqxx::connection* connection_ptr = NULL;
	std::string last_error; //описание последней возникшей ошибки
	bool connect_db(); //выполнить подключение к БД
	bool create_tables(); //создать необходимые таблицы
	bool create_templates(); //создать шаблоны для работы

public:

	explicit Data_base(const std::string params_str)  noexcept;
	
	bool start_db(); //начало работы с базой данных
	std::string get_last_error_desc(); //получить описание последней возникшей ошибки
	void print_last_error(); //вывести информацию о последней ошибке

	bool test_insert(); //убрать после отладки

	Data_base(const Data_base& other) = delete; //конструктор копирования
	Data_base(Data_base&& other) noexcept;	// конструктор перемещения
	Data_base& operator=(const Data_base& other) = delete;  //оператор присваивания 
	Data_base& operator=(Data_base&& other) noexcept;       // оператор перемещающего присваивания
	~Data_base();	

	/*взаимодействие пользователя с базой данных*/	

	/*информация о клиенте*/

	//получить информацию о клиенте по его id	
	std::tuple<int, std::string, std::string, std::string> get_user_by_id(int UserId); 
	
	//получить список номеров телефонов по id клиента
	std::vector<std::string> get_phones_list_by_user_id(int UserId); 
	
	//поиск клиентов по произвольной строке - имя, фамилия, email, телефон	
	std::map<int, std::tuple<std::string, std::string, std::string>> get_users_list_by_string(std::string DefaultStr);
	
	//поиск клиентов с заданным именем	
	std::map<int, std::tuple<std::string, std::string, std::string>> get_users_list_by_name(std::string UserName); 
	
	//поиск клиентов с заданной фамилией
	std::map<int, std::tuple<std::string, std::string, std::string>> get_users_list_by_lastname(std::string UserLastName);
	
	//поиск клиентов с заданным email	
	std::map<int, std::tuple<std::string, std::string, std::string>> get_users_list_by_email(std::string UserEmail); 
	
	//поиск id клиента с заданным телефоном
	int get_user_id_by_phone(std::string UserPhone);

	/*добавить или изменить информацию о клиенте*/

	//добавить нового клиента
	bool add_new_user(std::string UserName, std::string UserLastName, std::string UserEmail); 

	//добавление телефона клиента с известным id
	bool add_user_phone(int user_id, std::string user_phone);

	//изменение данных клиента с заданным id
	bool update_user_data(int user_id, std::string new_name, std::string new_lastname, std::string new_email);

	/*удаление информации о клиенте*/

	//удаление известного номера телефона  клиента с заданным id
	bool delete_user_phone(int user_id, std::string user_phone);

	//удалить  клиента с заданным id
	bool delete_user(int user_id);
};
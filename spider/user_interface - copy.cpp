#include <string>
#include <algorithm>
#include "user_interface.h"

//главное меню
int main_menu()
{
	std::cout << "\nДобро пожаловать в базу данных о клиентах. Введите номер интересующего Вас пункта меню:\n";
	std::cout << "1. добавить нового клиента\n";
	std::cout << "2. добавить телефон для существующего клиента\n";
	std::cout << "3. изменить данные о клиенте\n";
	std::cout << "4. удалить телефон у существующего клиента\n";
	std::cout << "5. удалить существующего клиента\n";
	std::cout << "6. найти клиента по его данным — имени, фамилии, email или телефону\n\n";
	std::cout << "0. Завершить работу\n";

	int user_chr = -1;
	while (user_chr < 0)
	{
		std::cin >> user_chr;
		if (std::cin.fail())  //ошибка ввода			
		{
			user_chr = -1;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Ваш выбор непонятен. Пожалуйста, введите номер интересующего Вас пункта меню: ";
		}		
	}
	return user_chr;
}

//добавить нового клиента
void add_new_user(Data_base& db_unit)
{
	std::string UserName;
	std::string UserLastName; 
	std::string UserEmail;
	std::cin.get();	

	while (UserName.length() < 1)
	{
		std::cout << "Введите имя клиента: ";	
		std::getline(std::cin, UserName);
	}
	
	while (UserLastName.length() < 1)
	{
		std::cout << "Введите фамилию клиента: ";
		std::getline(std::cin, UserLastName);
	}
	
	while (UserEmail.length() < 1)
	{
		std::cout << "Введите email клиента: ";
		std::getline(std::cin, UserEmail);	
	}	

	try
	{
		if (db_unit.add_new_user(UserName, UserLastName, UserEmail))
		{
			std::cout << "Новый клиент добавлен в базу данных \n";
			//return true;
		}
		else
		{
			std::cout << "К сожалению, новый клиент не добавлен.\n";
			db_unit.print_last_error(); //вывести информацию о последней ошибке
			//return false;
		}
	}
	catch (...)
	{
		std::cout << "Ошибка при добавлении клиента в базу" << "\n";
	}			
}

int get_search_param() //запросить у пользователя, по какому параметру ищем клиента
{
	std::cout << "\nЧто известно о клиенте? \n";
	std::cout << "1. Имя\n";
	std::cout << "2. Фамилия\n";
	std::cout << "3. email\n";
	std::cout << "4. Телефон\n";
	std::cout << "0. Что-то известно, но я не знаю, что это\n";

	int user_chr = -1;
	while (user_chr < 0)
	{
		std::cin >> user_chr;
		if ((std::cin.fail()) ||  //ошибка ввода	
			(user_chr < 0)||
			(user_chr > 4))
		{
			user_chr = -1;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Ваш выбор непонятен. Пожалуйста, введите номер пункта, по которому будем искать клиента: ";
		}
	}
	return user_chr;
}

void print_user_info(Data_base& db_unit) //вывод информации о клиенте
{
	//чтобы вывести информацию, нужно сначала найти клиента	
	int user_id = get_user_id(db_unit);

	//-1 - записи в базе не найдены
	if (user_id < 0)
	{
		std::cout << "В базе нет информации по этому клиенту \n";
		return;
	}

	//получить информацию о клиенте по его id
	auto user_tuple = db_unit.get_user_by_id(user_id);
	std::string u_name = std::get<1>(user_tuple);
	std::string u_lastname = std::get<2>(user_tuple);
	std::string u_email = std::get<3>(user_tuple);

	std::cout << "Найден клиент: id = " << user_id << ", имя =  " << u_name << ", фамилия = " << u_lastname << ", email = " << u_email << "\n";
	
	//получить информацию о имеющихся у клиента номерах телефона
	std::vector<std::string> phones_vector = db_unit.get_phones_list_by_user_id(user_id);

	if (phones_vector.empty())
	{
		std::cout << "Номера телефонов: не найдены\n";
	}
	else
	{
		for (const auto& elem : phones_vector)
			std::cout << elem << "\n";
	}
	
}

int get_user_id(Data_base& db_unit) //поиск id клиента
{	
	std::string user_info_str;
	std::map<int, std::tuple<std::string, std::string, std::string>> users_map;
	std::tuple<int, std::string, std::string, std::string> user_tuple;

	//по какому параметру будем искать
	int user_ch = get_search_param();
	switch (user_ch)
	{
	case 0: //по всем сразу
		user_info_str = get_string("Введите строку, которую знаете: "); //получить строку от пользователя
		users_map = db_unit.get_users_list_by_string(user_info_str); //списко клиентов, включающих указанную строку в каком-либо из параметров
		break;
	
	case 1: //по имени
		user_info_str = get_string("Введите имя клиента: "); //получить имя от пользователя
		users_map = db_unit.get_users_list_by_name(user_info_str); //списко клиентов с указанным именем
		break;

	case 2:  //по фамилии
		user_info_str = get_string("Введите фамилию клиента: "); //получить фамилию от пользователя
		users_map = db_unit.get_users_list_by_lastname(user_info_str); //списко клиентов с указанной фамилией
		break;

	case 3: //по email
		user_info_str = get_string("Введите email клиента: "); //получить email от пользователя
		users_map = db_unit.get_users_list_by_email(user_info_str); //списко клиентов с указанным email
		break;

	case 4:  //по номеру телефона
		user_info_str = get_string("Введите телефон клиента: "); //получить телефон от пользователя
		int usr_id = db_unit.get_user_id_by_phone(user_info_str); //клиент с указанным телефоном
		return usr_id;

	}
	
	//не найдено ни одной записи
	if (users_map.size() == 0)
	{
		return -1;
	}
	
	//найдена 1 запись
	if (users_map.size() == 1)
	{
		auto it = users_map.begin(); 
		return it->first;
	}

	//найдено несколько записей:
	std::cout << "\nВ базе найдено " << users_map.size() << " подходящих вариантов: \n";
	
	auto it = users_map.begin();
	for (int i = 0;  it != users_map.end();  ++i, ++it)
	{
		int temp_id = it->first; 
		std::cout << i+1 << ": id = " << temp_id << ", "; 
		auto temp_tuple = users_map[temp_id];
		
		std::string temp_str = std::get<0>(temp_tuple); //имя
		std::cout << "имя:  " << temp_str << ", ";
		
		temp_str = std::get<1>(temp_tuple); //фамилия
		std::cout << "фамилия:  " << temp_str << ", ";

		temp_str = std::get<1>(temp_tuple); //email
		std::cout << "email:  " << temp_str << "\n";
	}

	std::cout << "Введите номер нужного клиента или 0, если не подходит ни один из перечисленных: \n";

	int user_chr = -1;
	while (user_chr < 0)
	{
		std::cin >> user_chr;
		if ((std::cin.fail()) || //ошибка ввода	
			(user_chr < 0) ||
			(user_chr > users_map.size()))
		{
			user_chr = -1;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Введите номер нужного клиента или 0, если не подходит ни один из перечисленных: \n ";
		}
	}
	
	//пользователь указал, что не подходит ни один из перечисленных вариантов
	if (user_chr == 0)
	{
		return -1;
	}

	//найти запись в мапе по нужному итератору
	it = std::next(users_map.begin(), user_chr - 1);
	try
	{
		//возможен выход it за пределы мапа
		return it->first;
	}
	catch (...)
	{
		return -1;
	}	
	
}

std::string get_string(std::string print_str) //получить строку от пользователя
{
	std::string user_str;

	std::cin.get();
	while (user_str.length() < 1)
	{
		std::cout << print_str;
		std::getline(std::cin, user_str);
	}

	return user_str;
}

void add_user_phone(Data_base& db_unit) //добавить телефон существующему клиенту
{
	//сначала найти клиента
	int user_id = get_user_id(db_unit);

	//-1 - записи в базе не найдены
	if (user_id < 0)
	{
		std::cout << "В базе нет такого клиента \n";
		return;
	}

	//получить информацию о клиенте по его id
	auto user_tuple = db_unit.get_user_by_id(user_id);
	std::string u_name = std::get<1>(user_tuple);
	std::string u_lastname = std::get<2>(user_tuple);
	std::string u_email = std::get<3>(user_tuple);

	std::cout << "\nНайден клиент: id = " << user_id << ", имя =  " << u_name << ", фамилия = " << u_lastname << ", email = " << u_email << "\n";

	//получить информацию о имеющихся у клиента номерах телефона
	std::vector<std::string> phones_vector = db_unit.get_phones_list_by_user_id(user_id);

	if (phones_vector.empty())
	{
		std::cout << "Номера телефонов: не найдены\n";
	}
	else
	{
		for (const auto& elem : phones_vector)
			std::cout << elem << "\n";
	}
	
	std::string user_str;
	do
	{		
		std::cout << "Добавить номер телефона этому клиенту? \n 1 - добавить номер, \n 2 - выход\n";
		std::cin >> user_str;
	} while ((user_str != "1") && (user_str != "2"));
	
	if (user_str == "2")
	{
		return;
	}

	if (user_str == "1")
	{
		//получить строку от пользователя
		user_str = get_string("Введите номер телефона:\n"); //to-do по правильному нужно добавить проверку введенного на формат, допустимую длину, символы и т.п. 
	}
	
	if (db_unit.add_user_phone(user_id, user_str))
	{
		std::cout << "Телефон добавлен\n";
	}
	else
	{
		std::cout << "Не удалось добавить телефон.\n";
		db_unit.print_last_error();
	}
}

void update_user_data(Data_base& db_unit) //изменить данные о клиенте
{
	//сначала найти клиента
	int user_id = get_user_id(db_unit);

	//-1 - записи в базе не найдены
	if (user_id < 0)
	{
		std::cout << "В базе нет такого клиента \n";
		return;
	}

	//получить информацию о клиенте по его id
	auto user_tuple = db_unit.get_user_by_id(user_id);
	std::string u_name = std::get<1>(user_tuple);
	std::string u_lastname = std::get<2>(user_tuple);
	std::string u_email = std::get<3>(user_tuple);

	std::string u_name_new = u_name;
	std::string u_lastname_new = u_lastname;
	std::string u_email_new = u_email;

	std::string user_ch = "-1"; 
	while (user_ch != "0")
	{
		//какие данные меняем
		std::cout << "Клиент c id = " << user_id << ".  Какую информацию о клиенте нужно изменить?\n";
		
		std::cout << "1. Имя:  текущее значение = " << u_name;
		if (u_name == u_name_new) 	{std::cout << "\n";} 
		else { std::cout << "; новое значение = " << u_name_new << "\n"; }

		std::cout << "2. Фамилия: текущее значение = " << u_lastname; 
		if (u_lastname == u_lastname_new) { std::cout << "\n"; }
		else { std::cout << "; новое значение = " << u_lastname_new << "\n"; }

		std::cout << "3. email: текущее значение = " << u_email; 
		if (u_email == u_email_new) { std::cout << "\n"; }
		else { std::cout << "; новое значение = " << u_email_new << "\n"; }

		std::cout << "0. Все данные верны. Завершить обновление информации.\n";

		do
		{
			std::cin >> user_ch;
		} while ((user_ch != "0") && (user_ch != "1") && (user_ch != "2") && (user_ch != "3"));
		
		if (user_ch == "0") { break; }
		
		//получить новое значение		
		std::string new_value= get_string("Введите новое значение: ");  

		if (user_ch == "1") u_name_new = new_value; //новое имя
		if (user_ch == "2") u_lastname_new = new_value; //новое имя
		if (user_ch == "3") u_email_new = new_value; //новый email

		user_ch = "-1";		
	}
	
	//если значения не менялись, выход
	if ((u_name == u_name_new) && (u_lastname == u_lastname_new) && (u_email == u_email_new))
	{
		return;
	}
	else // внести новые значения в базу
	{
		if (db_unit.update_user_data(user_id, u_name_new, u_lastname_new, u_email_new))
		{
			std::cout << "Данные клиента обновлены\n";
		}
		else
		{
			std::cout << "Не удалось обновить данные клиента.\n";
			db_unit.print_last_error();
		}
	}
}

void delete_user_phone(Data_base& db_unit) //удалить телефон клиента 
{
	//сначала найти клиента
	int user_id = get_user_id(db_unit);

	//-1 - записи в базе не найдены
	if (user_id < 0)
	{
		std::cout << "В базе нет такого клиента \n";
		return;
	}

	//получить информацию о клиенте по его id
	auto user_tuple = db_unit.get_user_by_id(user_id);
	std::string u_name = std::get<1>(user_tuple);
	std::string u_lastname = std::get<2>(user_tuple);
	std::string u_email = std::get<3>(user_tuple);

	std::cout << "\nНайден клиент: id = " << user_id << ", имя =  " << u_name << ", фамилия = " << u_lastname << ", email = " << u_email << "\n";

	//получить информацию о имеющихся у клиента номерах телефона
	std::vector<std::string> phones_vector = db_unit.get_phones_list_by_user_id(user_id);

	if (phones_vector.empty())
	{
		std::cout << "Номера телефонов: не найдены\n";
	}
	else
	{
		for (int i = 0; i < phones_vector.size(); i++)
		{
			std::cout << i + 1 << ". " << phones_vector[i] << "\n";
		}

		std::cout << "0. Удалять номера телефона не нужно. Выход.\n";

		int user_ch = -1;
		do
		{
			std::cout << "Введите порядковый номер телефона, который нужно удалить или 0 для выхода.";
			std::cin >> user_ch;
			if (std::cin.fail()) 
			{
				user_ch = -1;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');				
			}
		} while (user_ch < 0);

		if (user_ch == 0) return; //пользователь решил ничего не удалять
		if ((user_ch < 0) ||
			(user_ch >= phones_vector.size()))
		{
			std::cout << "Невозможно удалить несуществующий номер. \n";
			return;
		}


		std::string  phone_to_delete = phones_vector[user_ch-1];

		std::cout << "Вы уверены, что хотите удалить номер телефона " << phone_to_delete << "?\n";
		std::cout << "Введите 1, чтобы подтвердить удаление: ";

		std::string user_str;
		std::cin >> user_str;

		if (user_str != "1") return;

		if (db_unit.delete_user_phone(user_id, phone_to_delete))
		{
			std::cout << "Номер телефона " << phone_to_delete << " удален.\n";
		}
		else
		{
			std::cout << "Не удалось удалить номер телефона " << phone_to_delete << ".\n";
			db_unit.print_last_error();
		}
	}
}

void delete_user(Data_base& db_unit) //удалить информацию о клиенте
{
	//сначала найти клиента
	int user_id = get_user_id(db_unit);

	//-1 - записи в базе не найдены
	if (user_id < 0)
	{
		std::cout << "В базе нет такого клиента \n";
		return;
	}

	//получить информацию о клиенте по его id
	auto user_tuple = db_unit.get_user_by_id(user_id);
	std::string u_name = std::get<1>(user_tuple);
	std::string u_lastname = std::get<2>(user_tuple);
	std::string u_email = std::get<3>(user_tuple);

	std::cout << "Вы уверены, что хотите удалить всю информацию о клиенте?\n";
	std::cout << "\nКлиент: id = " << user_id << ", имя =  " << u_name << ", фамилия = " << u_lastname << ", email = " << u_email << "\n";
	std::cout << "Введите 1, чтобы подтвердить удаление: ";

	std::string user_str;
	std::cin >> user_str;

	if (user_str != "1") return;

	if (db_unit.delete_user(user_id))
	{
		std::cout << "Данные о клиенте удалены.\n";
	}
	else
	{
		std::cout << "Не удалось удалить данные о клиенте.\n";
		db_unit.print_last_error();
	}





	

}

#pragma once
#include <iostream>

#include "data_base.h"

int main_menu(); //главное меню
void add_new_user(Data_base& db_unit); //добавить нового клиента
void print_user_info(Data_base& db_unit); //вывод информации о клиенте
void add_user_phone(Data_base& db_unit); //добавить телефон существующему клиенту
void update_user_data(Data_base& db_unit); //изменить данные о клиенте
void delete_user_phone(Data_base& db_unit); //удалить телефон клиента 
void delete_user(Data_base& db_unit); //удалить информацию о клиенте

int get_user_id(Data_base& db_unit); //поиск id клиента

int get_search_param(); //запросить у пользователя, по какому параметру ищем клиента

std::string get_string(std::string print_str); //получить строку от пользователя

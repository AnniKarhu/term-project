#pragma once
#include <iostream>

class ParserException_error : public std::exception
{
public:
	const char* what() const override
	{
		return "������ �������";
	}
};

class ParserException_no_file : public std::exception
{
public:
	const char* what() const override
	{
		return "�� ���������� ������� ������ �� �����";
	}
};

class ParserException_incorrect_data : public std::exception
{
public:
	const char* what() const override
	{
		return "� ����� ���������� ������������ ������. ���������� ������� ����� ���� ������������� ��� ���������.";
	}
};

class ParserException_no_section : public std::exception
{
public:
	const char* what() const override
	{
		return "������������� ������ �� ����������";
	}
};

class ParserException_no_variable : public std::exception
{
public:
	const char* what() const override
	{
		return "� ������ ��� ������������� ����������";
	}
};

class ParserException_incorrect_request : public std::exception
{
public:
	const char* what() const override
	{
		return "������ � ������ �������";
	}
};

class ParserException_type_error : public std::exception
{
public:
	const char* what() const override
	{
		return "�������� ��� ���������� � �������";
	}
};

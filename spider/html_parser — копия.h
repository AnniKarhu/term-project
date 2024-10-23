#pragma once

#include <iostream>
#include <regex>
//#include <boost/regex.hpp>
#include <set>

class html_parser
{
private:
	std::string complete_url(const std::string& in_url, const std::string& url_base);
	std::string get_base_path(const std::string& in_str);

public:
	std::set<std::string> get_urls_from_html(const std::string& html_body_str, const std::string& base_str);
	std::string get_base_host(const std::string& url_str);

	std::string get_tag_string(const std::string& html_body_str); //поиск любого первого тега в строке
};
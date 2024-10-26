#pragma once

#include <iostream>
#include <regex>
#include <set>
#include <map>

//удалить после отладки
//const std::string test_html_str = "<!doctype html>< html ><head><title>Example Domain< / title><meta charset = 'utf-8' / >"
//"<meta http - equiv = Content-type content = text/html; charset=utf-8 / >"
//"<meta name = viewport content = width=device-width, initial-scale=1 / >"
//"<style type = text/css>body{    background - color: #f0f0f2;    margin : 0;    padding : 0;"
//"   font - family: -apple - system, system - ui, BlinkMacSystemFont, Helvetica, Arial, sans - serif;}"
//"div{    width: 600px;    margin: 5em auto;    padding: 2em;    background - color: #fdfdff;    border - radius: 0.5em;"
//"    box - shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);}a:link, a : visited{    color: #38488f;    text - decoration: none;}"
//"@media(max - width: 700px) {    div{        margin: 0 auto;        width: auto;    }}"
//"< / style>< / head><body><div><h1>Example Domain< / h1>"
//"<p>This domain is for use in illustrative examples in documents.You may use this"
//"domain in literature without prior coordination or asking for permission.< / p>"
//"<p><a href = https://www.iana.org/domains/example>More information...< / a>. ,:;-_~#$%^&*+=!?text< / p>"
//"< / div>< / body>< / html>";

class html_parser
{
private:	
	std::string complete_url(const std::string& in_url, const std::string& url_base);
	std::string get_base_path(const std::string& in_str);
	bool check_this_host_only(const std::string& host_url, const std::string& url_str, bool this_host_only); //проверка, является ли урл требуемым хостом	

public:
	int max_word_len = 32; //максимальная длина слова для добавления в базу данных
	int min_word_len = 3;  //минимальная длина слова для добавления в базу данных

	std::string get_base_host(const std::string& url_str); //получить хост из строки	
	std::string clear_tags(const std::string& html_body_str);	//очистить строку от тегов, в том числе все до тега <body>
	
	std::set<std::string> get_urls_from_html(const std::string& html_body_str, const std::string& base_str, bool this_host_only, std::string host_url); //получить список новых урлов
	std::map<std::string, unsigned  int> collect_words(const std::string& text_str); //получить мап слов 
};
#pragma once

#include <iostream>
#include <regex>
#include <set>
#include <map>

const std::string test_html_str = "<!doctype html>< html ><head><title>Example Domain< / title><meta charset = 'utf-8' / >"
"<meta http - equiv = Content-type content = text/html; charset=utf-8 / >"
"<meta name = viewport content = width=device-width, initial-scale=1 / >"
"<style type = text/css>body{    background - color: #f0f0f2;    margin : 0;    padding : 0;"
"   font - family: -apple - system, system - ui, BlinkMacSystemFont, Helvetica, Arial, sans - serif;}"
"div{    width: 600px;    margin: 5em auto;    padding: 2em;    background - color: #fdfdff;    border - radius: 0.5em;"
"    box - shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);}a:link, a : visited{    color: #38488f;    text - decoration: none;}"
"@media(max - width: 700px) {    div{        margin: 0 auto;        width: auto;    }}"
"< / style>< / head><body><div><h1>Example Domain< / h1>"
"<p>This domain is for use in illustrative examples in documents.You may use this"
"domain in literature without prior coordination or asking for permission.< / p>"
"<p><a href = https://www.iana.org/domains/example>More information...< / a>. ,:;-_~#$%^&*+=!?text< / p>"
"< / div>< / body>< / html>";

class html_parser
{
private:	
	std::string complete_url(const std::string& in_url, const std::string& url_base);
	std::string get_base_path(const std::string& in_str);

public:
	int max_word_len = 32;
	int min_word_len = 3;
	
	std::string get_base_host(const std::string& url_str);
	std::set<std::string> get_urls_from_html(const std::string& html_body_str, const std::string& base_str);
	std::string clear_tags(const std::string& html_body_str);	
	
	//void collect_words(const std::string& text_str, std::map<std::string, unsigned  int>& words_map);
	std::map<std::string, unsigned  int> collect_words(const std::string& text_str);
	
};
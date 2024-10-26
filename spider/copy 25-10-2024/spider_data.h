#pragma once
#include <iostream>

struct Spider_data
{
	// information on urls
	std::string	start_url = "https://www.google.com/";
	
	//database
	std::string db_connection_string;

	//indexing settings
	int search_depth = 1;
	int min_word_length = 3;
	int	max_word_length = 32;

	bool this_host_only = false;

	//process settings
	int threads_num = 0; //0 - no limits, max possible threads for indexing
	int empty_thread_sleep_time = 100;

};
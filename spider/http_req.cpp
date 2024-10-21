
#include "http_req.h"

http_req::http_req(std::string _full_url) : full_url(_full_url) {}

bool http_req::check_url()
{
	std::cout << " full url = " << full_url << "\n";	

	size_t pos = full_url.find(url_start());

	if (pos != 0) return false;
	
	full_url.erase(pos, url_start().length());	

	//удалить лишние слеши из начала хоста (например, такие запросы http::////example.com) 
	while (!(pos = full_url.find("/")))
	{
		full_url.erase(0, 1);
	}

	pos = full_url.find("/");
	if (pos == std::string::npos)
	{
		host = full_url;
		target = "/";
	}
	else
	{
		target = full_url.substr(pos);
		host = full_url.erase(pos, target.length());
	}

	std::cout << "host = " << host << std::endl;;
	std::cout << "target = " << target << std::endl;

	return true;
}

bool http_req::get_page()
{	
	try
	{
		net::io_context ioc; // The io_context is required for all I/O

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		auto const results = resolver.resolve(host, port());// Look up the domain name

		stream.connect(results);// Make the connection on the IP address we get from a lookup

		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::get, target, version };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		// Send the HTTP request to the remote host
		http::write(stream, req);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::dynamic_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		fill_response_fields(res);

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;		
		return false;
	}
	
	return true;	
}

void http_req::fill_response_fields(http::response<http::dynamic_body>& res)
{
	request_res = static_cast<request_result>(res.result_int()); //response code from url

	//if (request_res == request_result::req_res_ok)
	//{
		switch (request_res)
		{
		case request_result::req_res_ok: {
				html_body_str = boost::beast::buffers_to_string(res.body().data());
				break;
			}

		case request_result::req_res_redirect: {
				auto location_it = res.find("Location");
				redirected_location = (*location_it).value();
				break;
			}
		
		}
	//}
}
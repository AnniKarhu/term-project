#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

enum class request_result
{
	req_res_unknown = 0,
	req_res_ok = 200,
	req_res_redirect = 301,
	req_res_forbidden = 403,
	req_res_page_not_found = 404,	
	req_res_other = -1
};

class http_req
{
protected:
	virtual const std::string port() { return "80"; }	
	virtual const std::string url_start() 	{ 	return "http://"; 	}
	
	std::string host;
	std::string target;
	const int version = 11;

	request_result request_res = request_result::req_res_unknown;
	std::string redirected_location;
	std::string html_body_str;
	
	void fill_response_fields(http::response<http::dynamic_body>& res);

private:
	std::string full_url;	
	
	 
public:

	http_req(std::string _full_url);
	request_result get_request_result() { 	return request_res; }

	virtual bool get_page();
	bool check_url();
	
};
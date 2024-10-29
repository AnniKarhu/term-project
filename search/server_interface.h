#pragma once

//#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio/dispatch.hpp>
//#include <boost/asio/strand.hpp>
//#include <boost/config.hpp>
//#include <algorithm>
//#include <cstdlib>
//#include <functional>
#include <iostream>
//#include <memory>
//#include <string>
//#include <thread>
//#include <vector>
#include <fstream>
#include <regex>
#include <set>

#include "data_base.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

std::string open_start_file_search_result(const std::string& file_path); //получить содержимое файла html для вывода результата клиенту
bool split_str_content(const std::string& source_str, std::string& start_str, std::string& end_str); //разделить строку на 2 части по делимитеру "<!--search result below-->"
std::string clear_request_string(const std::string& source_str); //очистить строку поиска от служебного содержимого
std::set<std::string> get_words_request_set(const std::string& source_str); //создать set из слов запроса

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
        {
            auto const pos = path.rfind(".");
            if (pos == beast::string_view::npos)
                return beast::string_view{};
            return path.substr(pos);
        }();
    if (iequals(ext, ".htm"))  return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php"))  return "text/html";
    if (iequals(ext, ".css"))  return "text/css";
    if (iequals(ext, ".txt"))  return "text/plain";
    if (iequals(ext, ".js"))   return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml"))  return "application/xml";
    if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if (iequals(ext, ".flv"))  return "video/x-flv";
    if (iequals(ext, ".png"))  return "image/png";
    if (iequals(ext, ".jpe"))  return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg"))  return "image/jpeg";
    if (iequals(ext, ".gif"))  return "image/gif";
    if (iequals(ext, ".bmp"))  return "image/bmp";
    if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif"))  return "image/tiff";
    if (iequals(ext, ".svg"))  return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path)
{
        if (base.empty())
            return std::string(path);

        std::string result(base);
    #ifdef BOOST_MSVC
        char constexpr path_separator = '\\';
        if (result.back() == path_separator)
            result.resize(result.size() - 1);

        result.append(path.data(), path.size());

        for (auto& c : result)
            if (c == '/')
                c = path_separator;
    #else
        char constexpr path_separator = '/';
        if (result.back() == path_separator)
            result.resize(result.size() - 1);

        result.append(path.data(), path.size());
    #endif
        return result;
}

// Return a response for the given request.
// The concrete type of the response message (which depends on the request), is type-erased in message_generator.
template <class Body, class Allocator>
http::message_generator handle_request(
                                        beast::string_view doc_root,
                                        http::request<Body, http::basic_fields<Allocator>>&& req,
                                        int search_results, 
                                        Data_base* data_base)
{
    // Returns a bad request response
    auto const bad_request =
        [&req](beast::string_view why)
        {
            http::response<http::string_body> res{ http::status::bad_request, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = std::string(why);
            res.prepare_payload();
            return res;
        };

    // Returns a not found response
    auto const not_found =
        [&req](beast::string_view target)
        {
            http::response<http::string_body> res{ http::status::not_found, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "The resource '" + std::string(target) + "' was not found.";
            res.prepare_payload();
            return res;
        };

    // Returns a server error response
    auto const server_error =
        [&req](beast::string_view what)
        {
            http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + std::string(what) + "'";
            res.prepare_payload();
            return res;
        };

    // Make sure we can handle the method
    if (req.method() != http::verb::get &&
        req.method() != http::verb::head &&
        req.method() != http::verb::post)
        return bad_request("Unknown HTTP-method");

    // Request path must be absolute and not contain "..".
    if (req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        return bad_request("Illegal request-target");

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");    

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;

    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return not_found(req.target());

    // Handle an unknown error
    if (ec)
        return server_error(ec.message());

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == http::verb::head)
    {
        http::response<http::empty_body> res{ http::status::ok, req.version() };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // Respond to GET request
    else if (req.method() == http::verb::get)
    {
        http::response<http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version()) };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());

        return res;
    }

    //POST request

    std::cout << "req.body() = " << req.body() << "\n\n";
    std::string request_string  = clear_request_string (req.body());

   // request_string = ;
    std::cout << "request_string = " << request_string << "\n\n";

    get_words_request_set(request_string);
   // почему в request_string плюсики вместо пробелов

    http::response<http::string_body> res{ http::status::ok, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    //res.set(http::field::content_type, "text/html");
    res.set(http::field::content_type, mime_type(path));
    res.keep_alive(req.keep_alive());


    std::string body_str = open_start_file_search_result(path);

    //std::cout << "body_str = " << body_str;
    
    std::string start_str;
    std::string end_str;
    if (split_str_content(body_str, start_str, end_str))
    {
        //std::cout << "start_str = \n" << start_str << "\n\n\n";
        //std::cout << "end_str = \n" << end_str << "\n";
        body_str = start_str + "<p>Your search: " + request_string + "</p>\n" + end_str;
    }    

    res.body() = body_str;

    auto const size_body = body_str.size();
    res.content_length(size_body);
    res.prepare_payload();

    std::cout << "get req search_results = " << search_results << "\n";

    //std::string  html_body_str = boost::beast::buffers_to_string(res.body().data());        

    return res;
}

// Report a failure
void
fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

std::string open_start_file_search_result(const std::string& file_path)
{
    std::cout << "path = " << file_path << "\n";
    std::ifstream res_file(file_path);
    if (!res_file.is_open())
    {
       return "file " + file_path + " not found";
    }
    
    std::string file_content;
    std::string str;

    while (!res_file.eof())
    {
        std::getline(res_file, str);
        file_content += str;
        file_content += "\n";
    }    

   // std::cout << file_content << std::endl;
    res_file.close();
    return file_content;
}

bool split_str_content(const std::string& source_str, std::string& start_str, std::string& end_str) //разделить результирующий  файл на 2 части - в середину буду вставлять результаты поиска
{        
    std::smatch res;
    if (regex_search(source_str, res, std::regex("<!--search result below-->")))
    {
        start_str = res.prefix();
        end_str = res.suffix();
        return true;
     }
    else return false;
}

std::string clear_request_string(const std::string& source_str) //очистить строку поиска от служебного содержимого
{
    std::string field_name = "search_request=";
    
    if (!source_str.find(field_name) == 0)
        return "";

    std::string res_string = source_str;

    res_string.erase(0, field_name.size());

    res_string = std::regex_replace(res_string, std::regex("%09"), " "); //убрать знаки табуляции
    res_string = std::regex_replace(res_string, std::regex("([\.,:;!?\\\"'*+=_~#$^&])"), " "); //убрать знаки препинания и спец символы
    res_string = std::regex_replace(res_string, std::regex(" {2,}"), " "); //убрать двойные пробелы
    
    //все строчные
    std::transform(res_string.begin(), res_string.end(), res_string.begin(),
        [](unsigned char c) { return std::tolower(c); });

    return res_string;
}

std::set<std::string> get_words_request_set(const std::string& source_str)
{
    std::string str = source_str; // "1111 2222 3333 4444"; //source_str;
    std::set<std::string> result_set;

    /*while (!str.empty())
    {

    }*/


    std::istringstream input{ str };
    std::vector<std::string> result_vector;

    // extract substrings one-by-one
    while (!input.eof()) {
        std::string substring;
        input >> substring;
        result_vector.push_back(substring);
    }

    std::cout << "___________get_words_request_set_________" << "\n";
    // print all the extracted substrings
    for (const std::string& substring : result_vector) {
        std::cout << substring << std::endl;
    }

    return result_set;
}
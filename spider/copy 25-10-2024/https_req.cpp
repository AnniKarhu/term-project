
#include "root_certificates.hpp"  //add for ssl, https connection
#include "https_req.h"

bool https_req::get_page()
{
    try
    { 
        net::io_context ioc; // The io_context is required for all I/O

        ssl::context ctx(ssl::context::tlsv12_client); // The SSL context is required, and holds certificates           
        load_root_certificates(ctx); // This holds the root certificate used for verification 
        ctx.set_verify_mode(ssl::verify_peer); // Verify the remote server's certificate
        
        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        ssl::stream<beast::tcp_stream> stream(ioc, ctx); 

        // Set SNI Hostname (many hosts need this to handshake successfully)        
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
        {
            beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
            throw beast::system_error{ ec };
        }

        auto const results = resolver.resolve(host, port()); // Look up the domain name        
        
        
        //эта функция вешает поток при недоступном хосте (например instagram)
        //попробовать заменить на асинхронный доступ и добавить тайм аут, если останется время
        beast::get_lowest_layer(stream).connect(results);  // Make the connection on the IP address we get from a lookup        
        stream.handshake(ssl::stream_base::client); // Perform the SSL handshake

        // Set up an HTTP GET request message
        http::request<http::string_body> req{ http::verb::get, target, version };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::accept, "text/html");

        // Send the HTTP request to the remote host
        http::write(stream, req);
       
        beast::flat_buffer buffer; // This buffer is used for reading and must be persisted
        http::response<http::dynamic_body> res;// Declare a container to hold the response        
        http::read(stream, buffer, res);// Receive the HTTP response        
        
        fill_response_fields(res);
             

        // Gracefully close the stream
        beast::error_code ec;
        stream.shutdown(ec);
            

        //if (ec != net::ssl::error::stream_truncated) 
        //    throw beast::system_error{ ec };
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    
    return true;

}

#include <iostream>
#include <regex>

std::string complete_url(const std::string& in_url, const std::string& url_base);

std::string get_base_path(const std::string& in_str);

int main()
{
    std::cout << "Hello World!\n";    


    std::string s1 = //"<!doctype html>         < html >        <head>        <title>Example Domain < / title>"

       // "<meta charset = 'utf - 8' / >        <meta http - equiv = 'Content - type' content = 'text / html; charset = utf - 8' / >"

       // "<meta name = 'viewport' content = 'width=device-width, initial-scale=1' / >"
       // " <style type = 'text/css'>        body{            background - color: #f0f0f2;            margin : 0;"
       // " padding : 0;   font - family: -apple - system, system - ui, BlinkMacSystemFont,"
       // "'Segoe UI', 'Open Sans', 'Helvetica Neue', Helvetica, Arial, sans - serif;"
       // "    }         div{            width: 600px;            margin: 5em auto;            padding: 2em;"
       // "     background - color: #fdfdff;            border - radius: 0.5em;            box - shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);"
       // "}    a:link, a : visited{        color: #38488f;        text - decoration: none;    }"
       // "   @media(max - width: 700px) {        div{            margin: 0 auto;            width: auto;        }    }    < / style>        < / head>"

       // "   <body>        <div>        <h1>Example Domain< / h1>        <p>This domain is for use in illustrative examples in documents.You may use this"
       // " domain in literature without prior coordination or asking for permission.< / p>"

        //"<p><       a  href='https://www.iana.org/domains/example' >More information...< / a>< / p>\n"
        //"<p><       a  href='https://1test.su' >More information...< / a>< / p>\n"
        //"<p><   a target = balnc  href='https://www.2test/d.htm' >More information...< / a>< / p>\n"
        //"<p><   a target = '_balnc'  href = 'https://www.3test/d.htm' >More information...< / a>< / p>\n"
        //"<p><   a target = '_balnc' class='simples '  href = 'https://www.4test/d.htm' >More information...< / a>< / p>\n"
        //"<p><   a target  href='/6test/d.htm' >More information...< / a>< / p>\n"
        "<p><   a target  href='/7test/' >More information...< / a>< / p>\n";
        //"<p><   a target>  href='https://www.5test/d.htm' >More information...< / a>< / p>\n"
        //"<p><   a target  href='http/8test/' >More information...< / a>< / p>\n";
       // "< / div>        < / body>                      < / html>";

    std::cout << "start string = " << s1 << "\n\n";

    std::string s2 = std::regex_replace(s1, std::regex("\t| {2,}"), " ");
    s2 = std::regex_replace(s2, std::regex("< /"), "</");
    s2 = std::regex_replace(s2, std::regex("</ "), "</");
    s2 = std::regex_replace(s2, std::regex("< a"), "<a");
    s2 = std::regex_replace(s2, std::regex("'"), "\"");
    s2 = std::regex_replace(s2, std::regex(" ?= ?"), "=");
    std::cout << "s2 string = " << s2 << "\n\n";

    std::smatch res1;
  
    std::regex r1("<a (.?[^>]*?)href=\"(.*?)\"");
    
    std::string url_base = "http://www.1test.tu";
    
    while (regex_search(s2, res1, r1))
    {
        std::cout <<  "\n\n";
        
        std::string find_str = res1.str();
       // std::cout << "New iter find_str: " << find_str << std::endl; // << " + " << res.suffix() << std::endl;
        std::string url_str = std::regex_replace(find_str, std::regex("<a (.?[^>]*?)href=\""), "");
        url_str = std::regex_replace(url_str, std::regex("\""), "");
        
        std::cout << "url_str = " << url_str << "\n";

        /*std::string new_base_path = get_base_path(url_str);        
        std::cout << "new_base_path = " << new_base_path << "\n";

        std::string final_url1 = complete_url(url_str, new_base_path);        
        std::cout << "final_url1 = " << final_url1 << "\n";

        std::string final_url2 = complete_url(url_str, url_base);
        std::cout << "final_url2 = " << final_url2 << "\n";       

        std::cout << "New iter: " << final_url2 << std::endl;
        s2 = res1.suffix();*/

        std::string new_base_path = get_base_path(url_str);        
        std::cout << "new_base_path = " << new_base_path << "\n";

        std::string final_url1 = complete_url(url_str, new_base_path);        
        std::cout << "final_url1 = " << final_url1 << "\n";

        std::string final_url2 = complete_url(url_str, url_base);
        std::cout << "final_url2 = " << final_url2 << "\n";       

        std::cout << "New iter: " << final_url2 << std::endl;
        s2 = res1.suffix();
    };
     
    

return 0;
}

std::string complete_url(const std::string& in_url, const std::string& url_base)
{
    std::cout << "complete_url****************\n";
    продолжить здесь - удалить двойной слеш, затем проверить остальные тесты

    std::cout << "complete_url in_url = " << in_url << "\n";
    std::string res_url = in_url;
    std::regex_replace(res_url, std::regex("/$"), "");

    auto pos = res_url.find("https://");
    if (pos == 0) return res_url;

    pos = res_url.find("http://");
    if (pos == 0) return res_url;

    pos = res_url.find("www.");
    if (pos == 0) return res_url;

    res_url = url_base + "/" + res_url;
    //std::regex_replace(res_url, std::regex("/{2,}"), "/");

    return res_url;
}

std::string get_base_path(const std::string& in_str)
{
   // std::cout << "************get_base_path*************\n";
    
   
    std::string res_str = in_str;
    res_str = std::regex_replace(res_str, std::regex("/$"), "");

    std::string http_prefix = "https://";
    if (res_str.find(http_prefix) == std::string::npos)
    {
        http_prefix = "http://";
        if (res_str.find(http_prefix) == std::string::npos)
        {
            http_prefix = "";
        }
    }

    res_str.erase(0, http_prefix.size());

    //std::string input_str = "http/8test/test.te.st";
   // std::cout << "start input_str = " << res_str << "\n";

    std::smatch res;
    //std::regex r("/(.[^/]*)$");
    std::regex r("/(.[^/]*)$");
    if (regex_search(res_str, res, r))
    {        
        std::string find_str = res.str();  
        auto n_pos = res_str.find(find_str);
        if (find_str.find(".") != std::string::npos)
        {
            res_str.erase(n_pos, find_str.size());
        }
       // std::cout << "find_str = " << find_str << "\n\n";
       // std::cout << "input_str = " << res_str << "\n\n";
    };
    
    res_str = std::regex_replace(res_str, std::regex("/{2,}"), "/");
   //res_str = std::regex_replace(res_str, std::regex("/$"), "");

    return http_prefix + res_str;
}
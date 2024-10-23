#include "html_parser.h"

std::string html_parser::complete_url(const std::string& in_url, const std::string& url_base)
{
    std::string res_url = in_url;
    std::regex_replace(res_url, std::regex("/$"), "");

    auto pos = res_url.find("https://");
    if (pos == 0) return res_url;

    pos = res_url.find("http://");
    if (pos == 0) return res_url;

    pos = res_url.find("www.");
    if (pos == 0) return res_url;

    pos = res_url.find("/");
    if (pos == 0)
    {
        res_url.erase(0, 1);
    }
   
    res_url = url_base + "/" + res_url;
    
    return res_url;
}
std::string html_parser::get_base_path(const std::string& in_str)
{
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

    std::smatch res;
    std::regex r("/(.[^/]*)$");
    if (regex_search(res_str, res, r))
    {
        std::string find_str = res.str();
        auto n_pos = res_str.find(find_str);
        if (find_str.find(".") != std::string::npos)
        {
            res_str.erase(n_pos, find_str.size());
        }        
    };

    res_str = std::regex_replace(res_str, std::regex("/{2,}"), "/");
   
    return http_prefix + res_str;
}


std::set<std::string> html_parser::get_urls_from_html(const std::string& html_body_str, const std::string& base_str)
{
    std::set<std::string> urls_set;
    //std::cout << "1" << "\n";
    
    /*���������� ����� - �������� ������ �������:
    1. ����� ��� <title></title> - ������� ����� �� ��������, ����� ����������� �����������
    2. ����� ��� <body >. ����� - ������� ������� <body, ��� � < body> - ������� ��� ������ �� ����� ����
    3. ������� ������ ������ ���� <> - ������� ����������� � ������ ������ - �� ������ std::string find - ����� < ����� ����� > ����� ����� ����� <
    4. ������� ����������� ���� ���� �� �����. 
    
        ����:
            5. ����� ���������� ���� - ���� a - ��������� �� ������� ��� ��������� ������ � ����������� �����������
            6. ������� ���
        ����� �����
        
    7. ������� ��� ������������� �������, ���������, ����� ����������, �������� ������
        8. ��������� ��� � ������ �������
        9. �������� ������ � map � ��������� ���������� ����*/

    //�������� html � ����� ��������� ����    
   
    //std::cout << "2\n";
    std::string s2 = std::regex_replace(html_body_str, std::regex("\n"), " ");
    s2 = std::regex_replace(s2, std::regex("\t"), " ");
    s2 = std::regex_replace(s2, std::regex(" {2,}"), " ");

    s2 = std::regex_replace(s2, std::regex("< /"), "</");
   // std::cout << "3 \n";
    s2 = std::regex_replace(s2, std::regex("</ "), "</");
   // std::cout << "4 \n";
    s2 = std::regex_replace(s2, std::regex("< a"), "<a");
    //std::cout << "5 \n";
    s2 = std::regex_replace(s2, std::regex("'"), "\"");
    //std::cout << "6 \n";
    s2 = std::regex_replace(s2, std::regex(" ?= ?"), "=");
    //std::cout << "7 \n";
   // std::cout << "s2 string = " << s2 << "\n\n";

    //����� ������ <a href>
    std::smatch res1;
    std::regex r1("<a (.?[^>]*?)href=\"(.*?)\"");
    //std::cout << "8 \n";
    while (regex_search(s2, res1, r1))
    {
       // std::cout << "9 \n";
        //std::cout << "\n\n";
        std::string find_str = res1.str();
       // std::cout << "find_str: " << find_str << std::endl; 
        //std::cout << "10 \n";
        std::string url_str = std::regex_replace(find_str, std::regex("<a (.?[^>]*?)href=\""), ""); //������� ��� �������� ����� ������ a � ��������� href
       // std::cout << "11 \n";
        url_str = std::regex_replace(url_str, std::regex("\""), ""); //������� ��� �������
       // std::cout << "12 \n";
        url_str = std::regex_replace(url_str, std::regex("/$"), ""); //������� ���� � ����� ������
        //std::cout << "13 \n";
        // std::cout << "url_str = " << url_str << "\n";

        std::string new_base_path = get_base_path(url_str); //����� ������� ����� ����
        //std::cout << "new_base_path = " << new_base_path << "\n";

        std::string suf_str = url_str.erase(0, new_base_path.size()); //����� ��� �������� ��� �������
       // std::cout << "suf_str = " << suf_str << "\n";

        std::string final_url = complete_url(new_base_path, base_str) + suf_str; //��������� ��� ����
       // std::cout << "final_url = " << final_url << "\n";
       // std::cout << "14 \n";
       // std::string out_str = "New iter: " + final_url + "\n"; // std::endl;
       // std::cout << out_str;
        urls_set.insert(final_url);
        s2 = res1.suffix(); //���������� ����� � ���������� �����
       // std::cout << "15 \n";
    };   
    //std::cout << "16 \n";
    //std::set<std::string> urls_set;
    return urls_set;
}

std::string html_parser::get_base_host(const std::string& url_str)
{
    std::string res_str = url_str;
    
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

    auto pos = res_str.find("/");
    if (pos != std::string::npos)
    {
        int num = res_str.size() - pos;
        res_str.erase(pos, num);
    }

    //std::cout << "get_base_host = " << res_str << "\n";
    return http_prefix + res_str;
}
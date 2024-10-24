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
    
    std::string s2 = html_body_str;

    //поиск ссылок <a href>
    std::smatch res1;
    std::regex r1("<a (.?[^>]*?)href=\"(.*?)\"");
    while (regex_search(s2, res1, r1))
    {     
        std::string find_str = res1.str();       
        std::string url_str = std::regex_replace(find_str, std::regex("<a (.?[^>]*?)href=\""), ""); //удалить все атрибуты между именем a и атрибутом href
        url_str = std::regex_replace(url_str, std::regex("\""), ""); //удалить все кавычки
        url_str = std::regex_replace(url_str, std::regex("/$"), ""); //удалить слеш в конце строки
       
        std::string new_base_path = get_base_path(url_str); //найти базовую часть пути        
        std::string suf_str = url_str.erase(0, new_base_path.size()); //найти имя страницы или скрипта       
        std::string final_url = complete_url(new_base_path, base_str) + suf_str; //финальный вид урла
       
        urls_set.insert(final_url);
        s2 = res1.suffix(); //продолжить поиск в оставшейся части      
    };     
    return urls_set;
}

std::string html_parser::clear_tags(const std::string& html_body_str)
{
    std::string s2 = html_body_str;

    //s2 = test_html_str;

    s2 = std::regex_replace(s2, std::regex("\n"), " ");
    s2 = std::regex_replace(s2, std::regex("\t"), " ");
    s2 = std::regex_replace(s2, std::regex(">"), "> ");
    s2 = std::regex_replace(s2, std::regex("< /"), "</");
    s2 = std::regex_replace(s2, std::regex("</ "), "</");
    s2 = std::regex_replace(s2, std::regex("< {1,}"), "<");
    s2 = std::regex_replace(s2, std::regex(" {1,}>"), ">");

  

  //  std::cout << "old s2 :\n" << s2 << "\n\n";

    std::string regex_str;

    //вытащить текст из title
    std::string res_str;
    std::smatch res1;
    regex_str = "<title>(.?)[^<]*";
   // std::regex r("<title>(.?)[^<]*");
    if (regex_search(s2, res1, std::regex(regex_str)))
    {
        res_str = res1.str();
        res_str = std::regex_replace(res_str, std::regex("<title>"), "");

     //   std::cout << "title_str = " << res_str << "\n";
        s2 = std::regex_replace(s2, std::regex(res_str), "");
        
      //  std::cout << "___s = " << s2 << "\n";
    }

    //удалить все до тега body
   // regex_str = "(.*?)<body>";
    regex_str = ("^.+?(<body)");

   // std::cout << "\n\n_______________s2\n" << s2;

    s2 = std::regex_replace(s2, std::regex(regex_str), "");
   // std::cout << "s2 no body :\n" << s2 << "\n\n";
    
   // std::cout << "\n\n_______________s2\n" << s2;

    regex_str = "<(.?)[^>][^<]*>";    
   // std::regex r1("<(.?)[^>][^<]*>");
    while (regex_search(s2, res1, std::regex(regex_str)))
    {
        s2 = std::regex_replace(s2, std::regex(regex_str), "");
      //  std::cout << "new s2 :\n" << s2 << "\n\n";
    };

    res_str += " ";
    res_str += s2;
    

    //res_str = std::regex_replace(res_str, std::regex("([.,:;-_~#$%^&*+=!?\\\"'])"), " ");
    //std::cout << "res str = \n" << res_str << "\n\n\n";
    res_str = std::regex_replace(res_str, std::regex("([\.,:;!?\\\"'*+=_~#$%^&])"), " "); //убрать знаки препинания и спец символы
    res_str = std::regex_replace(res_str, std::regex(" {2,}"), " "); //убрать лишние пробелы
    
    //все строчные
    std::transform(res_str.begin(), res_str.end(), res_str.begin(),
        [](unsigned char c) { return std::tolower(c); });

    //res_str = std::regex_replace(res_str, std::regex(" {2,}"), " ");
   // std::cout << "res str = \n" << res_str << "\n\n\n";
    return res_str;
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

    return http_prefix + res_str;
}

//void html_parser::collect_words(const std::string& text_str, std::map<std::string, unsigned  int>& words_map)
std::map<std::string, unsigned  int> html_parser::collect_words(const std::string& text_str)
{
    std::string search_str = text_str;   

    //извлечь следующее слово
    std::smatch res;
    std::regex r("(.[^ ]*)");

    std::map<std::string, unsigned  int> words_map;

        while (regex_search(search_str, res, r))
        {
            std::string find_str = res.str();        
            find_str = std::regex_replace(find_str, std::regex(" "), ""); 

            int len = find_str.size();
            if ((len >= min_word_len) && (len <= max_word_len))
            {
                auto word_pair = words_map.find(find_str);
                if (word_pair != words_map.end())
                {
                   // int words_count = words_map[find_str] + 1;
                    words_map[find_str] = words_map[find_str] + 1;//words_count;                   
                }
                else
                {
                    words_map[find_str] = 1;
                }
            }

            search_str = res.suffix();   
        };

        return words_map;
}

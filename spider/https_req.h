#include "http_req.h"


class https_req : public http_req
{
protected:
	virtual const std::string port() override { return "443"; }
	virtual const std::string url_start() override 
	{
		return "https://"; 
	}
	
private:
	

public:

	https_req(std::string _full_url) : http_req(_full_url) {};
	virtual bool get_page() override;
	
};
#include <airm/fp_setup.h>

namespace airm{

	std::string	FPSetup::getName()
	{
		return "FPSetup";
	}

	void	FPSetup::doGet(
		const StringMapPtr&	querys,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		//
	}
	void	FPSetup::doPost(
		const muradin::base::bytebuffer& data,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		//
	}
}//airm
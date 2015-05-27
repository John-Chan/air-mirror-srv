#include <airm/stream_setup.h>

namespace airm{

	std::string	StreamSetup::getName()
	{
		return "StreamSetup";
	}

	/*GET / stream.xml HTTP / 1.1
		X - Apple - Device - ID: 0xf4f15a2307da
		X - Apple - ProtocolVersion : 0
		X - Apple - Client - Name : Cj iPhone
		Content - Length : 0
		User - Agent : AirPlay / 211.3
*/
	void	StreamSetup::doGet(
		const StringMapPtr&	querys,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		//
	}
	void	StreamSetup::doPost(
		const muradin::base::bytebuffer& data,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		//
	}
}//airm
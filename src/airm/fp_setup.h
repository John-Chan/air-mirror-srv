#ifndef AIR_MIRROR_FAIRPLAY_SETUP_H
#define	AIR_MIRROR_FAIRPLAY_SETUP_H

#include <httpserver/user/simple_servlet.hpp>
#include <httpserver/user/servlet_fac.hpp>

#include <boost/noncopyable.hpp>

namespace airm{

	// GET /stream.xml HTTP/1.1
	// POST /fp-setup HTTP/1.1
	// POST /fp-setup HTTP/1.1
	// POST /stream HTTP/1.1
	// binary...(NOT HTTP)

	/*
	GET / stream.xml HTTP / 1.1
		X - Apple - Device - ID: 0xf4f15a2307da
		X - Apple - ProtocolVersion : 0
		X - Apple - Client - Name : Cj iPhone
		Content - Length : 0
		User - Agent : AirPlay / 211.3

		HTTP / 1.1 200 OK
	Date : Mon, 01 Dec 2014 03 : 03 : 41 GMT
		   Content - Type : text / x - apple - plist + xml
		   Content - Length : 425

		   < ? xml version = "1.0" encoding = "UTF-8" ? >
		   <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
		   <plist version = "1.0">
		   <dict>
		   .<key>height< / key>
		   .<integer>1080< / integer>
		   .<key>overscanned< / key>
		   .<integer>0< / integer>
		   .<key>refreshRate< / key>
		   .<real>0.016666666666666666< / real>
		   .<key>version< / key>
		   .<string>150.33< / string>
		   .<key>width< / key>
		   .<integer>1920< / integer>
		   < / dict>
		   < / plist>
		   */
	class FPSetup :boost::noncopyable, public httpd::SimpleServlet
	{
	public:
		virtual		std::string	getName();

		virtual	void	doGet(
			const StringMapPtr&	querys,
			const httpd::HttpSessionPtr& currentSession,
			const httpd::HttpRequestPtr& req,
			const httpd::HttpResponsePtr& rsp);
		virtual	void	doPost(
			const muradin::base::bytebuffer& data,
			const httpd::HttpSessionPtr& currentSession,
			const httpd::HttpRequestPtr& req,
			const httpd::HttpResponsePtr& rsp);
	};


	class FPSetupServiceFac : public httpd::IServletFactory
	{
	public:
		virtual	httpd::ServletPtr	create()
		{
			return httpd::ServletPtr(new FPSetup());
		}

	};
}//namespace

#endif // !AIR_MIRROR_FAIRPLAY_SETUP_H

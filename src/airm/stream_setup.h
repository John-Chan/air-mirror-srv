#ifndef AIR_MIRROR_STREAM_SETUP_H
#define	AIR_MIRROR_STREAM_SETUP_H

#include <httpserver/user/simple_servlet.hpp>
#include <httpserver/user/servlet_fac.hpp>

#include <boost/noncopyable.hpp>

namespace airm{
	
	// GET /stream.xml HTTP/1.1
	// POST /fp-setup HTTP/1.1
	// POST /fp-setup HTTP/1.1
	// POST /stream HTTP/1.1
	// binary...(NOT HTTP)
	class StreamSetup :boost::noncopyable, public httpd::SimpleServlet
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

	class StreamSetupServiceFac : public httpd::IServletFactory
	{
	public:
		virtual	httpd::ServletPtr	create()
		{
			return httpd::ServletPtr(new StreamSetup());
		}

	};
}//namespace

#endif // !AIR_MIRROR_STREAM_SETUP_H

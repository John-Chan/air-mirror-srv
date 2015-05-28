#ifndef AIR_MIRROR_FAIRPLAY_SETUP_H
#define	AIR_MIRROR_FAIRPLAY_SETUP_H

#include <httpserver/user/simple_servlet.hpp>
#include <httpserver/user/servlet_fac.hpp>

#include <boost/noncopyable.hpp>

namespace airm{

	// for /fp-setup
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
	private:
		void	handleFP1(
			const httpd::HttpSessionPtr& currentSession,
			const httpd::HttpRequestPtr& req,
			const httpd::HttpResponsePtr& rsp
			);
		void	handleFP2(
			const httpd::HttpSessionPtr& currentSession,
			const httpd::HttpRequestPtr& req,
			const httpd::HttpResponsePtr& rsp
			);
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

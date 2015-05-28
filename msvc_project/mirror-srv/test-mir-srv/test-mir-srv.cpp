// test-mir-srv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <airm/fp_setup.h>
#include <airm/stream_setup.h>

#include <httpserver/util/server_warper.hpp>
#include <httpserver/buildin/time_service.hpp>
#include <httpserver/loger/loger.hpp>
#include <muradin/system/stdio_helper.hpp>

using namespace httpd;
using namespace httph;

#include <boost/date_time.hpp>

void	run_srv_mt(unsigned short port)
{
	boost::asio::io_service	ios;
	ServerCtxPtr	ctx(new ServerCtx());

	std::string final_srv_path;
	// StreamOtcSetup /stream
	// StreamXmlSetup /stream.xml
	// FPSetup /fp-setup
	final_srv_path = ctx->getServiceFactory()->regServletFactoryForService(
		"/stream.xml",
		ServletFactoryPtr(new airm::StreamXmlSetupServiceFac()),
		false
		);
	LOG_INFO << " reg webservice on url: " << final_srv_path;

	final_srv_path = ctx->getServiceFactory()->regServletFactoryForService(
		"/stream",
		ServletFactoryPtr(new airm::StreamOtcSetupServiceFac()),
		false
		);
	LOG_INFO << " reg webservice on url: " << final_srv_path;

	final_srv_path = ctx->getServiceFactory()->regServletFactoryForService(
		"/fp-setup",
		ServletFactoryPtr(new airm::FPSetupServiceFac()),
		false
		);
	LOG_INFO << " reg webservice on url: " << final_srv_path;
	/*
	std::string welcomePage = "/hi.html";
	ctx->getWebAppCfg()->setWelcomePage(welcomePage);
	LOG_INFO << " setWelcomePage: " << welcomePage;
	*/

	TcpServerRunner runner(4);
	ThreadHolder holder;
	runner.start(ctx, port);
	holder.wait();
}

int _tmain(int argc, _TCHAR* argv[])
{
	//muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL::LOG_LVL_INFO);
	muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL::LOG_LVL_TRACE);
	//muradin::system::redirect_file_stream("test-mir-srv.log", "w", stdout);


	if (argc<2){
		std::cout << "miss parameter [port]" << std::endl;
		return -1;
	}
	unsigned short port = 34001;
	std::string port_str(argv[1]);
	muradin::string_helper::str_to_int<unsigned short>(port_str, 10, port);

	run_srv_mt(port);
	return 0;
}


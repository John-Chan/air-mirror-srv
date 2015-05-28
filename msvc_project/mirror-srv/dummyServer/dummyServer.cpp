// dummyServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <airm/tcp/tcp_server.hpp>
#include <muradin/boost_dep/loging_v2/loger.hpp>
#include <muradin/string_helper/str_int.hpp>

#include <exception>

void	showUsage()
{
	std::cout << "Usage : "<< " [port]" << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL::LOG_LVL_TRACE);
	boost::asio::io_service ios;
	unsigned short port = 34000;

	if (argc<2){
		std::cout <<"miss parameter [port]" << std::endl;
		return -1;
	}
	std::string port_str(argv[1]);
	muradin::string_helper::str_to_int<unsigned short>(port_str, 10, port);

	airm::TcpServer serv(ios, port);
	try	{
		LOG_INFO << "run server,port= " << port;
		ios.run();
	}
	catch (const std::exception& e){
		LOG_ERR << e.what();
	}
	catch (...){
		LOG_ERR << "Unkonw exception";
	}
	return 0;
}


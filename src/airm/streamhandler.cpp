#include <airm/streamhandler.h>
#include <muradin/boost_dep/loging_v2/loger.hpp>

#include <httpserver/util/str_helper.hpp>

namespace airm{

	void	TcpStreamHandler::handleRequest(
		const boost::shared_ptr<httpd::HttpSession>& httpSession,
		muradin::base::bytebuffer& incomeing,
		muradin::base::bytebuffer& out)
	{
		size_t dataSizeIn = incomeing.readable_bytes();
		LOG_DEBUG << " >>> " << "TCP" << " | " << httpd::formatedSize(dataSizeIn);

		processData(httpSession, incomeing,out);

		size_t dataSizeOut = out.readable_bytes();
		LOG_DEBUG << " >>> " << "TCP" << " | " << httpd::formatedSize(dataSizeOut);
	}

	void	TcpStreamHandler::processData(
		const boost::shared_ptr<httpd::HttpSession>& httpSession,
		muradin::base::bytebuffer& incomeing,
		muradin::base::bytebuffer& out)
	{
		incomeing.discard_all();
		out.discard_all();
	}

}//namespace
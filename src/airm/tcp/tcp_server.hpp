#ifndef AIRM_NET_TCP_SERVER_HPP
#define	AIRM_NET_TCP_SERVER_HPP

#include <airm/tcp/tcp_connection.hpp>
#include <muradin/boost_dep/loging_v2/loger.hpp>

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace airm{
	class TcpServer 
		:public boost::enable_shared_from_this<TcpServer>,private boost::noncopyable
	{
	private:
		boost::asio::io_service&	ioService;
		unsigned short	listenPort;
		boost::asio::ip::tcp::acceptor	acceptor;
		TcpConnectionPtr	newConnect;
	public:
		TcpServer(boost::asio::io_service& ios, unsigned short port)
			:ioService(ios),
			listenPort(port),
			acceptor(ios)
		{
			LOG_INFO << "TcpServer going to run,listen port :" << listenPort;
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), listenPort);
			acceptor.open(endpoint.protocol());
			acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor.bind(endpoint);
			acceptor.listen();

			startAccept();

		}
		~TcpServer()
		{
			//
		}

	private:
		void	startAccept()
		{
			newConnect.reset(new TcpConnection(ioService));
			acceptor.async_accept(newConnect->getSocket(), newConnect->getRemoteAddr(),
				boost::bind(&TcpServer::handleAccept, this, boost::asio::placeholders::error));
		}


		void		handleAccept(const boost::system::error_code& error)
		{
			if (!error){
				newConnect->start();
			}
			else{
				//std::cerr << __FUNCTION__ << "  " << ",error =" << error.value() << ",msg=" << error.message() << std::endl;
				LOG_ERR << "error =" << error.value() << ",msg=" << error.message();
			}

			startAccept();
		}
	};

	typedef	boost::shared_ptr<TcpServer>	TcpServerPtr;

}//namespace

#endif // !HTTPD_NET_TCP_SERVER_HPP

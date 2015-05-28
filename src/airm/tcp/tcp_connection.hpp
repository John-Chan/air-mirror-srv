#ifndef AIRM_NET_TCP_CONNECTION_HPP
#define	AIRM_NET_TCP_CONNECTION_HPP


#include <muradin/boost_dep/loging_v2/loger.hpp>
#include <muradin/base/bytebuffer.hpp>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace airm{
	class TcpConnection 
		: public boost::enable_shared_from_this<TcpConnection>,private boost::noncopyable
	{
	private:
		enum	{ MAX_READ_BYTES_ONCE = 1024 * 1024 };
		boost::asio::ip::tcp::socket	socket;
		boost::asio::ip::tcp::endpoint remoteAddr;
		bool			writing;
		bool			reading;
		size_t			readBytes;
		size_t			writeBytes;
		muradin::base::bytebuffer		writeBuff;
		muradin::base::bytebuffer		readBuff;

	public:

		TcpConnection(boost::asio::io_service& ios)
			:socket(ios), 
			writing(false),
			reading(false),
			readBytes(0),
			writeBytes(0)
		{
			//
		}
		virtual ~TcpConnection()
		{
			//
		}
		boost::asio::ip::tcp::socket&	getSocket()
		{
			return socket;
		}

		virtual void	start()
		{

			boost::system::error_code err;
			applySocketOpts(err);
			startRead();
		}

		const boost::asio::ip::tcp::endpoint& getRemoteAddr()const
		{
			return remoteAddr;
		}
		boost::asio::ip::tcp::endpoint& getRemoteAddr()
		{
			return remoteAddr;
		}
	private:

		void	checkNotReading(bool abort)
		{
			if (true == reading){
				LOG_FATAL << "Should not start read before a read operation completed";
				if (abort){
					::abort();
				}
			}
		}
		void	checkNotWriting(bool abort)
		{
			if (true == writing){
				LOG_FATAL << "Should not start write before a write operation completed"; 
				if (abort){
					::abort();
				}
			}
		}
		void	onReadMsg(const muradin::base::bytebuffer& in, muradin::base::bytebuffer& out)
		{
			LOG_TRACE << "READ " << in.readable_bytes() << " Bytes,Data=";
			LOG_TRACE << in.as_string();
		}
		void	applySocketOpts(boost::system::error_code& set_option_err)
		{
			socket.set_option(boost::asio::ip::tcp::no_delay(true), set_option_err);
			if (set_option_err){
				//std::cerr << "socket.set_option => " << set_option_err.message() << std::endl;
				LOG_ERR << "socket.set_option => " << set_option_err.message();
			}
			/*
			socket.set_option( boost::asio::socketbase::send_buffer_size( 65536 ) ,set_option_err);
			if(set_option_err){
			std::cerr <<"socket.set_option => " << set_option_err.message() <<std::endl;
			}
			socket.set_option( boost::asio::socketbase::receive_buffer_size( 65536 ),set_option_err );
			if(set_option_err){
			std::cerr <<"socket.set_option => " << set_option_err.message() <<std::endl;
			}
			*/
		}
		/// Handle completion of a read operation. 
		void handleRead(const boost::system::error_code& error, std::size_t bytes_transferred)
		{

			if (!error){
				readBytes += bytes_transferred;
				readBuff.has_written(bytes_transferred);
				onReadMsg(readBuff, writeBuff);
				readBuff.discard_all();

				reading = false;

				if (writeBuff.readable_bytes()>0){
					//std::cout << "going to send " << writeBuff.readable_bytes() << std::endl;
					//std::cout << writeBuff.as_string() << std::endl;
					LOG_DEBUG << "going to send " << writeBuff.readable_bytes();
					{

						size_t out_put_size = 256;
						if (out_put_size > writeBuff.readable_bytes()){
							out_put_size = writeBuff.readable_bytes();
						}
						
						LOG_DEBUG << writeBuff.as_string(0,out_put_size);
					}
					startWrite();
				}
				else{
					startRead();
				}

			}
			else{
				if (error == boost::asio::error::misc_errors::eof){
					LOG_INFO << " Client disconnectd:" << remoteAddr.address() << "  " << remoteAddr.port();
				}
				else{
					LOG_ERR << remoteAddr.address() << "  " << remoteAddr.port()
						<< ",error =" << error.value()
						<< ",msg=" << error.message();
				}
			}
			reading = false;
		}

		/// Handle completion of a write operation.
		void handleWrite(const boost::system::error_code& error, std::size_t bytes_transferred)
		{

			if (!error){
				//
				//std::cout << "send done,bytes_transferred:" << writeBuff.readable_bytes() << bytes_transferred << std::endl;
				LOG_DEBUG << "send done,bytes_transferred:"  << bytes_transferred;
				BOOST_ASSERT(bytes_transferred == writeBuff.readable_bytes());
				writeBytes += bytes_transferred;
				writeBuff.discard_all();
				writing = false;
				startRead();
			}
			else{ 
				LOG_ERR << remoteAddr.address() << "  " << remoteAddr.port()
					<< ",error =" << error.value()
					<< ",msg=" << error.message();
			}
			writing = false;

		}


		void	startRead()
		{
			checkNotReading(true);
			 
			reading = true;
			readBuff.ensure_write_space(MAX_READ_BYTES_ONCE);
			socket.async_read_some(boost::asio::buffer(readBuff.wt_ptr(), readBuff.writeable_bytes()),
				boost::bind(&TcpConnection::handleRead, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		void	startWrite()
		{

			checkNotWriting(true);

			writing = true;
			boost::asio::async_write(socket,
				boost::asio::buffer(writeBuff.rd_ptr(), writeBuff.readable_bytes()),
				boost::bind(&TcpConnection::handleWrite, shared_from_this(),
				boost::asio::placeholders::error,
				/*_1*/
				boost::asio::placeholders::bytes_transferred)
				);
		}
	};

	typedef	boost::shared_ptr<TcpConnection>	TcpConnectionPtr;
}//namespace

#endif // !AIRM_NET_TCP_CONNECTION_HPP

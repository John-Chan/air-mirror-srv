#ifndef AIR_MIRROR_STREAM_HANDLER_H
#define	AIR_MIRROR_STREAM_HANDLER_H

#include <httpserver/user/handler_low.hpp>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace airm{
	
	class TcpStreamHandler:public httpd::IDataHandler,boost::noncopyable
	{
	public:
		virtual void	handleRequest(
			const boost::shared_ptr<httpd::HttpSession>& httpSession,
			muradin::base::bytebuffer& incomeing,		/* process the incomeing data and clear it(or clear some),or you got them again */
			muradin::base::bytebuffer& out);
	private:
		void	processData(
			const boost::shared_ptr<httpd::HttpSession>& httpSession,
			muradin::base::bytebuffer& incomeing,		
			muradin::base::bytebuffer& out);
	};
}//namespace

#endif // !AIR_MIRROR_STREAM_HANDLER_H

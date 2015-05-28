#include <airm/stream_setup.h>

#include <airm/streamhandler.h>

#include <Plist.hpp>

#include <muradin/boost_dep/loging_v2/loger.hpp>

#include <boost/any.hpp>

#include <map>
#include <strstream>

namespace airm{
	/////////////////////////////////////////////////////////////////////////////
	/// StreamXmlSetup
	/////////////////////////////////////////////////////////////////////////////
	std::string	StreamXmlSetup::getName()
	{
		return "StreamXmlSetup";
	}

	void	StreamXmlSetup::doGet(
		const StringMapPtr&	querys,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		std::string key = "X-Apple-Device-ID";
		LOG_INFO << key << " : " << req->getHeader().getVal(key, ""); 
		key = "X-Apple-ProtocolVersion";
		LOG_INFO << key << " : " << req->getHeader().getVal(key, "");
		key = "X-Apple-Client-Name";
		LOG_INFO << key << " : " << req->getHeader().getVal(key, "");


		std::map<std::string, boost::any> dict;
		dict["height"] = 1080;
		dict["width"] = 1920;
		dict["overscanned"] = 0;
		dict["refreshRate"] = (double)0.016666666666666666;
		dict["version"] = 150.33;
		dict["height"] = 1080;

		std::ostrstream ss;
		Plist::writePlistXML(ss, dict);
		std::string repBody=ss.str();
		rsp->getStartLine().warp(httph::http_200);

		//Content-Type: text/x-apple-plist+xml
		httph::MimeType mimePlist("text","x-apple-plist+xml");
		rsp->getHeader().warpEntityInfo(mimePlist, repBody.length());
		rsp->getBody().discard_all();
		rsp->getBody().append(repBody.c_str(), repBody.length());
	}
	void	StreamXmlSetup::doPost(
		const muradin::base::bytebuffer& data,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		reject405(rsp);
	}


	/////////////////////////////////////////////////////////////////////////////
	/// StreamOtcSetup
	/////////////////////////////////////////////////////////////////////////////

	std::string	StreamOtcSetup::getName()
	{
		return "StreamOtcSetup";
	}

	void	StreamOtcSetup::doGet(
		const StringMapPtr&	querys,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		reject405(rsp);
	}
	void	StreamOtcSetup::doPost(
		const muradin::base::bytebuffer& data,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		reject405(rsp);
		size_t entitySize = req->getBody().readable_bytes();
		typedef std::map<std::string, boost::any>	MssgType;
		MssgType dict;
		Plist::readPlist<MssgType>(req->getBody().rd_ptr(), entitySize, dict);
		/// str: param1,param2,version
		/// int: sessionID 
		/// array of sstring: timestampInfo  
		std::string param1 = boost::any_cast<std::string> (dict["param1"]);
		std::string param2 = boost::any_cast<std::string> (dict["param2"]);
		int sessionID = boost::any_cast<int> (dict["sessionID"]);
		std::string version = boost::any_cast<std::string> (dict["version"]);

		LOG_INFO << "switch to TCP";
		currentSession->setUserDataHandler(httpd::DataHandlerPtr(new TcpStreamHandler()));
	}
}//airm
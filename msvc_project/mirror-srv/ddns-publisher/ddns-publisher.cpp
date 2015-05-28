// ddns-publisher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ddnspp/dnssd.h>
#include <ddnspp/initlog.hpp>
#include <ddnspp/logv/logv.hpp>
#include <iostream>
#include <boost/thread.hpp>


air::bonjour::LocalServicePtr	makeService(boost::asio::io_service& ios, const air::bonjour::DNSDApi& dll, std::string srv_type, std::string name, std::string port, air::common::StringMap records)
{
	//  -R "My Test" _http._tcp . 80 path=/path-to-page.html
	air::bonjour::ServiceFactory fac(ios, dll);
	air::bonjour::ServiceType st(air::bonjour::SP_TCP, srv_type);
	air::bonjour::TxtRecordEncoder txtEncoder(dll, 5, true);
	const char* old_ptr = txtEncoder.getRecordPtr();
	air::bonjour::BonjourError err;
	err = txtEncoder.putOrReplace(records);
	if (err){
		LOG_ERR << "txtEncoder.putOrReplace " << err.getMessage() << std::endl;
	}
	size_t len = txtEncoder.getRecordLenth();
	const char* new_ptr = txtEncoder.getRecordPtr();
	//air::bonjour::LocalServicePtr ptr=fac.registerService(name,st,"local","sunnysce.local",port,txtEncoder,NULL,err);
	air::bonjour::LocalServicePtr ptr = fac.registerService(name, st, "", "", port, txtEncoder, NULL, err);
	if (err){
		LOG_ERR << "RegisterService " << err.getMessage() << std::endl;
	}
	return ptr;
}

int	publish(const std::string& airplayPort, const std::string& raopPort )
{

	boost::asio::io_service ios;
	std::cout << "loading dnssd.dll" << std::endl;
	air::bonjour::DNSDApi dll("dnssd.dll");
	if (!dll.load()){
		std::cout << "dll.load fail" << std::endl;
		return -1;
	}
	air::bonjour::BonjourUtility utl(dll);
	unsigned int sdk_ver, daemon_ver;
	unsigned int sdk_ver_major, sdk_ver_min, daemon_ver_major, daemon_ver_min;
	sdk_ver = utl.getSdkVersionDef(0);
	daemon_ver = utl.getDaemonVersionDef(0);
	air::bonjour::BonjourUtility::extractVersionNumber(sdk_ver, sdk_ver_major, sdk_ver_min);
	air::bonjour::BonjourUtility::extractVersionNumber(daemon_ver, daemon_ver_major, daemon_ver_min);
	LOG_INFO << "Use bonjour SDK ,version: " << sdk_ver_major << "." << sdk_ver_min;
	LOG_INFO << "Bonjour Server ,version: " << daemon_ver_major << "." << daemon_ver_min;

	// "vv=1 pw=0 srcvers=150.33 deviceid=00:0E:C6:F5:6B:46 features=0x100029ff model=AppleTV3,1 "
	air::common::StringMap airPlayRecords;
	airPlayRecords.insert(air::common::StringMap::value_type("vv", "1"));
	airPlayRecords.insert(air::common::StringMap::value_type("pw", "0"));
	airPlayRecords.insert(air::common::StringMap::value_type("srcvers", "150.33"));
	//38:59:F9:85:AD:23
	//00:0E:C6:F5:6B:46
	airPlayRecords.insert(air::common::StringMap::value_type("deviceid", "00:0E:C6:F5:6B:46"));
	airPlayRecords.insert(air::common::StringMap::value_type("features", "0x100029ff"));
	airPlayRecords.insert(air::common::StringMap::value_type("model", "AppleTV3,1"));

	// "txtvers=1 vv=1 cn=0,1,2,3 ek=1 am=AppleTV3,1 et=0,1,3 sv=false tp=UDP sm=false ss=16 sr=44100 vs=150.33 ch=2 vn=65537 da=true md=0,1,2 sf=0x4 pw=false "
	air::common::StringMap raopRecords;
	raopRecords.insert(air::common::StringMap::value_type("txtvers", "1"));
	raopRecords.insert(air::common::StringMap::value_type("vv", "1"));
	raopRecords.insert(air::common::StringMap::value_type("cn", "0,1,2,3"));
	raopRecords.insert(air::common::StringMap::value_type("ek", "1"));
	raopRecords.insert(air::common::StringMap::value_type("am", "AppleTV3,1"));
	raopRecords.insert(air::common::StringMap::value_type("et", "0,1,3"));
	raopRecords.insert(air::common::StringMap::value_type("sv", "false"));
	raopRecords.insert(air::common::StringMap::value_type("tp", "UDP"));
	raopRecords.insert(air::common::StringMap::value_type("sm", "false"));
	raopRecords.insert(air::common::StringMap::value_type("ss", "16"));
	raopRecords.insert(air::common::StringMap::value_type("sr", "44100"));
	raopRecords.insert(air::common::StringMap::value_type("vs", "150.33"));
	raopRecords.insert(air::common::StringMap::value_type("ch", "2"));
	raopRecords.insert(air::common::StringMap::value_type("vn", "65537"));
	raopRecords.insert(air::common::StringMap::value_type("da", "true"));
	raopRecords.insert(air::common::StringMap::value_type("md", "0,1,2"));
	raopRecords.insert(air::common::StringMap::value_type("sf", "0x4"));
	raopRecords.insert(air::common::StringMap::value_type("pw", "false"));

	boost::thread_group threads;
	std::string airplaySrvName = "airplay-cj";
	std::string raopSrvName = "000EC6F56B46@cj";
	//std::string airplayPort="7000";
	//std::string raopPort="49152";
	//std::string airplayPort = "34000";
	//std::string raopPort = "34000";

	LOG_INFO << "reg airplaySvc,port= " << airplayPort;
	LOG_INFO << "reg raopSvc,port= " << raopPort;
	air::bonjour::LocalServicePtr airplaySvc = makeService(ios, dll, "_airplay", airplaySrvName, airplayPort, airPlayRecords);
	air::bonjour::LocalServicePtr raopSvc = makeService(ios, dll, "_raop", raopSrvName, raopPort, raopRecords);


	threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios))));
	threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios))));
	threads.join_all();

	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//std::string airplayPort="7000";
	//std::string raopPort="49152";
	std::string airplayPort = "7000";
	std::string raopPort = "34000";

	publish(airplayPort, raopPort);
	return 0;
}


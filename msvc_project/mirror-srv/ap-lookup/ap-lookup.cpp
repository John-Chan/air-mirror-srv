// ap-lookup.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <ddnspp/dnssd.h>
#include <ddnspp/initlog.hpp>
#include <ddnspp/logv/logv.hpp>
#include <iostream>
#include <boost/thread.hpp>
#include <list>


#include <muradin/system/stdio_helper.hpp>

typedef std::list<air::bonjour::RemoteServicePtr>	BonjourServiceBrowerPtrList;

#define	LOOKINGFOR_ALL	0

void	listAllService(air::bonjour::ServiceTypeList& list)
{
#if LOOKINGFOR_ALL
	air::bonjour::ServiceTypePraser::allWellkonwServiceTypeList(list);

#else

	list.push_back(air::bonjour::ServiceType(air::bonjour::SP_TCP, "_raop"));
	list.push_back(air::bonjour::ServiceType(air::bonjour::SP_TCP, "_airplay"));
#endif // LOOKINGFOR_ALL

	
}
void	onResolveAddress
(
air::bonjour::ServiceFactory& fac,
air::bonjour::AddressResolveData	data
)
{
	if (data.error){
		LOG_ERR << "onResolveAddress fail:" << data.error.getMessage()  ;
	}
	else{
		LOG_INFO << "onResolveAddress success:" << data.hostName << " <= " << data.addr.to_string()  ;
	}
	//
}
//
void	onResolveService(air::bonjour::ServiceFactory& fac,
	air::bonjour::ServiceResolveData data)
{
	air::bonjour::AddressResolverPtr service;

	/// must close it or you got xxx alive 2 minutes error in windows event manager
	fac.removeService(data.owner);
	data.owner->close();
	data.owner.reset();


#if 0
	air::bonjour::BonjourError err;
	if (!data.error){
		service = fac.createAddressResolver(
			air::bonjour::IP_V4,
			data.hostName,
			boost::bind(&onResolveAddress, boost::ref(fac), _1),
			err);
	}
	if (err){
		LOG_ERR << "createAddressResolver fail:" << err.getMessage();
	}
#endif
}

void	onFoundService(air::bonjour::ServiceFactory& fac,
	air::bonjour::RemoteServiceData	data)
{
	//air::bonjour::ServiceResolverPtr rolser=fac.createServiceResolver(name,type,domain,err);
	air::bonjour::ServiceResolverPtr rolser;

	air::bonjour::BonjourError err;
	if (!data.error){
		//rolser=fac.createServiceResolver(name,type,domain,boost::bind(&onResolveService,boost::ref(fac),_1,_2,_3,_4,_5,_6,_7,_8),err);
		rolser = fac.createServiceResolver(data.serviceName, data.serviceType, data.domainName, boost::bind(&onResolveService, boost::ref(fac), _1), err);
	}
	if (err){
		LOG_INFO << "createServiceResolver fail:" << err.getMessage()  ;
	}
}

void	close_all(air::bonjour::ServiceFactory& fac, BonjourServiceBrowerPtrList& all_srv)
{
	while (all_srv.size() > 0){
		boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
		air::bonjour::RemoteServicePtr ptr = all_srv.front();
		all_srv.pop_front();
		ptr->getCoreContext()->close();
		fac.removeService(ptr);
	}
}

int	lookup()
{
	boost::asio::io_service ios;
	LOG_INFO << "loading dnssd.dll"  ;
	air::bonjour::DNSDApi dll("dnssd.dll");
	if (!dll.load()){
		LOG_ERR << "dll.load fail"  ;
		return -1;
	}
	air::bonjour::BonjourError err;
	air::bonjour::ServiceFactory fac(ios, dll);

	air::bonjour::ServiceTypeList all_type_tcp;
	BonjourServiceBrowerPtrList all_srv;
	listAllService(all_type_tcp);

	air::bonjour::ServiceTypeList::const_iterator it = all_type_tcp.begin();
	air::bonjour::ServiceTypeList::const_iterator over = all_type_tcp.end();

	while (it != over){
		air::bonjour::RemoteServicePtr srv = fac.createServiceBrower(*it, "local", boost::bind(&onFoundService, boost::ref(fac), _1), err);
		if (NULL == srv){
			LOG_ERR << "createServiceBrower fail:" << err.getMessage()  ;
		}
		else{
			all_srv.push_back(srv);
		}
		++it;
	}

	/// 
	air::bonjour::DomainEumeraterPtr srv = fac.createDomainEumerater(0, NULL, err);
	if (NULL == srv){
		LOG_ERR << "createServiceBrower fail:" << err.getMessage()  ;
	}

	boost::thread_group threads;
	size_t	thread_size = 1;
	for (size_t i = 0; i<thread_size; ++i){
		threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios))));
	}

	/// close is optional
	//close_all(fac,all_srv);

	threads.join_all();

	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL::LOG_LVL_TRACE);
	//muradin::system::redirect_file_stream("dns-lookup.log", "w", stdout);
	lookup();
	return 0;
}


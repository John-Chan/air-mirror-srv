#include <airm/fp_setup.h>
#include <airm/fairplay.hpp>

#include <muradin/boost_dep/loging_v2/loger.hpp>

#include <boost/assert.hpp>

char	type;				// [04]			02 for audio, 03 for mirroring, and apparently 0x64 for remote - pairing ?
char	tag1;				// [05]			Seems to be 0x1 to establish a session, and 0x2 for an encrypted key
char	seq;				// [06]			Sequence number
char	unknow1[4];			// [07 - 10]	Unknown.Probably reserved : Appears to always be zeroes ?
unsigned char	bodyLength;	// [11]			Length of payload
namespace airm{
	static void	log_fp_pack(const std::string& title,const void* ptr,size_t bytes)
	{
		const fplay::FP_HEADER* head_ptr = (const fplay::FP_HEADER*)(ptr);
		LOG_INFO << title;
		LOG_INFO << "full pack" << muradin::loger_v2::HexLogRecord(ptr, bytes);
		LOG_INFO << "header: "
			<< "type = " << (int)(head_ptr->type) << ","
			<< "seq = " << (int)(head_ptr->seq) << ","
			<< "bodyLength = " << (int)(head_ptr->bodyLength);

		if (bytes > fplay::fp_header_size()){
			const unsigned char* playload_ptr = ((const unsigned char*)ptr) + fplay::fp_header_size();
			LOG_INFO << "playload=> " << muradin::loger_v2::HexLogRecord(playload_ptr, head_ptr->bodyLength);
		}
	}
	
	std::string	FPSetup::getName()
	{
		return "FPSetup";
	}

	void	FPSetup::doGet(
		const StringMapPtr&	querys,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		this->reject405(rsp);
	}
	void	FPSetup::doPost(
		const muradin::base::bytebuffer& data,
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp)
	{
		size_t dataSize = req->getBody().readable_bytes();
		if (dataSize > 4){
			if (memcmp(req->getBody().rd_ptr(), fplay::FPLY_MAGIC, 4) == 0){
				if (dataSize == 16){
					handleFP1(currentSession, req, rsp);
				}
				else if (dataSize == 164){
					handleFP2(currentSession, req,rsp);
				}
			}
		}
	}

	void	FPSetup::handleFP1(
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp
		)
	{
		size_t dataBytes = req->getBody().readable_bytes();
		// should got 16 bytes data
		BOOST_ASSERT(dataBytes==16);
		/* should return:
		Content-Type: application/octet-stream
		X-Apple-ET: 32
		Content-Length: 142
		Server: AirTunes/150.33
		*/

		//static const size_t	kRspTotalBytes = 142;
		static const size_t rsp_playload_size = 130;
		const fplay::FP_HEADER* reqHeader = (const fplay::FP_HEADER*)(req->getBody().rd_ptr());

		log_fp_pack("FP-REQ", req->getBody().rd_ptr(), req->getBody().readable_bytes());

		fplay::FP_HEADER	rspHeader;
		fplay::fp_make_empty_header(rspHeader);

		rspHeader.seq = reqHeader->seq + 1;
		rspHeader.tag1 = reqHeader->tag1;
		rspHeader.type = reqHeader->type;
		rspHeader.bodyLength = (unsigned char)(rsp_playload_size);


		httph::MimeType mimeOtcStream("application", "octet-stream");
		rsp->getHeader().addOrUpdate("X-Apple-ET", "32");
		rsp->getHeader().addOrUpdate("Server", "AirTunes/150.33");
		rsp->getBody().discard_all();
		rsp->getBody().append(&rspHeader, fplay::fp_header_size());

		/*
		00000295  46 50 4c 59 03 01 02 00  00 00 00 82 02 03 90 8b FPLY.... ........
		000002A5  5f 89 47 4e 8e 7e c0 0f  44 3f ea ca 28 3a 7d ea _.GN.~..D ? ..(:
		}.
		000002B5  46 06 8f a8 4b e5 d5 83  80 f4 f2 ac a1 b9 8c 00 F...K... ........
		000002C5  ba 33 98 eb 2a 94 a7 7b  9d 62 8f ac 00 86 3b b8 .3..*..{.b....; .
		000002D5  a5 00 9b f1 5d 9e 1e 8f  4f ac 11 8a 28 2b 93 a9 ....]... O...(+..
		000002E5  b9 54 f0 0a 63 65 43 2b  95 0b 05 b9 40 67 b7 fe.T..ceC + ....@g..
		000002F5  9f a1 b9 31 d5 49 e1 6d  87 28 0c 2d 36 0f 7a 95 ...1.I.m .(. - 6.z.
		00000305  de 37 3b ba d1 e1 4a 35  ea 5f 0f f2 85 a2 dc b9 .7; ...J5._......
		00000315  0f cb 9a 52 b8 e5 bf 7f  9a 1a 07 00 cf fc       ...R.... ......
		*/
		/*
		02 03 90 8b 5f 89 47 4e 8e 7e c0 0f  44 3f ea ca 28 3a 7d ea
		46 06 8f a8 4b e5 d5 83  80 f4 f2 ac a1 b9 8c 00
		ba 33 98 eb 2a 94 a7 7b  9d 62 8f ac 00 86 3b b8
		a5 00 9b f1 5d 9e 1e 8f  4f ac 11 8a 28 2b 93 a9
		b9 54 f0 0a 63 65 43 2b  95 0b 05 b9 40 67 b7 fe
		9f a1 b9 31 d5 49 e1 6d  87 28 0c 2d 36 0f 7a 95
		de 37 3b ba d1 e1 4a 35  ea 5f 0f f2 85 a2 dc b9
		0f cb 9a 52 b8 e5 bf 7f  9a 1a 07 00 cf fc

		*/

		/*
		02, 03, 90, 8b, 5f, 89, 47, 4e, 8e, 7e, c0, 0f, 44, 3f, ea, 
		ca, 28, 3a, 7d, ea, 46, 06, 8f, a8, 4b, e5, d5, 83, 80, f4, 
		f2, ac, a1, b9, 8c, 00, ba, 33, 98, eb, 2a, 94, a7, 7b, 9d, 
		62, 8f, ac, 00, 86, 3b, b8, a5, 00, 9b, f1, 5d, 9e, 1e, 8f, 
		4f, ac, 11, 8a, 28, 2b, 93, a9, b9, 54, f0, 0a, 63, 65, 43, 
		2b, 95, 0b, 05, b9, 40, 67, b7, fe, 9f, a1, b9, 31, d5, 49, 
		e1, 6d, 87, 28, 0c, 2d, 36, 0f, 7a, 95, de, 37, 3b, ba, d1, 
		e1, 4a, 35, ea, 5f, 0f, f2, 85, a2, dc, b9, 0f, cb, 9a, 52, 
		b8, e5, bf, 7f, 9a, 1a, 07, 00, cf, fc                       

		*/

		char unkonw[rsp_playload_size] = {
			0x02, 0x03, 0x90, 0x8b, 0x5f, 0x89, 0x47, 0x4e, 0x8e, 0x7e, 0xc0, 0x0f, 0x44, 0x3f, 0xea,
			0xca, 0x28, 0x3a, 0x7d, 0xea, 0x46, 0x06, 0x8f, 0xa8, 0x4b, 0xe5, 0xd5, 0x83, 0x80, 0xf4,
			0xf2, 0xac, 0xa1, 0xb9, 0x8c, 0x00, 0xba, 0x33, 0x98, 0xeb, 0x2a, 0x94, 0xa7, 0x7b, 0x9d,
			0x62, 0x8f, 0xac, 0x00, 0x86, 0x3b, 0xb8, 0xa5, 0x00, 0x9b, 0xf1, 0x5d, 0x9e, 0x1e, 0x8f,
			0x4f, 0xac, 0x11, 0x8a, 0x28, 0x2b, 0x93, 0xa9, 0xb9, 0x54, 0xf0, 0x0a, 0x63, 0x65, 0x43,
			0x2b, 0x95, 0x0b, 0x05, 0xb9, 0x40, 0x67, 0xb7, 0xfe, 0x9f, 0xa1, 0xb9, 0x31, 0xd5, 0x49,
			0xe1, 0x6d, 0x87, 0x28, 0x0c, 0x2d, 0x36, 0x0f, 0x7a, 0x95, 0xde, 0x37, 0x3b, 0xba, 0xd1,
			0xe1, 0x4a, 0x35, 0xea, 0x5f, 0x0f, 0xf2, 0x85, 0xa2, 0xdc, 0xb9, 0x0f, 0xcb, 0x9a, 0x52,
			0xb8, 0xe5, 0xbf, 0x7f, 0x9a, 0x1a, 0x07, 0x00, 0xcf, 0xfc
		};

		rsp->getBody().append(unkonw, rspHeader.bodyLength);

		rsp->getHeader().warpEntityInfo(mimeOtcStream, rsp->getBody().readable_bytes());

		log_fp_pack("FP-RSP", rsp->getBody().rd_ptr(), rsp->getBody().readable_bytes());
	}
	void	FPSetup::handleFP2(
		const httpd::HttpSessionPtr& currentSession,
		const httpd::HttpRequestPtr& req,
		const httpd::HttpResponsePtr& rsp
		)
	{
		size_t dataBytes = req->getBody().readable_bytes();
		// should got 16 bytes data
		BOOST_ASSERT(dataBytes == 164);

		/* should return:		Content-Type: application/octet-stream
		X-Apple-ET: 32		Content-Length: 32
		*/

		//static const size_t	kRspTotalBytes = 32;
		static const size_t rsp_playload_size = 20;
		const fplay::FP_HEADER* reqHeader = (const fplay::FP_HEADER*)(req->getBody().rd_ptr());

		log_fp_pack("FP-REQ", req->getBody().rd_ptr(), req->getBody().readable_bytes());

		fplay::FP_HEADER	rspHeader;
		fplay::fp_make_empty_header(rspHeader);

		rspHeader.seq = reqHeader->seq + 1;
		rspHeader.tag1 = reqHeader->tag1;
		rspHeader.type = reqHeader->type;
		rspHeader.bodyLength = (unsigned char)(rsp_playload_size);

		httph::MimeType mimeOtcStream("application", "octet-stream");
		rsp->getHeader().addOrUpdate("X-Apple-ET", "32");

		rsp->getBody().discard_all();
		rsp->getBody().append(&rspHeader, fplay::fp_header_size());

		/*
		00000382  46 50 4c 59 03 01 04 00 00 00 00 14 0d fb 56 99 FPLY.... ......V.
		00000392  02 79 6a 6e c1 b0 99 8a 89 dd c4 f6 df aa 64 98 .yjn.... ......d.
		*/

		/* data
		0x0d, 0xfb, 0x56, 0x99, 0x02, 0x79, 0x6a, 0x6e, 0xc1, 0xb0, 0x99, 0x8a, 0x89, 0xdd, 0xc4, 0xf6, 0xdf, 0xaa, 0x64, 0x98
		*/
		/* data
		0x0d, 0xfb, 0x56, 0x99, 0x02, 0x79, 0x6a, 0x6e, 0xc1, 0xb0, 0x99, 0x8a, 0x89, 0xdd, 0xc4, 0xf6, 0xdf, 0xaa, 0x64, 0x98
		*/
		char unkonw[rsp_playload_size] = {
			0x0d, 0xfb, 0x56, 0x99, 0x02, 0x79, 0x6a, 0x6e, 0xc1, 0xb0, 0x99, 0x8a, 0x89, 0xdd, 0xc4, 0xf6, 
			0xdf, 0xaa, 0x64, 0x98
		};
		rsp->getBody().append(unkonw, rspHeader.bodyLength);

		rsp->getHeader().warpEntityInfo(mimeOtcStream, rsp->getBody().readable_bytes());
		log_fp_pack("FP-RSP", rsp->getBody().rd_ptr(), rsp->getBody().readable_bytes());
	}
}//airm
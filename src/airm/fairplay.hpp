#ifndef AIRM_FAIRPLAY_HPP
#define	AIRM_FAIRPLAY_HPP
#include <memory.h>
namespace airm{
	namespace fplay{

		static const char* FPLY_MAGIC = "FPLY";

#pragma pack (1)
		typedef struct FP_PACK_HEADER_{
			char	magic[4];			// [00 - 03]	Magic bytes "FPLY"
			char	type;				// [04]			02 for audio, 03 for mirroring, and apparently 0x64 for remote - pairing ?
			char	tag1;				// [05]			Seems to be 0x1 to establish a session, and 0x2 for an encrypted key
			char	seq;				// [06]			Sequence number
			char	unknow1[4];			// [07 - 10]	Unknown.Probably reserved : Appears to always be zeroes ?
			unsigned char	bodyLength;	// [11]			Length of payload
		}FP_HEADER;
#pragma pack ()

		inline	size_t		fp_header_size()
		{
			return sizeof(FP_HEADER);
		}

		inline FP_HEADER& fp_make_empty_header(FP_HEADER& header)
		{
			::memset(&header, 0x0, sizeof(FP_HEADER));
			header.magic[0] = 'F';
			header.magic[0] = 'P';
			header.magic[0] = 'L';
			header.magic[0] = 'Y';
			return header;
		}
	}//namespace
}//namespace

#endif	//!AIRM_FAIRPLAY_HPP

#pragma once

using std::string;


const char gHeaderChecker[ 32] = "MAIET Encrypted File System";

union RSAHEADER
{
	byte byteHeader[ 37];					// 37 byte

	struct
	{
		char szHeaderChecker[ 32];			// 32 byte	: 파일 확인용 캐릭터
		unsigned long nFileLength;			// 4 byte	: 헤더를 제외한 원본 파일의 길이
		byte nCRC;							// 1 byte	: 헤더를 제외한 원본 파일의 CRC값
	};

	RSAHEADER()					{ memset( byteHeader, 0, sizeof( byteHeader));								}

	void SetHeaderChecker()		{ memcpy( szHeaderChecker, gHeaderChecker, sizeof( gHeaderChecker));		}
	bool IsValidHeader()		{ return (strcmp( szHeaderChecker, gHeaderChecker) == 0) ? true : false;	}
	unsigned long GetLength()	{ return nFileLength;														}
	byte GetCRC()				{ return nCRC;																}
};


string RSADecryptString( const byte *privKeyBuff, size_t keyLength, const char *dataBuff, size_t dataLength,  RSAHEADER* pHeader =NULL);
RSAHEADER RSAGetHeader( const char* buff);



#include "stdafx.h"

#include "MRsaEncrypt.h"
//#include "rsa.h"
//#include "randpool.h"
//#include "gzip.h"
//#include "hex.h"
//#include "files.h"


using namespace std;
//using namespace CryptoPP;

#ifdef _DEBUG
#else
#pragma comment (lib,"cryptlib2003.lib")
#endif


//RandomNumberGenerator & GlobalRNG()
//{
//	static RandomPool randomPool;
//	return randomPool;
//}


string RSADecryptString( const byte *privKeyBuff, size_t keyLength, const char *dataBuff, size_t dataLength, RSAHEADER* pHeader)
{
	//string result;

	//try
	//{
	//	// Read private key
	//	StringSource privFile( privKeyBuff, keyLength, true);
	//	RSAES_PKCS1v15_Decryptor priv( privFile);

	//	// Get size of 1 block
	//	size_t nCipherTextLength = priv.FixedCiphertextLength();

	//	// Set string source
	//	StringSource stringSource( (byte *)dataBuff, dataLength, false);

	//	// Get header
	//	string strHeader;
	//	stringSource.Attach( new StringSink( strHeader));
	//	stringSource.Pump( sizeof( RSAHEADER));

	//	RSAHEADER header;
	//	memcpy( &header, strHeader.c_str(), sizeof( RSAHEADER));

	//	if ( pHeader != NULL)
	//		*pHeader = header;

	//	// Decrypt
	//	string strBuff;
	//	StringSink* pOutput = new StringSink( strBuff);
	//	PK_DecryptorFilter *pDecryptFilter = new PK_DecryptorFilter( GlobalRNG(), priv, pOutput);
	//	stringSource.Attach( pDecryptFilter);

	//	// Pump data
	//	lword nPump = 0;
	//	while ( nPump = stringSource.Pump( nCipherTextLength))
	//		pDecryptFilter->MessageEnd();

	//	pOutput->MessageEnd();

	//	// Unzip code
	//	string strBuffUnzip;
	//	StringSource( strBuff, true, new Gunzip( new StringSink( strBuffUnzip)));
	//	strBuff.clear();

	//	// Check CRC
	//	byte nCRC = 0;
	//	for ( string::iterator itr = strBuffUnzip.begin();  itr != strBuffUnzip.end();  itr++)
	//		nCRC += (byte)(*itr);

	//	if ( header.GetCRC() != nCRC)
	//	{
	//		assert( "File CRC error.");
	//		return false;
	//	}

	//	// Save to file
	//	StringSource( strBuffUnzip, true, new StringSink( result));
	//	strBuffUnzip.clear();
	//}

	//// Error
	//catch ( CryptoPP::Exception)
	//{
	//	assert( 0);
	//}


	//return result;

	return "";
}


RSAHEADER RSAGetHeader( const char* buff)
{
	RSAHEADER header;
	memcpy( header.byteHeader, buff, sizeof( RSAHEADER));

	return header;
}


#include "stdafx.h"
#include "MMatchDebug.h"
#include "MPdb.h"

void SetSecurityErrorHandler()
{

}

void __cdecl ReportBufferOverrun(int code, void * unused)
{
   {
		mlog("\n!!!ERROR: Buffer overrun detected! Program will end.\n");

		// string str;
		// GetCrashInfo(NULL, str);
		// mlog(str.c_str());

		char szCrashLogFileName[ 1024 ] = {0,};
		_snprintf( szCrashLogFileName, 1024, "Crash_%s", MGetLogFileName() );
		WriteCrashInfo( NULL, szCrashLogFileName );
   }

   exit(1);
}

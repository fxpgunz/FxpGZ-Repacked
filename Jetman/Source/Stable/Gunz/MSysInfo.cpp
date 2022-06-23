#include "StdAfx.h"
#include "MDebug.h"
#include "d3d9.h"
//#include <dsound.h>

void MSysInfoLog_CPU()
{
	LARGE_INTEGER ulFreq, ulTicks, ulValue;

	// it is number of ticks per seconds
	QueryPerformanceFrequency(&ulFreq);
	// current value of the performance counter

	QueryPerformanceCounter(&ulTicks);
	// calculate one-second interval
	ulValue.QuadPart = ulTicks.QuadPart + ulFreq.QuadPart;
	// read time stamp counter
	// this asm instruction load the highorder 32 bit of the
	// register into EDX and the lower order 32 bits into EAX
	// Get extended ids.
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	// Get the information associated with each extended ID.
	char CPUBrandString[0x40] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);

		// Interpret CPU brand string and cache information.
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(CPUBrandString + 16,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}

	}
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	char szDesc[512] = "";
	sprintf(szDesc, "CPU ID = %s ( family = %d , model = %d , stepping = %d )\n",
		CPUBrandString, sysInfo.wProcessorArchitecture, sysInfo.dwProcessorType, sysInfo.wProcessorRevision
	);//sysInfo->);
	mlog(szDesc);
}

void MSysInfoLog_Display()
{
	HMODULE					hD3DLibrary=NULL;
	LPDIRECT3D9				pD3D=NULL;
	LPDIRECT3DDEVICE9		pLPDIRECT3DDEVICE9=NULL;
	D3DADAPTER_IDENTIFIER9	deviceID;
/*
	hD3DLibrary = LoadLibrary( "d3d9.dll" );

	if (!hD3DLibrary)
	{
		mlog("Error, could not load d3d9.dll");
		return;
	}*/

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D == nullptr)
	{
		mlog("Error creating device.\n");
		return;
	}



	pD3D->GetAdapterIdentifier(0,0,&deviceID);
	pD3D->Release();

#ifndef _PUBLISH
	mlog("D3D_SDK_VERSION = %d \n", D3D_SDK_VERSION);
#endif

	mlog("Display Device = %s ( vendor=%x device=%x subsys=%x revision=%x )\n", 
		deviceID.Description,deviceID.VendorId,deviceID.
		DeviceId,deviceID.SubSysId,deviceID.Revision);

	mlog("Display Driver Version = %d.%d.%04d.%04d\n",
		deviceID.DriverVersion.HighPart >> 16 , deviceID.DriverVersion.HighPart & 0xffff,
		deviceID.DriverVersion.LowPart >> 16 , deviceID.DriverVersion.LowPart & 0xffff );
}

//BOOL CALLBACK MSysInfoLog_Audio_EnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
//{
//	mlog("Audio : %s %s \n",lpszDesc,lpszDrvName);
//    return TRUE;
//}
//
//void MSysInfoLog_Audio()
//{
//	HMODULE					hDSoundLibrary=NULL;
//	hDSoundLibrary = LoadLibrary( "dsound.dll" );
//	if (!hDSoundLibrary)
//	{
//		mlog("Error, could not load dsound.dll");
//		return;
//	}
//
//	typedef HRESULT * (__stdcall *DIRECTSOUNDENUMERATETYPE)(LPDSENUMCALLBACK  lpDSEnumCallback,LPVOID  lpContext);
//	DIRECTSOUNDENUMERATETYPE pDirectSoundEnumerate = (DIRECTSOUNDENUMERATETYPE) GetProcAddress(hDSoundLibrary, "DirectSoundEnumerate");
//
//	if (!pDirectSoundEnumerate)
//	{
//		mlog("Error, could not get proc adress of Direct3DCreate9.");
//		FreeLibrary(hDSoundLibrary);
//		return;
//	}
//
//	*pDirectSoundEnumerate((LPDSENUMCALLBACK)MSysInfoLog_Audio_EnumProc, NULL);
//
//}

void MSysInfoLog_OS()
{
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*)&os);

	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(MEMORYSTATUSEX);
	::GlobalMemoryStatusEx(&ms);
	ULONGLONG dwPhysicalMemory;
	dwPhysicalMemory=ms.ullTotalPhys;

	char szDesc[512]="";
	sprintf(szDesc, "Windows = %d.%d Build %d , %s (%dKB) : ", os.dwMajorVersion, os.dwMinorVersion,
		os.dwBuildNumber, os.szCSDVersion, (int)(dwPhysicalMemory/1024));
	mlog(szDesc);

	if (os.dwMajorVersion == 10) {
		if (os.dwMinorVersion == 0) sprintf(szDesc, " Windows 10..\n");
	}

	else if (os.dwMajorVersion == 6){
		if (os.dwMinorVersion == 0) sprintf(szDesc, " Windows Vista..\n");
		else if (os.dwMinorVersion == 1) sprintf(szDesc, " Windows 7..\n");
		else if (os.dwMinorVersion == 2) sprintf(szDesc, " Windows 8..\n");
		else if (os.dwMinorVersion == 3) sprintf(szDesc, " Windows 8.1..\n");
	}

	else if(os.dwMajorVersion==5) {
		if(os.dwMinorVersion==0)		sprintf(szDesc," Windows 2000..\n");
		else if(os.dwMinorVersion==1)	sprintf(szDesc," Windows xp..\n");
		else if(os.dwMinorVersion==2)	sprintf(szDesc," Windows 2003..\n");
		else							sprintf(szDesc," ..\n");
	}
	else if(os.dwMajorVersion==4) {
		if(os.dwMinorVersion==0)		sprintf(szDesc," Windows 95..\n");
		else if(os.dwMinorVersion==10)	sprintf(szDesc," Windows 98..\n");
		else if(os.dwMinorVersion==90)	sprintf(szDesc," Windows Me..\n");
		else							sprintf(szDesc," ..\n");
	}
	else if(os.dwMajorVersion==3) {
		if(os.dwMinorVersion==51)		sprintf(szDesc," Windows NT 3.51..\n");
		else							sprintf(szDesc," ..\n");
	}
	else	{
		sprintf(szDesc," ..\n");
	}
	mlog(szDesc);
}

void MSysInfoLog()
{
	MSysInfoLog_OS();
	MSysInfoLog_CPU();
	MSysInfoLog_Display();
//	MSysInfoLog_Audio();
}
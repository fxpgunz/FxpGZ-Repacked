//-----------------------------------------------------------------------------
// Copyright 2017 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <tchar.h>
#include <fstream>

// Note: this header has no dependencies on 3ds Max so that it may be included in
// external projects without establishing a dependency on 3ds Max

namespace MaxSDK
{
	namespace Util
	{
		/*!
		\brief When a 3ds Max session starts up, immediately after creating the main HWND a named file mapping
		object is created that contains the data associated with an instance of struct ProcessData3dsMax.
		This functionality was added in 3ds Max 2018 as the class of the main HWND is now a Qt-based class 
		rather than "3DSMAX". This new mechanism also allows the version of 3ds Max to be determined and is
		extendable in the future.
		*/

		//-----------------------------------------------------------------------------
		// struct ProcessData3dsMax
		//-----------------------------------------------------------------------------
		/** \brief The structure definition of the data stored to the named file mapping object.
		*/
		struct ProcessData3dsMax
		{
			enum version 
			{
				version1, // mSize, mMaxVersion, and mMaxHwnd members
			};
			/* \brief The size of the ProcessData3dsMax instance. 
			For future use in exposing a <bool>ProcessData3dsMax(DWORD processId, ProcessData3dsMax& processData3dsMax) function. 
			*/
			DWORD mSize = sizeof(ProcessData3dsMax);
			/* \brief The version number of the instance. For version 1, additional members are mMaxVersion and mMaxHwnd. */
			DWORD mProcessDataVersion = 0;
			/* \brief The 3ds Max release version number (MAX_RELEASE). */
			DWORD mMaxVersion = 0;
			/* \brief The main HWND of 3ds Max session. */
			DWORD mMaxHwnd = 0;
		};

		/** \brief The root name of the named file mapping object that contains the ProcessData3dsMax data.
		The process id value is appended to this string to form the full name.
		*/
		static const TCHAR* sProcessData3dsMaxFileMappingObjectNameRoot = _T("Local\\3dsmax_process_data_");

		/** \brief The root name of the mutex for accessing the named file mapping object that contains the ProcessData3dsMax data.
		The process id value is appended to this string to form the full name.
		*/
		static const TCHAR* sProcessData3dsMaxFileMappingObjectNameMutexRoot = _T("Local\\3dsmax_process_data_mutex");

		/** \brief Utility function to get the main 3ds Max window handle for the given process handle. 
		\param processId [in] The 3ds Max Process id to get data for.
		\param maxVersion [in, out] The 3ds Max version of the process.
		\return The main 3ds Max window handle for the given process handle if found, NULL otherwise
		*/
		inline HWND FindProcessMainWindow(DWORD processId, DWORD &maxVersion)
		{
			HWND max_hwnd = NULL;
			wchar_t pidAsString[40];
			wsprintf(pidAsString, L"%lu", processId);
			std::wstring sharedMemName = std::wstring(sProcessData3dsMaxFileMappingObjectNameRoot) + pidAsString;

			HANDLE hSharedMem = OpenFileMapping(FILE_MAP_READ, 0, sharedMemName.c_str());
			if (hSharedMem)
			{
				// The shared memory needs to be guarded by OS primitive
				std::wstring sharedMemMutexName = std::wstring(sProcessData3dsMaxFileMappingObjectNameMutexRoot) + pidAsString;
				HANDLE hBufferAccessGuard = OpenMutex(MUTEX_ALL_ACCESS, FALSE, sharedMemMutexName.c_str());
				if (hBufferAccessGuard == NULL) {
					hBufferAccessGuard = CreateMutex(NULL, FALSE, sharedMemMutexName.c_str());
				}
				WaitForSingleObject(hBufferAccessGuard, INFINITE);

				ProcessData3dsMax* pProcessData3dsMax = (ProcessData3dsMax*)MapViewOfFile(hSharedMem, FILE_MAP_READ, 0, 0, 0);
				if (pProcessData3dsMax)
				{
					max_hwnd = (HWND)(UINT_PTR)pProcessData3dsMax->mMaxHwnd;
					maxVersion = pProcessData3dsMax->mMaxVersion;
					UnmapViewOfFile(pProcessData3dsMax);
				}
				CloseHandle(hSharedMem);
				hSharedMem = NULL;

				ReleaseMutex(hBufferAccessGuard);
				CloseHandle(hBufferAccessGuard);
				hBufferAccessGuard = NULL;
			}
			return max_hwnd;
		}
	}
}

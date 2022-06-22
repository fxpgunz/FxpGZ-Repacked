//////////////////////////////////////////////////////////////////////////////
//
//  Presence of this DLL (detoured.dll) marks a process as detoured.
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
#include "stdafx.h"
#include <windows.h>
#include "detoured.h"

static HMODULE s_hDll;

HMODULE WINAPI Detoured()
{
    return s_hDll;
}

//
///////////////////////////////////////////////////////////////// End of File.

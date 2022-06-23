/**********************************************************************
// 	FILE: PopulateExport.h
// 
// 	DESCRIPTION:  Dll Export for Populate
//
// 	AUTHOR: Susan Amkraut  
// 
//	HISTORY: project initiated in Nov. 2006
//
// Copyright 2010 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
**********************************************************************/

#pragma once

#ifdef BLD_POPULATE
#define POPULATE_EXPORT __declspec( dllexport )
#else
#define POPULATE_EXPORT __declspec( dllimport )
#endif

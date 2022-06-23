#pragma once

//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
[!if SIMPLE_TYPE != 0]
[!if PROCEDURAL_OBJECT_TYPE != 0]
#include <Simpobj.h>
#include <mouseman.h>
[!endif]
[!if MODIFIER_TYPE != 0]
#include <Simpmod.h>
#include <Simpobj.h>
[!endif]
[!if PARTICLE_TYPE != 0]
#include <Simpobj.h>
[!endif]
[!endif]//SIMPLE TYPE

[!if PROCEDURAL_OBJECT_TYPE != 0]
#include <mouseman.h>
#include <triobj.h>
[!endif]

[!if SPACE_WARP_TYPE != 0]
#include <simpmod.h>
#include <simpobj.h>
[!endif]
[!if COLPICK_TYPE != 0]
#include <hsv.h>
[!endif]
[!if FILE_EXPORT_TYPE != 0]
#include <impexp.h>
[!endif]
[!if FILE_EXPORT_3DXI_TYPE != 0]
#include <impexp.h>
[!endif]
[!if FILE_IMPORT_TYPE != 0]
#include <impexp.h>
#include <direct.h>
#include <commdlg.h>
[!endif]
[!if GUP_TYPE != 0]
#include <guplib.h>
[!endif]
[!if IMAGE_FILTER_COMPOSITOR_TYPE != 0]
#include <tvnode.h>
#include <bmmlib.h>
#include <fltlib.h>
[!endif]
[!if IMAGE_VIEWER_TYPE != 0]
#include <ViewFile.h>
[!endif]
[!if MODIFIER_TYPE != 0]
#include <meshadj.h>
#include <XTCObject.h>
[!endif]
[!if SAMPLER_TYPE != 0]
#include <samplers.h>
[!endif]
[!if SHADER_TYPE != 0]
#include <texutil.h>
#include <shaders.h>
#include <macrorec.h>
#include <gport.h>
[!endif]
[!if SHADOW_TYPE != 0]
#include <shadgen.h>
[!endif]
[!if TEX_TYPE != 0]
#include <stdmat.h>
#include <imtl.h>
#include <macrorec.h>
[!endif]
[!if TRACK_VIEW_UTILITY_TYPE != 0]
#include <tvutil.h>
[!endif]
[!if UTILITY_TYPE != 0]
#include <utilapi.h>
[!endif]
[!if IK_TYPE != 0]
#include <IKSolver.h>
[!endif]
[!if SKIN_GIZMO_TYPE != 0]
#include <ISkin.h>
#include <ISkinCodes.h>
#include <icurvctl.h>
[!endif]
[!if MANIP_TYPE != 0]
#include <Manipulator.h>
#include <mouseman.h>
[!endif]
[!if ATMOSPHERIC_TYPE != 0]
#include <gizmo.h>
#include <gizmoimp.h>
[!endif]
[!if EXTENSION != 0]
#include <XTCObject.h>
[!endif]


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

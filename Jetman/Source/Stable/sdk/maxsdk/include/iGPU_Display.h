
/**********************************************************************
 *<
	FILE: iOSD_GPU_Display.h

	DESCRIPTION:	Classes to handle GPU display

	CREATED BY:

	HISTORY:

	*>	Copyright (c) 2014, All Rights Reserved.
	**********************************************************************/

#pragma once;

#include "baseinterface.h"
#include "ref.h"
#include <Graphics/IRefObject.h>
#include <Graphics/ICustomRenderItem.h>
#include <Graphics/UpdateDisplayContext.h>
#include <Graphics/UpdateNodeContext.h>

/* Interface class manages the direct gpu display for D3D 11 */
class IGPUDisplayRenderItem : public MaxSDK::Graphics::ICustomRenderItem
{
public:
	IGPUDisplayRenderItem() {};

	/* returns if the d3d display is enabled, this can from the user toggling the display off or the video card not supporting d3d11*/
	virtual bool IsEnabled() = 0;
	/* returns if the mesh caches are invalid and the display buffers need to be rebuilt*/
	virtual bool IsInvalid() = 0;

	// For now use this temporary method to dispose display resource before Nitrous is released. 
	virtual void DisposeDisplayResource() = 0;
};


/* this is the container class around mnmesh that holds both teh display call back and
the actual display pointer.  This is attached as an interface to the mnmesh and then
the PolyObject examines it to determine whether to use the GPU display callback or normal 
gfx path to display the mnmesh*/
#define IMNMESH_GPU_DISPLAY_INTERFACE Interface_ID(0xDE20034f, 0x9C3F511)
class iMNMESH_GPU_Display : public BaseInterface
{
public:
	DllExport iMNMESH_GPU_Display();
	virtual ~iMNMESH_GPU_Display();

	// from BaseInterface
	DllExport virtual BaseInterface* GetInterface(Interface_ID id);
	DllExport virtual Interface_ID GetID();
	DllExport virtual void DeleteInterface();

	// This function will be only called in the destructor of owner mesh to make 
	// sure this interface don't maintain a dangling pointer.
	DllExport void DisposeInterface();

	MaxSDK::Graphics::RefPtr<IGPUDisplayRenderItem> mpCustomRenderItem;	
};


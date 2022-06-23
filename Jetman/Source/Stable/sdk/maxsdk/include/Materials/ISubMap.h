//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../MaxHeap.h"
#include "../CoreExport.h"
#include "../strclass.h"

#pragma warning(push)
#pragma warning(disable:4100)

class ParamDlg;
class Texmap;
class ReferenceTarget;

/*! \defgroup mapSlotTypes Map Slot Types
\sa Class MtlBase */
//@{
#define MAPSLOT_TEXTURE      0   //!< A slot that holds a texture map.
/*! Generate UVW on-the-fly using view vector, default to spherical */
#define MAPSLOT_ENVIRON      1   //!< A slot which holds an environment map.
#define MAPSLOT_DISPLACEMENT 2   //!< Displacement maps: a type of texture map.
#define MAPSLOT_BACKGROUND   3   //!< Background maps: generate UVW on-the-fly using view vector, default to screen
//@}

/*! \sa  Class MtlBase, Class Texmap, Class ParamDlg, Class ReferenceTarget.\n\n
\par Description:
The methods of this class provide access to the sub-textures of a MtlBase.
These properties include the number of sub-maps, the slot type, on / off state,
etc.\n\n
The MtlBase class sub-classes from this class. If a developer is creating a
plug-in derived from MtlBase (for instance a Material or Texture) then
implementations of these methods are required. Developers may call these
methods on an existing MtlBase sub-class.  */
class ISubMap : public MaxHeapOperators 
{
public:
	/*! \remarks Destructor. */
	virtual ~ISubMap() {;}
	// Methods to access sub texture maps of material or texmap
	/*! \remarks Returns the number of sub-textures managed by this material or texture.
	\par Default Implementation:
	<b>{ return 0; }</b> */
	virtual int NumSubTexmaps() { return 0; }
	/*! \remarks Returns a pointer to the 'i-th' sub-texmap managed by the material or
	texture.\n\n
	Note: For the 3ds Max Standard material, the sub-texmap indexes used with this
	method are listed in \ref Material_TextureMap_IDs.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the texmap to return.
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual Texmap* GetSubTexmap(int i) { return NULL; }
	/*! \remarks In the Coordinates rollup in the user interface for a texture map are two
	options. These options are Texture or Environment. The slot type is one of
	these two options, texture coordinates or environment coordinates. There are
	a variety of texture coordinate types. There are the type assigned to the
	object and the environment type (Spherical, Cylindrical, Shrink-wrap,
	Screen). This method is used to determine the type required by the
	particular sub-texture. This is either texture coordinates
	(<b>MAPSLOT_TEXTURE</b>) or environment coordinates
	(<b>MAPSLOT_ENVIRON</b>).
	\par Parameters:
	<b>int i</b>\n\n
	The index of the sub-texture whose slot type to return.
	\return See \ref mapSlotTypes.
	\par Default Implementation:
	<b>{ return MAPSLOT_TEXTURE; }</b> */
	virtual int MapSlotType(int i)=0;
	/*! \remarks Stores the 'i-th' sub-texmap managed by the material or texture.\n\n
	Note: For the 3ds Max Standard material, the sub-texmap indexes used with this
	method are listed in \ref Material_TextureMap_IDs.
	\par Parameters:
	<b>int i</b>\n\n
	The index of the storage for the texmap.\n\n
	<b>Texmap *m</b>\n\n
	The texmap to store.
	\par Default Implementation:
	<b>{}</b> */
	virtual void SetSubTexmap(int i, Texmap *m) { }

	// query ISubMap about the On/Off state of each sub map.
	/*! \remarks Returns nonzero if the specified sub-texmap is on; otherwise zero. Some
	materials may have user interface controls to toggle the sub-maps on and
	off. The Standard material has such controls for example.
	\par Parameters:
	<b>int i</b>\n\n
	The index of the sub-texmap to check.
	\par Default Implementation:
	<b>{ return 1; }</b> */
	virtual int SubTexmapOn(int i) { return 1; } 

	/*! \remarks This method returns the slot name of the 'i-th' sub-texmap. This name
	appears in the materials editor dialog. For instance, if you are in a
	material and then you go down into a map, this is the name that appears just
	below the 'Get Material' icon. For example, in the Standard material when
	you choose one of the maps, the map name appears to let you know which slot
	you are working on. For the Standard material, this may be "Ambient",
	"Diffuse", "Specular", etc.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the slot whose name is returned.
	\par Default Implementation:
	The default implementation returns an empty ("") string. */
	CoreExport virtual MSTR GetSubTexmapSlotName(int i);
	/*! \remarks Returns the name to appear in Track %View of the 'i-th' sub-texmap.
	\par Parameters:
	<b>int i</b>\n\n
	Specifies the sub-texmap whose name is returned. */
	CoreExport MSTR GetSubTexmapTVName(int i);

	// called by the ParamMap2 AUTO_UI system if the material/map is letting
	// the system build an AutoMParamDlg for it.  SetDlgThing() is called on
	// a material/map coming into an existing set of ParamDlgs, once for each 
	// secondary ParamDlg and it should set the appropriate 'thing' into the
	// given dlg (for example, Texout* or UVGen*).  Return FALSE if dlg is unrecognized.
	// See Gradient::SetDlgThing() for an example.
	/*! \remarks This method is called by the ParamMap2 <b>AUTO_UI</b> system
	if the material/texmap is letting the system build an <b>AutoMParamDlg</b>
	for it. This method is called on a material/texmap coming into an existing
	set of ParamDlgs, once for each secondary <b>ParamDlg</b> and it should set
	the appropriate 'thing' into the given dlg (the 'thing' being, for example,
	a <b>Texout*</b> or <b>UVGen*</b>). Return FALSE if <b>dlg</b> is
	unrecognized.\n\n
	Note: See the discussion above in <b>CreateParamDlg()</b> for additional
	details on this method.
	\par Parameters:
	<b>ParamDlg* dlg</b>\n\n
	Points to the ParamDlg to check. See Class ParamDlg.
	\return  TRUE if the <b>dlg</b> passed is recognized; FALSE if unrecognized.
	\par Default Implementation:
	<b>{ return FALSE; }</b>
	\par Sample Code:
	\code
	BOOL Gradient::SetDlgThing(ParamDlg* dlg)
	{
	if (dlg == uvGenDlg)
	uvGenDlg->SetThing(uvGen);
	else if (dlg == texoutDlg)
	texoutDlg->SetThing(texout);
	else
	return FALSE;
	return TRUE;
	}
	\endcode */
	virtual BOOL SetDlgThing(ParamDlg* dlg) { return FALSE; }

	// use this for drag-and-drop of texmaps
	/*! \remarks Implemented by the System.\n\n
	This method is used to handle the drag-and-drop of texmaps. A developer
	implements the logic to handle the drag and drop. Once they have the
	information about what slot was dropped on what other slot, this method may
	be called to handle the copying. This is used by the 3ds Max Standard
	material.
	\par Parameters:
	<b>HWND hwnd</b>\n\n
	The rollup page window handle.\n\n
	<b>int ifrom</b>\n\n
	The source texmap.\n\n
	<b>int ito</b>\n\n
	The destination texmap. */
	CoreExport void CopySubTexmap(HWND hwnd, int ifrom, int ito);

	/*! \remarks Implemented by the System.\n\n
	The implementation of this method is provided by <b>MtlBase</b>. It returns
	its <b>this</b> pointer. */
	virtual ReferenceTarget *GetRefTarget()=0;
};

#pragma warning(pop)

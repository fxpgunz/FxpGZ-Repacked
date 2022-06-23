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

#include "../custcont.h"

#pragma warning(push)
#pragma warning(disable:4100)

class ParamDlg;

//--------------------------------------------------------------------------------
// Use this class to implement Drag-and-Drop for materials sub-Texmaps.
// If this class is used the ParamDlg method FindSubTexFromHWND() must be implemented.
//--------------------------------------------------------------------------------
/*! \sa  Class DADMgr, Class ParamDlg.\n\n
\par Description:
Use this class to provide drag and drop functionality for materials
sub-Texmaps. It provides implementations of the methods of <b>DADMgr</b>. If
this class is used the <b>ParamDlg</b> method <b>FindSubTexFromHWND()</b> must
be implemented. */
class TexDADMgr: public DADMgr {
    ParamDlg *dlg;
public:
    TexDADMgr(ParamDlg *d=NULL) { dlg = d;}
    void Init(ParamDlg *d) { dlg = d; }
    // called on the draggee to see what if anything can be dragged from this x,y
    SClass_ID GetDragType(HWND hwnd, POINT p) { return TEXMAP_CLASS_ID; }
    // called on potential dropee to see if can drop type at this x,y
    CoreExport BOOL OkToDrop(ReferenceTarget *dropThis, HWND hfrom, HWND hto, POINT p, SClass_ID type, BOOL isNew);
    int SlotOwner() { return OWNER_MTL_TEX;   }
    CoreExport ReferenceTarget *GetInstance(HWND hwnd, POINT p, SClass_ID type);
    CoreExport void Drop(ReferenceTarget *dropThis, HWND hwnd, POINT p, SClass_ID type, DADMgr* srcMgr = NULL, BOOL bSrcClone = FALSE);
    CoreExport BOOL  LetMeHandleLocalDAD(); 
    CoreExport void  LocalDragAndDrop(HWND h1, HWND h2, POINT p1, POINT p2);
    BOOL  AutoTooltip(){ return TRUE; }
};

#pragma warning(pop)

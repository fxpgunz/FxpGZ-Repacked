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

/*!
An instance of this class is created by Renderer::CreateParamDlg().
Since this dialog is modeless and non-interactive, as the user changes
parameters in the dialog, the renderer does not need to update its state. When
the user is through, they may choose 'OK' or 'Cancel' from the dialog. If the
user selects OK then the AcceptParams() method will be called. If the
user selects Cancel, then the RejectParams() method is called.
\see  Class Renderer.
*/
class RendParamDlg : public MaxHeapOperators {
public:
    /*! Destructor. */
    virtual ~RendParamDlg() { }
    /*! If the user selects OK from the dialog, this method will be called, at
    which time the renderer can read the parameters out of the UI and
    modify its state. */
    virtual void AcceptParams() = 0;
    /*! If this method is called, typically the renderer will not have to do
    anything since it has not yet modified its state, but if for some
    reason it has, it should restore its state.
    \par Default Implementation:
    <b>{}</b> */
    virtual void RejectParams() {}
    /*! Deletes this instance of the class. The instance is allocated by
    Renderer::CreateParamDialog(). */
    virtual void DeleteThis() = 0;
};

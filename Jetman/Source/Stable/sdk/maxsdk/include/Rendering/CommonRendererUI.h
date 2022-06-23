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

#include "../NameTab.h"

#ifdef COMMON_RENDER_UI_MODULE
    #define CommonRendererUIExport __declspec(dllexport)
#else
    #define CommonRendererUIExport __declspec(dllimport)
#endif

//! This namespace aggregates various UI functionality to be shared by renderers.
namespace CommonRendererUI
{
    /*! \brief Shows the missing maps dialog.
        The missing maps dialog displays a list of provided file paths for missing maps. It is up to the caller to accumulate that list.
        The dialog provides the user with the option to browse for the missing maps, adding to the list of search paths. Thus, if the user
        should press <i>Continue</i>, the renderer should attempt to re-resolve the missing map files before moving on to rendering.
        \remark This method internally handles logging, error reporting, network rendering, etc.
        \param names The list of file paths to missing maps.
        \param hwnd Handle to the parent window of the pop-up to be displayed.
        \return True if rendering should continue normally - either because the user pressed <i>Continue</i>, or because UI pop-ups are disabled
            (e.g. for network rendering) and the system is set to continue on errors. False if rendering should be cancelled, either because the user
            pressed <i>Cancel</i> or because UI pop-ups are disabled (e.g. for network rendering) and the system is set to abort on errors. */
	CommonRendererUIExport bool DoMissingMapsDialog(const NameTab& names, HWND hwnd);
    /*! \brief Shows the missing UVW channels dialog.
        The missing UVW channels dialog informs the user of a list of objects and UVW channels that are required by their materials, but missing
        from the geometry. This dialog is purely informative and allows the user to continue or cancel.
        \remark This method internally handles logging, error reporting, network rendering, etc.
        \param missingUVWEntries The list of strings to display in the missing UVW channels dialog. The standard format is as follows: 
            <b>"(UVW <channel_id>): <object_name>"</b> or <b>"(Vertex Colors): <object_name>"</b>.
        \param hwnd Handle to the parent window of the pop-up to be displayed.
        \return True if rendering should continue normally - either because the user pressed <i>Continue</i>, or because UI pop-ups are disabled
            (e.g. for network rendering) and the system is set to continue on errors. False if rendering should be cancelled, either because the user
            pressed <i>Cancel</i> or because UI pop-ups are disabled (e.g. for network rendering) and the system is set to abort on errors. */
	CommonRendererUIExport bool DoMissingUVWChannelsDialog(const NameTab& missingUVWEntries, HWND hwnd);
}

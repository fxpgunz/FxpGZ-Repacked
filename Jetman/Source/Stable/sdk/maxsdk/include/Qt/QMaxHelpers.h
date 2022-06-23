//-----------------------------------------------------------------------------
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------

#pragma once

#include "../CoreExport.h"
#include "QtMax.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800 )
#include <QtWidgets/QtWidgets>
#include <QtCore/QObject>
#pragma warning(pop)

class Bitmap;


namespace QtHelpers
{

   ////////////////////////////////////////////////////////////////////////////
   // Image functions.

	//Takes the name of the icon to load directory/iconName.png. It loads all versions of the 
   //icon, example: directory/iconName_XX.png, directory/iconName_on_XX.png where XX is the size
   //of the icon.
   //Input Parameters: iconName - The name of the icon to load.
   //OutputParameters: numIcons- the number of elements in the returned array.
   //Return Value    : An array of HIcon containing the HIcons of the loaded icons. Must be destroyed 
   //                  by the caller function.
   //Note: The returned HICON array must be destroyed by the user. Also, the user has to call DestroyIcon
   //      to destroy the icon when not used anymore.
   MaxQTExport HICON* CreateHIconsFromMultiResIcon(const MCHAR* iconName, int& numIcons);

   // Convert a 3ds Max SDK bitmap to a Qt icon.
   MaxQTExport QIcon QIconFromBitmap( Bitmap* bm );

   // Convert a Qt icon to a 3ds Max SDK bitmap.
   MaxQTExport Bitmap* BitmapFromQIcon( const QIcon& icon, int width, int height, QIcon::Mode mode = QIcon::Mode::Normal, QIcon::State state = QIcon::State::Off );

   // Convert a Qt icon to a HBITMAP.
   MaxQTExport HBITMAP HBITMAPFromQIcon( const QIcon& icon, int width, int height, QIcon::Mode mode = QIcon::Mode::Normal, QIcon::State state = QIcon::State::Off );

   /* \brief Associates a help context identifier with a given widget.
   * Equivalent to the windows method SetWindowContextHelpId().
   * Use this function for registering a help context id on a qwidget without 
   * making it native win32 window.
   * \param widget The widget with which the help context id should be associated.
   * \param contextId The Help context identifier.
   */
   MaxQTExport void SetWindowContextHelpId( QWidget* widget, int contextId );


}; // end of QtHelpers namespace -------------------------------------------------

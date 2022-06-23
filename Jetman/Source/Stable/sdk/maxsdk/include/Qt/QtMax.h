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

#include "../strclass.h"
#include "../acolor.h"
#include "../point2.h"
#include "../matrix3.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800 )
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#pragma warning(pop)

#ifdef BLD_MAXQTBRIDGE
#define MaxQTExport __declspec( dllexport )
#else
#define MaxQTExport __declspec( dllimport )
#endif

/*! \file
 * \warning This API is unsupported and provided for experiment only.  It should not be used for production.
 * This API will change or disappear in a future release.
 */
namespace MaxSDK
{;

// Enable 3ds Max data types to be used in a QVariant
Q_DECLARE_METATYPE( AColor );
Q_DECLARE_METATYPE( Point2 );
Q_DECLARE_METATYPE( Point3 );
Q_DECLARE_METATYPE( Point4 );
Q_DECLARE_METATYPE( Matrix3 );

// Retrieve localized text from its resource ID.
QString QStringFromID(int resourceID);

}     // namespace

#include "QtMax.inline.h"
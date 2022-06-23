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

#include "../sfx.h"

/*! This is container, anchor point for a set of atmospherics. It's used by the system in RendParams::atmos, enabling the attachment
     of multiple atmospherics where a single Atmospheric* exists. A pointer to this class may be obtained through a dynamic cast from an
     Atmospheric*, most notably RendParams::atmos. */
class IAtmosphericContainer : public Atmospheric
{
public:
    //! Returns the number of atmospherics present in the container.
    virtual int NumAtmospheric() = 0;
    /*! Returns the i'th atmospheric from the container.
        \param i The index of the atmospheric to be returned. */
    virtual Atmospheric* GetAtmospheric(int i) = 0;
};


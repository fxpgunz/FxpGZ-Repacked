//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2014 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once 

// Max SDK
#include <Materials/Texmap.h>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class IEnvironmentContainer
//
/*! Aggregates the properties related to environment and background mapping.
    
    This container aggreates the properties of the environment and background maps associated with a scene. The exact source from which
    these are collected is abstracted, thus conveniently hiding any future evolution in 3ds Max's support for environment, background, and backplate
    mapping.
*/
class IEnvironmentContainer : public Texmap
{
public:

    //! The modes in which the background/backplate may operate.
    enum class BackgroundMode
    {
        Texture = 0,        //!< Use background texture (ignore the color)
        Color = 1,          //!< Use background color (ignore the texture)
        Environment = 2     //!< Use environment as background (ignoring background color and texture)
    };
    /*! Returns the mode in which the background should operate. */
    virtual BackgroundMode GetBackgroundMode() const = 0;
    /*! Returns the background texture, to be used if and only if the background mode is set to BackgroundMode::Texture. */
    virtual Texmap* GetBackgroundTexture() const = 0;
    /*! Returns the background color, to be used if and only if the background mode is set to BackgroundMode::Color.
        \param t The time value at which to evaluate the color, which may be animated.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual AColor GetBackgroundColor(const TimeValue t, Interval& validity) const = 0;

    //! The modes in which the environment may operate.
    enum class EnvironmentMode
    {
        Texture = 0,        //!< Use environment texture (ignore the color)
        Color = 1,          //!< Use environment color (ignore the texture)
        None = 2            //!< No environment (black)
    };
    /*! Returns the mode in which the environment should operate. */
    virtual EnvironmentMode GetEnvironmentMode() const = 0;
    /*! Returns the environment texture, to be used if and only if the environment mode is set to EnvironmentMode::Texture. */
    virtual Texmap* GetEnvironmentTexture() const = 0;
    /*! Returns the environment color, to be used if and only if the environment mode is set to EnvironmentMode::Color.
        \param t The time value at which to evaluate the color, which may be animated.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual AColor GetEnvironmentColor(const TimeValue t, Interval& validity) const = 0;
};

}}	// namespace

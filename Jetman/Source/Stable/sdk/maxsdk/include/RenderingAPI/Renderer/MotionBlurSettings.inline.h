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

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// struct MotionBlurSettings
//==================================================================================================

inline MotionBlurSettings::MotionBlurSettings() 
    : shutter_offset(0),
    shutter_duration(0) 
{

}

inline MotionBlurSettings::MotionBlurSettings(const TimeValue duration, const TimeValue offset) 
    : shutter_offset(offset), 
    shutter_duration(duration) 
{

}

inline bool MotionBlurSettings::IsMotionBlurEnabled() const
{
    return (shutter_duration != 0);
}

inline bool MotionBlurSettings::operator==(const MotionBlurSettings& other) const
{
    return (shutter_duration == other.shutter_duration)
        && (shutter_offset == other.shutter_offset);
}

inline bool MotionBlurSettings::operator!=(const MotionBlurSettings& other) const
{
    return !(*this == other);
}

//==================================================================================================
// struct MotionTransforms
//==================================================================================================

inline MotionTransforms::MotionTransforms()
    // initialize to identify
    : shutter_open(true),
    shutter_close(true)
{
}

inline bool MotionTransforms::operator==(const MotionTransforms& other) const 
{ 
    return (shutter_open == other.shutter_open) 
        && (shutter_close == other.shutter_close) 
        && (shutter_settings == other.shutter_settings); 
}

inline bool MotionTransforms::operator!=(const MotionTransforms& other) const
{
    return !(*this == other);
}

}}	// namespace 

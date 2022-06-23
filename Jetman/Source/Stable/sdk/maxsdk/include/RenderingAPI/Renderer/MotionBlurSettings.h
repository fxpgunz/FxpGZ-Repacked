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

#include <maxtypes.h>
#include <matrix3.h>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class MotionBlurSettings
//
/*! Encapsulates the camera shutter settings used with motion blur.
    \see ICameraContainer::GetGlobalMotionBlurSettings()
    \see IRenderSessionContext::GetMotionBlurSettingsForNode()
*/
struct MotionBlurSettings
{
    //! The shutter duration, 0 if and only if motion blur is disabled.
    TimeValue shutter_duration;
    /*! The offset, to be applied to the time value for the current frame, to calculate the shutter open time.
        \remark shutter_open_time = current_frame_time + shutter_offset
        \remark shutter_close_time = currrent_frame_time + shutter_offset + shutter_duration */
    TimeValue shutter_offset;

    //! Initializes the both shutter_duration and shutter_offset to 0.
    MotionBlurSettings();
    //! Initializes the shutter duration and offset with the given values.
    MotionBlurSettings(const TimeValue duration, const TimeValue offset);

    //! Returns whether motion blur is enabled, e.e. whether shutter_duration is not 0.
    bool IsMotionBlurEnabled() const;

    bool operator==(const MotionBlurSettings& other) const;
    bool operator!=(const MotionBlurSettings& other) const;
};

//==================================================================================================
// class MotionTransforms
//
/*! Encapsulates motion transforms for an object.
*/
struct MotionTransforms
{
    //! The transform at shutter open time (t + offset)
    Matrix3 shutter_open;
    //! The transform at shutter close time (t + offset + duration)
    Matrix3 shutter_close;
    //! The settings used to evaluate the transforms
    MotionBlurSettings shutter_settings;

    // Initializes the transforms to identify matrices.
    MotionTransforms();

    bool operator==(const MotionTransforms& other) const;
    bool operator!=(const MotionTransforms& other) const;
};

}}	// namespace 

#include "MotionBlurSettings.inline.h"

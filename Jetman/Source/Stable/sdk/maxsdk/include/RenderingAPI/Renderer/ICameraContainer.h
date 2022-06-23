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

#include "MotionBlurSettings.h"
#include <ipoint2.h>
#include <box2.h>

class INode;
class ViewParams;
class Bitmap;
class View;

namespace MaxSDK
{;
class IPhysicalCamera;

namespace RenderingAPI
{;

//==================================================================================================
// class ICameraContainer
//
/*! Abstracts the common properties of the camera to be rendered.
    
    This container wraps and abstracts the camera to be rendered, whether it be a Standard, Physical, or viewport camera.
    The logic for interpreting a camera INode, ViewExp, and ViewParams are conveniently hidden away by this class.
    Additionally, This class provides a unified mechanism for notifying of changes in the camera properties. 
*/
class ICameraContainer
{
public:

    enum class ProjectionType;
    class IChangeNotifier;

    //! \name Camera Node/Object
    //@{
    //! Returns the camera node being used. Will return null if a camera-less viewport is being rendered.
    virtual INode* GetCameraNode() const = 0;
    /*! Returns the physical camera being used, if one is being used at all - or null otherwise.
        The physical camera exposes additional, advanced parameters which are not generic enough to be exposed directly by ICameraContainer.
        \param t The time value at which the camera node is evaluated to extract the physical camera. */
    virtual IPhysicalCamera* GetPhysicalCamera(const TimeValue t) const = 0;
    //@}

    //! \name Camera Projection
    //@{
    /*! Evaluate the motion transform for the camera.
        \param t The time at which to evaluate the transform.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. 
        \param camera_motion_blur_settings The optional motion blur settings to used when evaluating the transform. If null is passed, then 
            the camera's and node's motion blur settings are used, just as IRenderSessionContext::EvaluateMotionTransforms(). */
    virtual MotionTransforms EvaluateCameraTransform(
        const TimeValue t, 
        Interval& validity, 
        const MotionBlurSettings* camera_motion_blur_settings = nullptr) const = 0;
    /*! Returns the projection type used by the camera.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual ProjectionType GetProjectionType(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the field-of-view of the camera, in radians.
        \remark Only valid for perspective cameras.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual float GetPerspectiveFOVRadians(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the width of the orthographic aperture, in system units.
        \remark Only valid for orthographic cameras.
        \remark The ratio between the aperture width and height is image aspect ratio.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. 
        \see GetOrthographicApertureHeight() */
    virtual float GetOrthographicApertureWidth(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the height of the orthographic aperture, in system units.
        \remark Only valid for orthographic cameras.
        \remark The ratio between the aperture width and height is image aspect ratio.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. 
        \see GetOrthographicApertureWidth() */
    virtual float GetOrthographicApertureHeight(const TimeValue t, Interval& validity) const = 0;
    //@}
    
    //! \name Focus and Depth-of-Field
    //@{
    /*! Returns whether Depth-of-Field is enable on the camera.
        \remark Only valid for perspective cameras.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual bool GetDOFEnabled(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the focal length of the lends, in system units.
        \remark Only valid for perspective cameras.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual float GetLensFocalLength(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the distance to the focus plane, in system units. This is the distance at which the lens is focused, not to be confused with the lens
        focal length.
        \remark Only valid for perspective cameras.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual float GetFocusPlaneDistance(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the lens' aperture radius, in system units. This is the absolute radius, not the f-stop.
        \remark Only valid for perspective cameras.
        \remark Returns 0 if the camera does not specify the lens aperture size (not all cameras do).
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual float GetLensApertureRadius(const TimeValue t, Interval& validity) const = 0;
    //@}

    //! \name Clipping Planes
    //@{
    /*! Returns the clipping planes are enabled
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual bool GetClipEnabled(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the distance to the near clipping plane, in system units.
        \remark returns 0 if clipping planes are disabled.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual float GetClipNear(const TimeValue t, Interval& validity) const = 0;
    /*! Returns the distance to the far clipping plane, in system units.
        \remark returns std::numeric_limits<float>::max() if clipping planes are disabled.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual float GetClipFar(const TimeValue t, Interval& validity) const = 0;
    //@}

    //! \name Resolution, Region, and Bitmap
    //@{
    /*! Returns the resolution of the image to be rendered. 
        \remark This is identical to the resolution of the Bitmap returned by GetBitmap(). */
    virtual IPoint2 GetResolution() const = 0;
    /*! Returns the pixel aspect ratio of the image to be rendered. 
        \remark The image aspect ratio is: pixel_aspect_ratio * resolution.x / resolution.y */
    virtual float GetPixelAspectRatio() const = 0;
    /*! Returns the bitmap to which the image is to be rendered.
        \remark Progressive renderers should generally not render into this Bitmap directly, as this may cause artifacts when saving the image while it is
            being rendered. Instead, they should render to an internal frame buffer and using IRenderSessionContext::GetMainFrameBufferProcessor() to copy
            the buffer to the bitmap. */
    virtual Bitmap* GetBitmap() const = 0;
    /*! Returns the region to be rendered. 
        The renderer should only render pixels within this region; pixels outside the region should generally not not be overwritten in the Bitmap.
        \remark The region covers the entire image if region rendering is not being used; the region is thus guaranteed to be clipped to the current resolution and to be at least one pixel in size.
        \remark The box is relative to the upper-left corner of the image (i.e. upper-left is (0,0)).
        \remark The region is inclusive: the last row/column of the region must be rendered. */
    virtual Box2 GetRegion() const = 0;
    /*! Returns the X-Y offset of the canvas to be rendered, in the image planes, as number of pixels.
        The offset is expressed as floating-points as it may include fractions of a pixel.
        \remark The offset is relative to the upper-left corner of the image (i.e. upper-left is (0,0)).
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual Point2 GetImagePlaneOffset(const TimeValue t, Interval& validity) const = 0;
    //@}
    
    //! \name View Parameters
    //@{
    /*! Returns a View structure that may be passed GeomObject::GetRenderMesh().
        \remark The View that is returned does not implement View::CheckForRenderAbort(), as this method is not supported by the RenderingAPI in general.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual const View& GetView(const TimeValue t, Interval& validity) const = 0;
    /*! Returns a ViewParams structure, initialize with the camera's properties.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual ViewParams GetViewParams(const TimeValue t, Interval& validity) const = 0;
    //@}

    //! \name Motion Blur
    //@{
    /*! Returns the scene motion blur settings specified by the camera.
        \remark These are the settings to be used for the scene, not the settings for the camera's own motion blur.
        \param t The time value at which to evaluate the camera.
        \param validity [in, out] The validity of the returned value is intersected into the input value of this parameter. */
    virtual MotionBlurSettings GetGlobalMotionBlurSettings(const TimeValue t, Interval& validity) const = 0;
    //@}

    //! \name Notification
    //@{
    //! Registers a notifier callback, to be called when interactive changes occur.
    virtual void RegisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //! Unregisters a notification callback, previously registered with RegisterChangeNotifier().
    virtual void UnregisterChangeNotifier(IChangeNotifier& notifier) const = 0;
    //@}

protected:

    //! Protected destructor prevents deletion through this interface.
    virtual ~ICameraContainer() {}
};

//==================================================================================================
// enum class ICameraContainer::ProjectionType
//
/*! Camera projection types
*/
enum class ICameraContainer::ProjectionType
{
    Perspective,
    Orthographic
};

//==================================================================================================
// class ICameraContainer::IChangeNotifier
//
/*! Notification callback interface for ICameraContainer

    When registered with an ICameraContainer, this interface will be called whenever a relevant change occurs in the parameters of the camera. 
*/
class ICameraContainer::IChangeNotifier
{
public:
    //! Notifies that something has changed in the camera.
    virtual void NotifyCameraChanged() = 0;
protected:
    //! Protected destructor prevents deletion through this interface
    virtual ~IChangeNotifier() {}
};

}}	// namespace 


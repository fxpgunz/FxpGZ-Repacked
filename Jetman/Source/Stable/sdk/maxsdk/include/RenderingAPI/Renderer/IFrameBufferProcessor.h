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

// max sdk
#include <maxtypes.h>
#include <ipoint2.h>
// std
#include <vector>

class Box2;

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class IFrameBufferProcessor
//
/*! Generic interface for processing the contents of an frame buffer. 
    
    This interface abstracts functionality for processing the contents of a frame buffer - typically for storing a frame buffer into an output
    representation. The most common use case should be to transfer the contents of a renderer's internal frame buffer into a 3ds Max Bitmap,
    in a thread-safe and efficient manner.
*/
class IFrameBufferProcessor
{
public:

    class IFrameBufferReader;

    /*! Processes the given frame buffer.
        The exact behaviour of this method is defined by the particular implementation of this interface. For example, class IRenderSessionContext returns
        a frame buffer processor which transfers the data to the output Bitmap. But other implementations could, for example, store the result in 
        a render element Bitmap, or to an entirely different output device.
        \param process_tone_operator Whether the active ToneOperator should be applied to the contents of the frame buffer.
        \param t The time at which the ToneOperator will be evaluated.
        \param frame_buffer_access The interface, implemented by the caller, from which the frame buffer data is extracted. The implementation would typically
            fetch the pixels from the renderer's internal frame buffer. 
        \return True on success. False on error, generally if IFrameBufferReader::GetPixelLine() has returned false. */
    virtual bool ProcessFrameBuffer(const bool process_tone_operator, const TimeValue t, IFrameBufferReader& frame_buffer_access) = 0;

protected:

    //! Protected destructor prevents deletion through this interface.
    virtual ~IFrameBufferProcessor() {}
};

//==================================================================================================
// class IFrameBufferProcessor::IFrameBufferReader
//
/*! Generic interface for reading frame buffer data.
    
    This is the interface, used by IFrameBufferProcessor, for extracting the frame buffer data from the renderer's internal representation.
*/
class IFrameBufferProcessor::IFrameBufferReader
{
public:

    /*! Reads a subset of a single line of the frame buffer.
        \param y The index of the frame buffer line to be read.
        \param x_start The first pixel to be read, within the line.
        \param num_pixels The number of pixels to be read, guaranteed to be within the bounds of the line.
        \param target_pixels The buffer into which the pixels are to be written, allocated to store <i>num_pixels</i> pixels.
        \remark Pixels are to be written to <i>target_pixels</i> starting at index 0, without any offset. <i>x_start</i> is <b>not</b> to be used as an offset in
            the target buffer.
        \return True on success, false on failure to read to frame buffer for any reason. */
    virtual bool GetPixelLine(const unsigned int y, const unsigned int x_start, const unsigned int num_pixels, BMM_Color_fl* const target_pixels) = 0;

    /*! Returns the resolution of the frame buffer being abstracted by this class.
        This resolution bounds the range of valid pixel indices that may be accessed with GetPixelLine().
        \remark Internally, the caller is responsible for supporting arbitrary resolutions - and thus to resize the frame buffer if necessary. */
    virtual IPoint2 GetResolution() const = 0;
    /*! Returns the region, within the frame buffer, which can be read.
        This is the region which has been rendered, which contains meaningful value. The caller should limit its frame buffer access to the pixel
        indices within the region returned here. */
    virtual Box2 GetRegion() const = 0;

protected:

    //! Protected destructor prevents deletion through this interface
    virtual ~IFrameBufferReader() {}
};


}}	// namespace 
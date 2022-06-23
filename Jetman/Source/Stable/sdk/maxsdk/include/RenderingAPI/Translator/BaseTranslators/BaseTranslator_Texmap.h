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

#include "BaseTranslator_MtlBase.h"

#include <vector>

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class BaseTranslator_Texmap
//
/*! Base class for translating a Texmap.
    This base class provides basic, common functionality related to translating a Texmap. It handles:
    \li Monitoring the Texmap and invalidating the translator on any changes.
    \li Calling RenderBegin() on the Texmap before translation.
    \li Calling MtlBase::Update() on the Texmap before translation.
*/
class RenderingAPIExport BaseTranslator_Texmap : 
    public BaseTranslator_MtlBase
{
public:

    //! \param texmap The Texmap to be translated.
    //! \param graphNode The graph node to be passed to Translator::Translator().
    BaseTranslator_Texmap(Texmap* texmap, TranslatorGraphNode& graphNode);
    ~BaseTranslator_Texmap();

    /*! Bakes a Texmap into linear array of pixels.
        Returns false if failure to bake, for any reason.
        \param texmap The texmap to be baked.
        \param t The time at which to evaluate the Texmap.
        \param [in, out] validity The validity interval of the Texmap is intersected into this.
        \param default_resolution The resolution to bake the Texmap at, should the Texmap not define its own resolution.
        \param [out] baked_resolution The resolution at which the Texmap was baked.
        \param [out] pixel_data The baked Texmap pixels.
        \return True on success, false on failure - for any reason (e.g. missing texture file). */
    static bool BakeTexmap(
        Texmap& texmap, 
        const TimeValue t, 
        Interval& validity, 
        const IPoint2& default_resolution, 
        IPoint2& baked_resolution,
        std::vector<BMM_Color_fl>& pixel_data);

    /*! Extracts the pixels from a Bitmap, as 32-bit floating-point RGBA values.
        This method extracts the pixels from a given Bitmap, storing them into a linear array of pixels.
        \param bitmap The bitmap from which to extra the pixels.
        \param [out] resolution The resolution of the extracted pixel data.
        \param [out] pixel_data The pixels, extracted from the bitmap.
        \return True on success, false on error - for any reason (e.g. missing texture file). */
    static bool ExtractBitmap(
        Bitmap& bitmap,    
        IPoint2& resolution,        
        std::vector<BMM_Color_fl>& pixel_data);

    /*! Extracts the pixels from a Bitmap, as 16-bit integer RGBA values.
        This method extracts the pixels from a given Bitmap, storing them into a linear array of pixels.
        \param bitmap The bitmap from which to extra the pixels.
        \param [out] resolution The resolution of the extracted pixel data.
        \param [out] pixel_data The pixels, extracted from the bitmap.
        \return True on success, false on error - for any reason (e.g. missing texture file). */
    static bool ExtractBitmap(
        Bitmap& bitmap, 
        IPoint2& resolution,
        std::vector<BMM_Color_64>& pixel_data);

protected:

    //! Returns the Texmap being handled by this translator.
    Texmap* GetTexmap() const;

    // -- inherited INotificationCallback
    virtual void NotificationCallback_NotifyEvent(const IGenericEvent& genericEvent, void* userData) override; 

private:

    Texmap* const m_texmap;
};

}}	// namespace 

#pragma warning(pop)

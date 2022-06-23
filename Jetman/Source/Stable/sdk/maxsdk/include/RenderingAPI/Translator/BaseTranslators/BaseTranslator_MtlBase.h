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

#include "BaseTranslator_ReferenceTarget.h"

#pragma warning(push)
#pragma warning(disable: 4275 4251)

class MtlBase;

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class BaseTranslator_MtlBase
//
/*! Base class for translating a MtlBase.
    This base class provides basic, common functionality related to translating a MtlBase. It handles:
    \li Monitoring the MtlBase and invalidating the translator on any changes.
    \li Calling RenderBegin() on the MtlBase before translation.
    \li Calling MtlBase::Update() and Texmap::LoadMapFiles() on the MtlBase, and its entire refernce sub-hierarchy, before translation.
*/
class RenderingAPIExport BaseTranslator_MtlBase : 
    public BaseTranslator_ReferenceTarget
{
public:

    //! \param mtl The MtlBase to be translated.
    //! \param monitor_mtl_base Whether this class should monitor the MtlBase. Set to false if monitoring through the subclass, as something else (e.g. as Mtl*).
    //! \param graphNode The graph node to be passed to Translator::Translator().
    BaseTranslator_MtlBase(
        MtlBase* mtl, 
        const bool monitor_mtl_base,
        TranslatorGraphNode& graphNode);
    ~BaseTranslator_MtlBase();

    // -- inherited from Translator
    virtual void PreTranslate(const TimeValue translationTime, Interval& validity) override;
    virtual MSTR GetTimingCategory() const override;

    /*! Calls MtlBase::Update() and Texmap::LoadMapFiles() on the given material/texmap and its sub-tree of materials and textures.
        This generally needs to be called before a material/texmap is used.
        This method is called automatically by this class (BaseTranslator_MtlBase) before the material is translated, but exposed here
        in case any external code might need to access it independently.
        \param mtlBase The material/texmap that forms the root of the material/texmap tree to be updated.
        \param t The time at which the material is to be updated.
        \param [in, out] validity The validity of the material at the given time, to be intersected with the input value. */
    static void UpdateMaterialHierarchy(MtlBase& mtlBase, const TimeValue t, Interval& validity);

protected:

    //! Returns the MtlBase being handled by this translator.
    MtlBase* GetMtlBase() const;

private:

    MtlBase* const m_mtl_base;
};

}}	// namespace 

#pragma warning(pop)

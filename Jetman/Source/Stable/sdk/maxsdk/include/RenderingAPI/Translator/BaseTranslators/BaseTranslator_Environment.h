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

#include "BaseTranslator_Texmap.h"

#pragma warning(push)
#pragma warning(disable: 4275 4251)

namespace MaxSDK
{;
namespace RenderingAPI
{;

class IEnvironmentContainer;

//==================================================================================================
// class BaseTranslator_Environment
//
/*! Base class for translating an environment.
    This base class provides basic, common functionality related to translating an environment. It handles:
    \li Monitoring the scene environment and invalidating the translator on any changes.
    \li Calling RenderBegin() on the environment before translation.
    \li Calling MtlBase::Update() on the environment before translation.
*/
class RenderingAPIExport BaseTranslator_Environment : 
    public BaseTranslator_Texmap
{
public:

    //! \param environment The environment to be translated.
    //! \param graphNode The graph node to be passed to Translator::Translator().
    BaseTranslator_Environment(IEnvironmentContainer* const environment, TranslatorGraphNode& graphNode);
    ~BaseTranslator_Environment();

    // -- inherited from Translator
    virtual MSTR GetTimingCategory() const override;

protected:

    //! Returns the environment being handled by this translator.
    IEnvironmentContainer* GetEnvironment() const;

private:

    IEnvironmentContainer* const m_environment;
};

}}	// namespace 

#pragma warning(pop)

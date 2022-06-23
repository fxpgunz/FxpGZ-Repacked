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
// class TranslationResult
//
/*! Encapsulates all of the possible result states for a translation operation.

    For simplicity, implementations of Translator::Translate() can return a result value by directly referencing the enum, e.g.:
    \par
    \code
    Translate(...)
    {
        return TranslationResult::Success;
    }
    \endcode
*/
class TranslationResult
{
public:

    //! The possible values that a TranslationResult may take.
    enum State
    {
        //! Translation succeeded
        Success,
        //! Translation failed (for any reason)
        Failure,
        //! Translation has been aborted, as requested by the user or the system.
        Aborted
    };

    //! Default constructor initializes state to Success.
    TranslationResult();

    //! Constructs (implicitly) from given state.
    TranslationResult(const State val);

    //! Implicit conversion operator to the State enum.
    operator State() const;

private:

    State m_value;
};

}}	// namespace 

#include "TranslationResult.inline.h"
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

#include <strbasic.h>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class ITranslationManager
//
/*! Interface used for progress reporting by translators.

    Translators must use this interface to report progress; they shouldn't use IRenderingProcess directly. This is to ensure stability when
    translating for an interactive render session. When updating the UI tho show the new progress status, window messages get processed; these
    messages may represent user input which will in turn be processed into scene modifications. The modifications may interfere with the translation
    process. For example, if the user hits the delete key, and that command gets processed while updating the progress bar, then a scene object
    may get deleted while it's still being accessed by the translator. This interface includes special checks to warn the translator of such situations,
    through the <i>abort_immediately</i> parameter of the method it exposes.
*/
class ITranslationProgress
{
public:

    /*! This method is equivalent to IRenderingProcess::SetRenderingProgressTitle(), but contains additional handling to abort translation if a scene
        change occurs while processing the UI messages.
        \param title The string to display as the progress title.
        \param abort_immediately [out] If set to true (by the system), the caller <b>must immediately abort</b> the translation that is currently
            in progress: no further access to the 3ds Max must be performed, under risk of crash (e.g. if a scene element was deleted);
            Translate() must immediately return TranslationResult::Aborted. Failure to abide by this will result in race conditions, random and
            hard to reproduce crashes. */
    virtual void SetTranslationProgressTitle(
        const MCHAR* title,
        bool& abort_immediately) = 0;

        /*! This method is equivalent to IRenderingProcess::SetRenderingProgress(), but contains additional handling to abort translation if a scene
        change occurs while processing the UI messages.
        \param done The number of units of work which are done. Current progress is expressed as: <i>done / total</i>.
        \param total The number of units of work in total. Current progress is expressed as: <i>done / total</i>.
        \param abort_immediately [out] If set to true (by the system), the caller <b>must immediately abort</b> the translation that is currently
            in progress: no further access to the 3ds Max must be performed, under risk of crash (e.g. if a scene element was deleted);
            Translate() must immediately return TranslationResult::Aborted. Failure to abide by this will result in race conditions, random and
            hard to reproduce crashes. */
    virtual void SetTranslationProgress(
        const size_t done, 
        const size_t total,
        bool& abort_immediately) = 0;

protected:

    // Protected destructor forces using implementation for deletion.
    virtual ~ITranslationProgress() {}
};

}};	// namespace 


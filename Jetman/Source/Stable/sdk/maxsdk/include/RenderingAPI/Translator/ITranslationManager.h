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

// local
#include "TranslationResult.h"
// Max SDK
#include <maxheap.h>
#include <maxtypes.h>
#include <assert1.h>
// std
#include <memory>

namespace MaxSDK
{;
namespace RenderingAPI
{;

class TranslatorKey;
class Translator;
class ITranslationProgress;
class IRenderingProcess;
class IRenderingLogger;
class IRenderSessionContext;

//==================================================================================================
// class ITranslationManager
//
/*! Manages the translation graph for a scene.

    The translation manager is responsible for maintaining the translation graph for a scene. It should be created by the render session that
    wishes to use the translation component of the Rendering API. Once created, this interface enables the creation of the root of the translation
    graph - the <i>scene translator</i>, which then serves as the anchor point for the translation of the whole scene.

    \remark This interface is implemented by the system. An instance of the interface can be obtained from IRenderSessionContext::GetTranslationManager().
*/
class ITranslationManager :
    public MaxHeapOperators
{
public:

    virtual ~ITranslationManager() {}

    /*! Acquires the root translator for a scene.
        This method acquires the root of the translation graph for the scene. This root is responsible for acquiring other translators, a recursive
        process which results in the scene's complete translation graph. The root translator is typically responsible for acquiring child translators for
        the camera, scene nodes, and environment.
        \param key The key that uniquely identifies the root translator. Given that a scene root is unique, this key need not store any data (as
            the root translator's input would typically dependent exclusively on the contents of the IRenderSessionContext), but should uniquely identify
            the type/class of the root translator.
        \param t The time at which the root translator is to be initially translated.
        \param [out] result Stores whether the translation was successful.
        \return A pointer to the root translator, dynamically cast to the templated type. Null if and only if translation wasn't successful. 
        \remark The root translator, returned by this method, is reference counted and must be released by calling ReleaseSceneTranslator(). 
        \remark Multiple root translators can be created, if needed. The translation manager would, in that case, manage multiple scene translation graphs.
            These graphs, though anchored using separate roots, would be able to share translators within the graph. The only potential issue, with
            such a scheme, is that every node in the graph can only be translated for a single time value, meaning that the scenes could not be translated
            at different time values. */
    template<typename TranslatorType> 
    TranslatorType* TranslateScene(const TranslatorKey& key, const TimeValue t, TranslationResult& result);

    /*! Updates the translation graph anchored at the given root.
        This method traverses the translation graph, starting at the given root, and updates (re-translates) every node which is not valid at the given
        time value.
        \remark This method may be called from any thread. Internally, the work will be performed from the main thread.
        \param rootTranslator The root translator node at which to start the traversal. This has to be a root translator, as returned by TranslateScene().
        \param t The time at which to update the scene translation graph.
        \return The result (success or failure) of the update. */
    virtual TranslationResult UpdateScene(Translator& rootTranslator, const TimeValue t) = 0;

    /*! Checks whether any part of the translation graph is invalid at the given time.
        \param rootTranslator The root of the translation graph to be checked.
        \param t The time value at which to check for validity.
        \return True if any node, in the translation graph, is invalid at the given time. False otherwise.
        \remark This method performs no acccess to the 3ds Max scene; it merely traverses the translation graph and checks for its validity.
            Consequently, it's safe to call it from any thread. But there is no locking in place, so the translation manager should not be accessed
            in parallel by another thread. */
    virtual bool DoesSceneNeedUpdate(const Translator& rootTranslator, const TimeValue t) = 0;

    /*! Releases a reference to a root translator.
        Root translators are reference counted. Acquiring one through TranslateScene() increments its reference count, which must then be decremented
        using this method.
        \remark The released root translator should no longer be accessed following this call. 
        \remark This call may result in the deletion (garbage collection) of parts or all of the translation graph, depending on whether the reference
            count of the root node goes to 0.
        \param rootTranslator The root translator to be released. */
    virtual void ReleaseSceneTranslator(const Translator& rootTranslator) = 0;

protected:

    virtual Translator* TranslateSceneInternal(const TranslatorKey& key, const TimeValue t, TranslationResult& result) = 0;
};

}};	// namespace 

#include "ITranslationManager.inline.h"
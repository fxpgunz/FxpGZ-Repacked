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

#include "TranslationResult.h"

#include "../RenderingAPI.h"

// Max SDK
#include <Noncopyable.h>
#include <interval.h>
#include <strclass.h>

// std
#include <memory>
#include <set>

namespace Max
{
    namespace RenderingAPI
    {
        // Forward-declare internal class used to store the manage the translator.
        class TranslatorGraphNode;
    }
}

namespace MaxSDK
{;
namespace RenderingAPI
{;

class TranslatorKey;
class TranslatorStatistics;
class ITranslationProgress;
class ITranslatorOutput;
class IRenderSessionContext;
using Max::RenderingAPI::TranslatorGraphNode;

#pragma warning(push)
#pragma warning(disable: 4275)      // Disable warning about base class not dll-exported

//==================================================================================================
// class Translator
//
/*! Base class for a translator.

    Translators are responsible for translating a set of inputs (classes TranslatorKey and IRenderSessionContext) into one or more outputs 
    (class TranslatorOutput). Together, the translators form a translation graph that is anchored around the scene's root translator. Each 
    Translator is entirely self-contained, such that it may be garbage collected once no longer used by the scene's translation graph.
*/
class RenderingAPIExport Translator :
    // Noncopyable to inherit MaxHeapOperators as well as avoid C4215
    public MaxSDK::Util::Noncopyable
{
public:

    typedef std::set<TimeValue> KeyframeList;

    /*! The constructor expects a reference to a TranslatorGraphNode.
        TranslatorGraphNode is internal to the system; the public API merely forward declares it. There is a one-to-one relationship between 
        classes Translator and TranslatorGraphNode, with Translator taking charge of the plugin-side translation task, whereas TransaltorGraphNode
        is responsible for the system-side management of the translation graph. This relationship is akin to the Pimpl pattern. */
    // This constructor expects the graph node passed to TranslatorKey::AllocateNewTranslator()
    explicit Translator(TranslatorGraphNode& graphNode);
    virtual ~Translator();

    /*! Performs the translation for which this class is responsible.
        This method must be implemented to translate the translator's inputs (formed by the TranslatorKey and, possibly, the IRenderSessionContext)
        into a set of outputs (formed by TranslatorOutput). This method may be called to perform the initial translation of the inputs, or to
        re-perform the translation because the translator has been invalidated.
        \remark Whenever this method is called, all outputs that have been acquired from dependencies <b>must</b> be re-acquired. That is, all child 
            translators must be acquired. The outputs should never be cached in any way. This is necessary because all the child dependencies are
            re-set when a translator is re-translated. This ensures that the graph is continuously up-to-date, with any dangling translator being
            immediately garbage collected.
        \param translationTime The time at which the scene is being translated.
        \param [in, out] newValidity The validity of the translator, to be intersected with the input value. The translator will be re-translated 
            whenever the current time value falls outside of this validity.
        \param translationProgress The interface through which translation progress is to be reported (optionally). 
        \param [out] keyframesNeeded Optional set of keyframe time values for which TranslateKeyframe() should be called
            following the call to Translate(). 
        \return The (success) state of the translation process. */
	virtual TranslationResult Translate(
		const TimeValue translationTime,
		Interval& newValidity,         	
        ITranslationProgress& translationProgress,
        KeyframeList& keyframesNeeded) = 0;
    /*! Performs translation for a single keyframe.
        This method is designed to optimize the translation of keyframe data for use with motion blur. If an object
        must be evaluated at various time values (keyframes) in order to generate motion blur data (i.e. motion
        transforms, motion vectors, etc.), then it may be problematic to generate all that data within the implementation
        of Translate(). Evaluating an object at various time values, within each call to Translate(), can cause a ping-pong
        effect if various scene objects were dependent on each other: the object being depended on would be re-evaluated
        for every keyframe of every dependent object - for example, if three objects were dependent on a fourth, and each
        had to be evaluated at 4 keyframes for purposes of motion blur, then the fourth object would be fully evaluated a total
        of 16 times during scene translation. To avoid this problem, it makes sense to serialize the evaluation of keyframes,
        such that all objects are first evaluate at the first keyframe, then the second, and so on. This way, the fourth object
        ends up being evaluated only five times (once at time t, then once for each keyframe). When dependencies exist on
        complex objects, this can have a major impact on overall translation time.
        \remark AcquireChildTranslator() may not be called from this method (it will always return null).  Child
            translators must be acquired in Translate(), and cached locally if needed by TranslateKeyframe().
        \remark It is possible for this method to set translator outputs; if this happens, then the parent translators
            would be invalidated, and re-translated once all keyframes have been processed. This allows parent translators
            to have dependencies on outputs that are created as a result of translating the keyframes. But in such a case,
            of course, the parent translator would have to understand that, the first time gets translated, the outputs of
            its child translator will not exist yet... and it should not fail in that case, but simply knowlingly wait for
            the second translation to be successful.
        \param frame_time The time value at which the scene is being translated - equal to the 't' argument passed to Translate().
        \param keyframe_time The time value at which the current keyframe is to be evaluated. This is one of the values
            returned through the keyframesNeeded argument of Translate(); TranslateKeyframe() will be called once for each
            keyframe returned through keyframesNeeded, in ascending order.
        \param translationProgress The interface through which translation progress is to be reported (optionally). 
        \return The (success) state of the translation process. */
    virtual TranslationResult TranslateKeyframe(
        const TimeValue frame_time, 
        const TimeValue keyframe_time,
        ITranslationProgress& translationProgress) = 0;

    /*! Called just before Translate() to enable pre-processing of any kind.
        This method is to be used to perform per-processing that needs to happen before Translate() is called. A separate method is needed
        in case the class hierarchy results in a sub-classes Translate() being called before this class. Example uses of this method include calling
        IRenderSessionContext::CallRenderBegin() or MtlBase::Update(). 
        \param translationTime The time at which Translate() is about to be called.
        \param [in, out] validity The validity of the translator, to be intersected with the input value. The translator will be re-translated 
            whenever the current time value falls outside of this validity. */
    virtual void PreTranslate(const TimeValue translationTime, Interval& validity) = 0;
    /*! Called just after Translate() to enable post-processing of any kind.
        The principle is identical to that of PreTranslate(), except that this method is called immediately <i>after</i> the call to Translate().
        \param translationTime The time at which Translate() was just called.
        \param [in, out] validity The validity of the translator, to be intersected with the input value. The translator will be re-translated 
            whenever the current time value falls outside of this validity. 
        \remark This method is called regardless of the return value of Translate(). */
    virtual void PostTranslate(const TimeValue translationTime, Interval& validity) = 0;

    /*! Called by the system to accumulate statistics on the translated scene.
        The implementation may add to the statistics, if this translator outputs anything that is relevant to them. 
        \param stats The interface with which statistics may be recorded. */
    virtual void AccumulateStatistics(TranslatorStatistics& stats) const = 0;
    /*! Accumulates statistics on the whole translate graph, starting at this translation node.
        Included in the traversal are this translator, and all descendant translators. This method would normally be called on a scene root translator. 
        \param [out] statistics The interface with which statistics are recorded.  */
    void AccumulateGraphStatistics(TranslatorStatistics& statistics) const;

    /*! Deferred checks whether this translator should be invalidated.
        Used in combination with a call to Invalidate(true), this method is called when checking the scene validity to see if this translator should
        be deferred invalidated. Deferred invalidation is necessary if the translator requires access to the scene to determine if it is in fact 
        invalid. Accessing the scene within notification callbacks is generally unsafe, as some notifications are sent while the scene is in an 
        intermediate state.
        \param t The time at which the validity check is performed.
        \param previous_validity The current validity interval interval of the translator.
        \return The new validity interval of the translator; empty if the translator is to be invalidated outright. Return previous_validity
            to leave the translator's validity as-is.*/
    virtual Interval CheckValidity(const TimeValue t, const Interval& previous_validity) const = 0;

    /*! Returns the string label to be used by the time reporting mechanism.
        The translation system automatically times every translator for reporting, to the log, a categorized table that shows where translation
        time is spent. This method returns the label under which this translator's time is reported. Multiple translator classes can use the
        same label, resulting in their time being added up to a common total. Returning an empty string causes this translator's time to be reported
        under a "miscellaneous" category. */
    virtual MSTR GetTimingCategory() const = 0;

    /*! For internal use.    
        Returns the TranslatorGraphNode associated with this translator, a private class used by the system as a sort of Pimpl pattern. */
    TranslatorGraphNode& GetGraphNode() const;

protected:

    /*! Acquires a dependency to a another translator and its outputs.
        This method acquires a dependency to the translator that matches the given TranslatorKey. An existing translator will be re-used, if one
        exists for the given key - otherwise a new one will be created. The acquired translator is automatically translated as/if necessary
        and its outputs are automatically acquired by the current translator, such that they will not be garbage collected so long as they are in use.
        \param key The key that uniquely identifies the translator to be acquired.
        \param t The time at which the acquired translator will be translated.
        \param translation_progress The progress interface that was passed to the call to Translate().
        \param result Stores the result of the translation of the child translator. 
        \return The acquired child translator, dynamically cast to the given template type. Returns null if and only the tranlsation did not succeed
            (result != TranslationResult::Success).     */
    template<typename TranslatorType> 
    const TranslatorType* AcquireChildTranslator(
        const TranslatorKey& key, 
        const TimeValue t, 
        ITranslationProgress& translation_progress, 
        TranslationResult& result);

    /*! Invalidates this translator, typically as a result of a change notification callback.
        \param defer_invalidation_check If set to true, then the translator is not immediately invalidated. Instead, upon next checking whether the
            scene requires an update, CheckValidity() will be called to assess the translator's validity. Deferred invalidation is necessary if
            the translator requires accessing the scene to assess its validity; accessing the scene within a change notification callback is generally
            unsafe as these notifications are sometimes send out while the scene is in an intermediate state. */
    void Invalidate(const bool defer_invalidation_check = false);

	/*! Invalidates all parents of this translator.
		Invalidation of the parent translators forces them to be re-translated on the next scene update. This may be necessary if the outputs of the translator
		change. Normally, changing the outputs of the translator using the SetOutput() method automatically invalidates the parents. But there may be cases
		where this mechanism is insufficient, e.g. if a scene change would require a translator to be discarded and re-created from scratch (as the TranslatorKey
		could end up creating a translator of a different class). */
    void InvalidateParents();

    /*! Flags the objects/inputs, being translated by this translated, as having been deleted.
        This method is to be called when a change notification callback notifies us that whatever this translator translates has been deleted. For
        example, if this translator were to translate an INode, and that INode were to be deleted from the scene, then the change notification callback
        should call this method. The system will always ignore translators that have been flagged this way, to avoid accessing dangling pointers.
        But should this translator be re-acquired by another one, then this flag is cleared and the translator is allowed to proceed as normal - the 
        rationale being that the object is assumed to have been un-deleted (through an undo operation). */
    void TranslatedObjectDeleted();

    //! \name Translator Outputs
    //@{
    /*! Returns the number of outputs that are present on this translator. */
    size_t GetNumOutputs() const;

    /*! Returns the output at the given index, dynamically cast to the given sub-class of ITranslatorOutput. 
        \remark Translator outputs are automatically acquired (through the system) by dependent translators, such that outputs will get garbage collected
            only once they are no longer referenced by anyone.
        \remark This outputs returned by this method are flagged const, such that they may not be modified once installed. Modifying an output value
            can only be done by replacing that output - i.e. calling SetOutput(). This allows the system to catch all changes in output values, 
            and automatically notify / invalidate all dependent translators which then need to be updated to take the new output value into account. 
        \remark Will return null if the given output has not been initialized. */
    template<typename OutputType> 
    std::shared_ptr<const OutputType> GetOutput(const size_t index) const;

    /*! Initializes the size of the internal array, used to store the outputs.
        GetNumOutputs() will henceforth return the value passed here.
        \remark This method is a mere convenience and does not need to be called; SetOutput() will automatically increase the size of the array as necessary. */
    void SetNumOutputs(const size_t num);
    /*! Initializes the output, at the given index, to the given value.
        \remark The array, used internally to store the outputs, is automatically increased in size if needed.  */
    void SetOutput(const size_t index, std::shared_ptr<const ITranslatorOutput> output);
    /*! Deletes the output at the given index, such that GetOutput() will henceforth return null. */
    void ResetOutput(const size_t index);
    /*! Deletes all outputs.*/
    void ResetAllOutputs();
    /*! Shortcut method to initialize outputs which map to simple types that require no external cleanup code.
        The output mechanism is primarily meant to store interfaces that encapsulates handles or pointers to internal data structures, which may
        need special cleanup code when the output is destroyed. But if the output value has no cleanup code, or if this cleanup code is encapsulated 
        within the output type's own destructor, then this method may be used to avoid the hassle of implementing a custom ITranslatorOutput. 
        \remark The output value will be <b>copied</b>, making this method generally non-suitable for any data types that hold references which must 
            be destroyed by the destructor. */
    template<typename SimpleValueType> 
    void SetOutput_SimpleValue(const size_t output_index, const SimpleValueType& value);
    /*! Returns an output initialized using SetOutput_SimpleValue().
        \param output_index The index of the output to be fetched.
        \param default_value The value to be returned, if the output should be missing or uninitialized.
        \see SetOutput_SimpleValue(). */
    template<typename SimpleValueType> 
    SimpleValueType GetOutput_SimpleValue(const size_t output_index, const SimpleValueType& default_value) const;
    //@}

    //! Returns the render session context for which this translator was created.
    IRenderSessionContext& GetRenderSessionContext();
    //! Returns the render session context for which this translator was created.
    const IRenderSessionContext& GetRenderSessionContext() const;

private:

    template<typename ValueType> class SimpleValueTranslatorOutput;

    // Private implementations used by the (inlined) templated methods.
    const Translator* AcquireChildTranslatorInternal(const TranslatorKey& key, const TimeValue t, ITranslationProgress& translationProgress, TranslationResult& result);
    std::shared_ptr<const ITranslatorOutput> GetOutputInternal(const size_t index) const;

private:

    Max::RenderingAPI::TranslatorGraphNode& mGraphNode;
};

}}	// namespace 

#pragma warning(pop)

#include "Translator.inline.h"

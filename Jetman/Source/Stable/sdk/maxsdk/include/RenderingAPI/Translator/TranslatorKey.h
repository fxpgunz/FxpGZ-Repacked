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

// Max SDK
#include <maxtypes.h>
// std
#include <memory>

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

class Translator;
class IRenderSessionContext;
using Max::RenderingAPI::TranslatorGraphNode;

//==================================================================================================
// class TranslatorKey
//
/*! Unique identifier for a translator in the scene's translation graph.

    The TranslatorKey plays three roles:
    \li It unique identifies a single translator in the tranlsation scene graph. It does this through operator==(), with the help of a hash value.
    \li It encapsulates the <i>inputs</i> of a translator - everything that a translator needs to produce its outputs. This may be an INode*,
        a Mtl*, a set of boolean flags, an arbitrary data structure.
    \li It is the factory for the type of translator it represents.
    
    This class should rarely be implemented directly by the plugin developer. Instead, consider using one of the built-in implementations of translator
    keys, which greatly simplify the task:
    \li class GenericTranslatorKey_Empty
    \li class GenericTranslatorKey_SinglePointer
    \li class GenericTranslatorKey_SingleReference
    \li class GenericTranslatorKey_Struct
*/
class TranslatorKey :
    public MaxHeapOperators
{
public:

    /*! Initializes the translator key with a given hash value.
        The has value is used to efficiently search the lookup table that contains all of the scene's translators. As is standard with hash values,
        it doesn't have to be unique - but should be well-distributed within the space covered by size_t. Two keys that are deemed to be equal
        by operator==() must have identical hash values. */
	TranslatorKey(const size_t hash);
    virtual ~TranslatorKey() {}

    /*! Returns whether the given key is identical to this one.
        The comparison should typically consider, both, the set of values stored in the key, and the type of the key, such that two keys
        that store identical values (e.g. the same INode*) but which create different types of translators (e.g. Instance versus InstanceAttributes)
        would be deemed to be different. Such comparison will typically be performed using a dynamic_cast.*/
	virtual bool operator==(const TranslatorKey& rhs) const = 0;
    
    //! Returns the hash value passed to the constructor.
    size_t get_hash() const;

    /*! Allocates and initialized a brand new copy of this key.
        For efficiency, keys will normally be allocated on the stack - such that Translator::AcquireChildTranslator() will be called with a temporary
        key, avoiding an unnecessary memory allocation. But if a new translator needs to be created, then the system also needs to store a copy of the 
        key. For this purpose, it creates a clone of the key using this method. */
	virtual std::unique_ptr<const TranslatorKey> CreateClone() const = 0;

    /*! Factory method for the type of translator represented by this key.
        Whenever a new translator needs to be created for a given key, that key is responsible for allocating the new instance of that type
        of translator. 
        \param translator_graph_node The parameter which is to be passed to the constructor of class Translator.
        \param render_session_context The render session context within which the translator is being created.
        \param initial_time The time at which the translator will initially be translated. This may be used, by the method, to evalate scene elements
            and thus judge exactly what type of translator needs to be allocated (if a single key type maps to multiple types of translators).
        \return A new instance of a translator which uses this key as input. */
    virtual std::unique_ptr<Translator> AllocateNewTranslator(
        TranslatorGraphNode& translator_graph_node, 
        const IRenderSessionContext& render_session_context, 
        const TimeValue initial_time) const = 0;

private:

	// Disable copy operator and solve compiler warning.
	TranslatorKey& operator=(TranslatorKey&);

private:

    const size_t m_hash;
};

}}	// namespace 

#include "TranslatorKey.inline.h"


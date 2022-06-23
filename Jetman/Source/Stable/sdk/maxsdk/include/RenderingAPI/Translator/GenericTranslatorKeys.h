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

#include "TranslatorKey.h"

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class DefaultTranslatorAllocator
//==================================================================================================

template<class TranslatorType>
class DefaultTranslatorAllocator
{
public:
    template<class TranslatorKeyType>
    std::unique_ptr<Translator> operator()(const TranslatorKeyType& key, TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const;
};

//==================================================================================================
// class GenericTranslatorKey_Empty
//
// A generic implementation of TranslatorKey which maps to no data (i.e. it has only a key type).
//==================================================================================================
template <class TranslatorType, class TranslatorAllocator = DefaultTranslatorAllocator<TranslatorType>>
class GenericTranslatorKey_Empty : 
    public TranslatorKey
{
public:

    GenericTranslatorKey_Empty();

    // -- inherited from TranslatorKey
    virtual bool operator==(const TranslatorKey& rhs) const override;
    virtual std::unique_ptr<const TranslatorKey> CreateClone() const override;
    virtual std::unique_ptr<Translator> AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const override;
};

//==================================================================================================
// class GenericTranslatorKey_SinglePointer
//
// A generic implementation of TranslatorKey which maps to a single pointer.
//==================================================================================================
template <class TranslatorType, typename PointerType, class TranslatorAllocator = DefaultTranslatorAllocator<TranslatorType>>
class GenericTranslatorKey_SinglePointer : 
	public TranslatorKey
{
public:

	GenericTranslatorKey_SinglePointer(PointerType* ptr);

    operator PointerType*() const;

    // -- inherited from TranslatorKey
    virtual bool operator==(const TranslatorKey& rhs) const override;
    virtual std::unique_ptr<const TranslatorKey> CreateClone() const override;
    virtual std::unique_ptr<Translator> AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const override;

private:

	PointerType* const m_ptr;
};

//==================================================================================================
// class GenericTranslatorKey_SingleReference
//
// A generic implementation of TranslatorKey which maps to a single reference.
//==================================================================================================
template <class TranslatorType, typename ReferencedType, class TranslatorAllocator = DefaultTranslatorAllocator<TranslatorType>>
class GenericTranslatorKey_SingleReference : 
    public TranslatorKey
{
public:

    GenericTranslatorKey_SingleReference(ReferencedType& ref);

    operator ReferencedType&() const;

    // -- inherited from TranslatorKey
    virtual bool operator==(const TranslatorKey& rhs) const override;
    virtual std::unique_ptr<const TranslatorKey> CreateClone() const override;
    virtual std::unique_ptr<Translator> AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const override;

private:

    ReferencedType& m_ref;
};

//==================================================================================================
// class GenericTranslatorKey_Struct
//
// A generic implementation of TranslatorKey which maps to an arbitrary data structure.
// The struct must have a method with the following definition: size_t get_hash() const.
//==================================================================================================
template <class TranslatorType, typename StructType, class Hash = std::hash<StructType>, class TranslatorAllocator = DefaultTranslatorAllocator<TranslatorType>>
class GenericTranslatorKey_Struct : 
    public TranslatorKey
{
public:

    GenericTranslatorKey_Struct(const StructType& data);

    operator const StructType&() const;
    const StructType& GetData() const;

    // -- inherited from TranslatorKey
    virtual bool operator==(const TranslatorKey& rhs) const override;
    virtual std::unique_ptr<const TranslatorKey> CreateClone() const override;
    virtual std::unique_ptr<Translator> AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const override;

private:

    StructType m_data;
};

}}	// namespace 

#include "GenericTranslatorKeys.inline.h"

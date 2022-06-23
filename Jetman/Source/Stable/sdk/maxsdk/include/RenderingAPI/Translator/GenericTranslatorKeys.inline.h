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

#include <typeindex>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class DefaultTranslatorAllocator
//==================================================================================================

template<class TranslatorType>
template<class TranslatorKeyType>
std::unique_ptr<Translator> DefaultTranslatorAllocator<TranslatorType>::operator()(const TranslatorKeyType& key, TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& /*render_session_context*/, const TimeValue /*initial_time*/) const
{
    return std::unique_ptr<Translator>(new TranslatorType(key, translator_graph_node));
}

//==================================================================================================
// class GenericTranslatorKey_Empty
//==================================================================================================

template <class TranslatorType, class TranslatorAllocator>
inline GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>::GenericTranslatorKey_Empty()
    : TranslatorKey(std::type_index(typeid(GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>)).hash_code())
{

}

template <class TranslatorType, class TranslatorAllocator>
bool GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>::operator==(const TranslatorKey& rhs) const 
{
    // No data: just compare type
    const GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>* cast_rhs = dynamic_cast<const GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>*>(&rhs);
    return (cast_rhs != nullptr);
}

template <class TranslatorType, class TranslatorAllocator>
inline std::unique_ptr<const TranslatorKey> GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>::CreateClone() const
{
    return std::unique_ptr<const TranslatorKey>(new GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>());
}

template <class TranslatorType, class TranslatorAllocator>
std::unique_ptr<Translator> GenericTranslatorKey_Empty<TranslatorType, TranslatorAllocator>::AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const 
{
return TranslatorAllocator()(*this, translator_graph_node, render_session_context, initial_time);
}

//==================================================================================================
// class GenericTranslatorKey_SinglePointer
//==================================================================================================
template <class TranslatorType, typename PointerType, class TranslatorAllocator>
GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>::GenericTranslatorKey_SinglePointer(PointerType* ptr)
	: TranslatorKey(std::type_index(typeid(GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>)).hash_code() ^ std::hash<PointerType*>()(ptr)),
	m_ptr(ptr)
{
    
}

template <class TranslatorType, typename PointerType, class TranslatorAllocator>
GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>::operator PointerType*() const
{
    return m_ptr;
}

template <class TranslatorType, typename PointerType, class TranslatorAllocator>
bool GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>::operator==(const TranslatorKey& rhs) const 
{
    const GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>* cast_rhs = 
        dynamic_cast<const GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>*>(&rhs);
    return (cast_rhs != nullptr) && (m_ptr == cast_rhs->m_ptr);
}

template <class TranslatorType, typename PointerType, class TranslatorAllocator>
std::unique_ptr<const TranslatorKey> GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>::CreateClone() const
{
	return std::unique_ptr<const TranslatorKey>(new GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>(m_ptr));
}

template <class TranslatorType, typename PointerType, class TranslatorAllocator>
std::unique_ptr<Translator> GenericTranslatorKey_SinglePointer<TranslatorType, PointerType, TranslatorAllocator>::AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const 
{
    return TranslatorAllocator()(*this, translator_graph_node, render_session_context, initial_time);
}

//==================================================================================================
// class GenericTranslatorKey_SingleReference
//==================================================================================================
template <class TranslatorType, typename ReferencedType, class TranslatorAllocator>
GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>::GenericTranslatorKey_SingleReference(ReferencedType& ref)
    : TranslatorKey(std::type_index(typeid(GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>)).hash_code() ^ std::hash<ReferencedType*>()(&ref)),
    m_ref(ref)
{

}

template <class TranslatorType, typename ReferencedType, class TranslatorAllocator>
GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>::operator ReferencedType&() const
{
    // It's safe to dereference this as it was initialized from a reference
    return m_ref;
}

template <class TranslatorType, typename ReferencedType, class TranslatorAllocator>
bool GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>::operator==(const TranslatorKey& rhs) const 
{
    const GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>* cast_rhs  = 
        dynamic_cast<const GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>*>(&rhs);
    return (cast_rhs != nullptr) && (&m_ref == &(cast_rhs->m_ref));
}

template <class TranslatorType, typename ReferencedType, class TranslatorAllocator>
std::unique_ptr<const TranslatorKey> GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>::CreateClone() const
{
    return std::unique_ptr<const TranslatorKey>(new GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>(m_ref));
}

template <class TranslatorType, typename ReferencedType, class TranslatorAllocator>
std::unique_ptr<Translator> GenericTranslatorKey_SingleReference<TranslatorType, ReferencedType, TranslatorAllocator>::AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const 
{
    return TranslatorAllocator()(*this, translator_graph_node, render_session_context, initial_time);
}

//==================================================================================================
// class GenericTranslatorKey_Struct
//==================================================================================================

template <class TranslatorType, typename StructType, class Hash, class TranslatorAllocator>
GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>::GenericTranslatorKey_Struct(const StructType& data)
    : TranslatorKey(std::type_index(typeid(GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>)).hash_code() ^ Hash()(data)),
    m_data(data)
{

}

template <class TranslatorType, typename StructType, class Hash, class TranslatorAllocator>
GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>::operator const StructType&() const
{
    return m_data;
}

template <class TranslatorType, typename StructType, class Hash, class TranslatorAllocator>
const StructType& GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>::GetData() const
{
    return m_data;
}

template <class TranslatorType, typename StructType, class Hash, class TranslatorAllocator>
bool GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>::operator==(const TranslatorKey& rhs) const 
{
    const GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>* cast_rhs =
        dynamic_cast<const GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>*>(&rhs);
    return (cast_rhs != nullptr) && (m_data == cast_rhs->m_data);
}

template <class TranslatorType, typename StructType, class Hash, class TranslatorAllocator>
std::unique_ptr<const TranslatorKey> GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>::CreateClone() const
{
    return std::unique_ptr<const TranslatorKey>(new GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>(m_data));
}

template <class TranslatorType, typename StructType, class Hash, class TranslatorAllocator>
std::unique_ptr<Translator> GenericTranslatorKey_Struct<TranslatorType, StructType, Hash, TranslatorAllocator>::AllocateNewTranslator(TranslatorGraphNode& translator_graph_node, const IRenderSessionContext& render_session_context, const TimeValue initial_time) const 
{
    return TranslatorAllocator()(*this, translator_graph_node, render_session_context, initial_time);
}


}}	// namespace 

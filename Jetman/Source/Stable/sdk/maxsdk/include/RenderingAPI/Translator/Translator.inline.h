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

#include "ITranslatorOutput.h"

// Max SDK
#include <assert1.h>

namespace MaxSDK
{;
namespace RenderingAPI
{;

//==================================================================================================
// class Translator::SimpleValueTranslatorOutput
//
// Implementation of a ITranslatorOutput which stores a simple, templated value. The stored value
// must not be a reference type (pointer, handle) as the value is simply discarded when the output
// is deleted.
//==================================================================================================
template<typename ValueType>
class Translator::SimpleValueTranslatorOutput :
    public ITranslatorOutput
{
public:

    SimpleValueTranslatorOutput(const ValueType& value) : m_value(value) {}
    const ValueType& get_value() const { return m_value; }

private:

    const ValueType m_value;
};

//==================================================================================================
// class Translator
//==================================================================================================

template<typename TranslatorType> 
const TranslatorType* Translator::AcquireChildTranslator(const TranslatorKey& key, const TimeValue t, ITranslationProgress& translationProgress, TranslationResult& result)
{
    const Translator* translator = AcquireChildTranslatorInternal(key, t, translationProgress, result);
    const TranslatorType* cast_translator = dynamic_cast<const TranslatorType*>(translator);
    DbgAssert(translator == cast_translator);       // check that cast was valid
    return cast_translator;
}

template<typename OutputType> 
std::shared_ptr<const OutputType> Translator::GetOutput(const size_t index) const
{
    const std::shared_ptr<const ITranslatorOutput> output = GetOutputInternal(index);
    const std::shared_ptr<const OutputType> cast_output = std::dynamic_pointer_cast<const OutputType>(GetOutputInternal(index));
    DbgAssert(output == cast_output);       // check that cast was valid
    return cast_output;        
}

inline void Translator::ResetOutput(const size_t output_index)
{
    SetOutput(output_index, std::shared_ptr<ITranslatorOutput>());
}

inline void Translator::ResetAllOutputs()
{
    SetNumOutputs(0);
}

template<typename SimpleValueType> 
void Translator::SetOutput_SimpleValue(const size_t output_index, const SimpleValueType& value)
{
    // Check if the value is different before setting it (to avoid invalidating the translation graph if the value doesn't change)
    const auto output = GetOutput<SimpleValueTranslatorOutput<SimpleValueType>>(output_index);
    if((output == nullptr) || (output->get_value() != value))
    {
        SetOutput(output_index, std::shared_ptr<SimpleValueTranslatorOutput<SimpleValueType>>(new SimpleValueTranslatorOutput<SimpleValueType>(value)));
    }
}

template<typename SimpleValueType> 
SimpleValueType Translator::GetOutput_SimpleValue(const size_t output_index, const SimpleValueType& default_value) const
{
    const auto output = GetOutput<SimpleValueTranslatorOutput<SimpleValueType>>(output_index);
    return (output != nullptr) ? output->get_value() : default_value;
}

}}	// namespace 


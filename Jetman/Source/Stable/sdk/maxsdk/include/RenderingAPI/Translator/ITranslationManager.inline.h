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

template<typename TranslatorType> 
TranslatorType* ITranslationManager::TranslateScene(const TranslatorKey& key, const TimeValue t, TranslationResult& result)
{
    Translator* translator = TranslateSceneInternal(key, t, result);
    TranslatorType* cast_translator = dynamic_cast<TranslatorType*>(translator);
    if(DbgVerify(translator == cast_translator))
    {
        return cast_translator;
    }
    else
    {
        result = TranslationResult::Failure;
        return nullptr;
    }
}

}};	// namespace 


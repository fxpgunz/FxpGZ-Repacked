//
// Copyright 2013 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//

#pragma once

#include ".\BaseInterface.h"

/** Interface ID of class TexmapThreadSafe. */
#define ITEXMAP_THREAD_SAFE Interface_ID(0x47956311, 0x2216740a)

namespace MaxSDK
{
	/** TexmapThreadSafe provides any texture plugin with thread safe support information in 3ds Max 2015. 
	The Max system will use multiple threads to accelerate texmap render if the Texmap function is thread safe.
	This will improve the performance of baking textures for display in the viewport.
	A typical implementation would look like this, and can be added to the constructor of the plugin class:
	\code
	MaxSDK::TexmapThreadSafe* pTexmapThreadSafe = static_cast<MaxSDK::TexmapThreadSafe*>(GetInterface(ITEXMAP_THREAD_SAFE));
	DbgAssert(pTexmapThreadSafe != nullptr);
	pTexmapThreadSafe->SetFunctionFlag(MaxSDK::TexmapThreadSafe::FunctionEvalMono, true);
	pTexmapThreadSafe->SetFunctionFlag(MaxSDK::TexmapThreadSafe::FunctionEvalColor, true);
	\endcode
	\sa ~{ Thread Safety }~
	*/
	class TexmapThreadSafe : public BaseInterface
	{
	public:

		/** This enum type specifies a Texmap function.*/
		enum TexmapFunction
		{
			FunctionEvalColor = 1,			/*!< FunctionEvalColor EvalColor(ShadeContext& sc) of class Texmap */
			FunctionEvalMono = 1 << 1,		/*!< FunctionEvalMono  EvalMono(ShadeContext& sc) of class Texmap*/
		};

		/** Gets the interface ID of TexmapThreadSafe.
		*/
		virtual Interface_ID GetID() { return ITEXMAP_THREAD_SAFE; }

		/** Sets a thread safe flag for a texmap function.
		This requires the specified TexmapFunction to be implemented as thread safe between different instances of Texmap.
		\param[in] function the specified Texmap function.
		\param[in] bThreadSafe whether the specific function is thread safe.
		*/
		virtual void SetFunctionFlag(TexmapFunction function, bool bThreadSafe) = 0;

		/** Returns the specified TexmapFunction's thread safe flag.
		\param[in] function the specified TexmapFunction.
		\return thread safe flag. The default is false.
		*/
		virtual bool GetFunctionFlag(TexmapFunction function) = 0;

		/** Specifies the function of the texmap with strict thread safe flag.
		This requires the specified Texmap function to be thread safe within one instance of Texmap. 
		This is stricter than SetFunctionFlag().
		\param[in] function Texmap function.
		\param[in] bThreadSafe whether the specific function is thread safe.
		*/
		virtual void SetFunctionStrictFlag(TexmapFunction function, bool bThreadSafe) = 0;
		/** Returns the specified Texmap function's thread safe flag.
		\param[in] function the specified Texmap function.
		\return thread safe flag. The default is false.
		*/
		virtual bool GetFunctionStrictFlag(TexmapFunction function) = 0;
	};
}
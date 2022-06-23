//
// Copyright 2013 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//
//

#pragma once

#include "Fragment.h"
#include "../ViewSystem/EvaluationContext.h"

namespace MaxSDK { namespace Graphics {

	class SmartHandle;

	/** Base class of all viewport fragment.
	*/
	class ViewFragment : public Fragment
	{
	public:
		/** Constructor.
		*/
		MaxGraphicsObjectAPI ViewFragment();
		/** Destructor.
		*/
		MaxGraphicsObjectAPI virtual ~ViewFragment();

		/** Get a input of ref object type using SmartHandle
		\param[in] handle the smart handle to receive the ref object input
		\param[in] inputIndex the index of the input
		\return true if we manage to get this ref object input or false otherwise
		*/
		template <typename T>
		bool GetSmartHandleInput(T& handle, size_t inputIndex)
		{
			Output* pInputSource = GetInputSource(inputIndex);
			if (NULL != pInputSource && T::ClassID() == pInputSource->GetTypeID())
			{
				handle.SetPointer(pInputSource->GetVar().AsRefObject());
				return true;
			}

			return false;
		}

		/** Set a ref object to the output
		\param[in] handle the smart handle that has the ref object
		\param[in] outputIndex the index of the output
		\return true if we manage to set this ref object to the output or false otherwise
		*/
		template <typename T>
		bool SetSmartHandleOutput(const T& handle, size_t outputIndex)
		{
			Output& output = GetOutput(outputIndex);
			if (T::ClassID() == output.GetTypeID())
			{
				return SetOutputVariable(outputIndex, Variable(handle.GetPointer()));
			}

			return false;
		}

	protected:
		/** Do evaluation. Perform necessary computation in this function, and set value to its 
		output. Please note this function cannot be directly called, it can only be called 
		indirectly by calling Fragment::Evaluate(). 
		\param[in] evaluationContext context info for evaluation.
		\return true if succeeded, false otherwise. 
		*/
		virtual bool DoEvaluate(EvaluationContext* evaluationContext) = 0;

	private:
		/** Perform input parameter conversion from the void* to EvaluationContext*, then call the 
		DoEvaluate(EvaluationContext* evaluationContext) function. 
		\param[in] evaluationContext context info for evaluation.
		\return true if the called function DoEvaluate(EvaluationContext* evaluationContext) succees, false otherwise. 
		*/
		MaxGraphicsObjectAPI virtual bool DoEvaluate(void* evaluationContext);
	};

} } // namespace MaxSDK::Graphics

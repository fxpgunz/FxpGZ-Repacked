//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// AUTHOR: Nicolas Desjardins
// DATE: 2007-05-24
//***************************************************************************/

#pragma once

#include "noncopyable.h"

namespace MaxSDK
{

	/*! \brief Resets a variable when the object goes out of scope.
	*
	* Following the principle of resource acquisition is allocation, this class,
	* which should always be allocated on the stack, will set a variable to the
	* given value, then set the variable back to its original value when it goes
	* out of score.  This is safer than setting things by hand since the destructor
	* will still be executed if an exception is thrown from the guarded block.
	* An example given a member variable mRecursionLocked:
	* \code
	* void SomeClass::SomeFunction() {
	*   if (!mRecursionLocked) {
	*     VariableGuard<bool> guard(mRecursionLocked, true);
	*     // some possibly recursive code...
	*   }
	* } 
	* \endcode
	*/
	template<typename T> class VariableGuard : MaxSDK::Util::Noncopyable
	{
	public:

		/*! \brief Guards the given variable, restoring its value when this object instance goes out of scope.
		*
		* \param variable - the variable to be guarded (whose value needs to be restored)
		*/
		VariableGuard(T& variable) :
				mVariable(variable),
					mOriginalValue(variable)
				{ }

				/*! \brief Guards the given variable, and sets it to the specified value
				*
				* The variable is restored to its original value when this object instance goes out of scope.
				* \param variable - the variable to be guarded (whose value needs to be restored)
				* \param value - the new value the variable should be set to right away
				*/
				VariableGuard(T& variable, T value) :
				mVariable(variable),
					mOriginalValue(variable)
				{
					SetCurrentValue(value);
				}

				/*! \brief Restores the variable to its original value
				*/
				~VariableGuard()
				{
					mVariable = mOriginalValue;
				}

				/*! \brief Retrieves the current value of the guarded variable 
				*/
				T GetCurrentValue() const
				{
					return mVariable;
				}

				/*! \brief Sets the variable to the specified value.  
				*
				* Upon this guard's destruction, the variable will be reset to the original 
				* value found during the guard's construction.
				* \param value - the new value the variable should be set to
				*/
				void SetCurrentValue(T value)
				{
					mVariable = value;
				}

	private:
		T& mVariable;
		T mOriginalValue;
	};

}


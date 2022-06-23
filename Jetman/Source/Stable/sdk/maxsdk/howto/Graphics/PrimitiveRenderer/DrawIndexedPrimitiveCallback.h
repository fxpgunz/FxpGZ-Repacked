/**********************************************************************
 *<
	FILE: DrawIndexedPrimitiveCallback.h

	DESCRIPTION:	Includes for Plugins
					The base class of this class is BasePrimitiveDisplayCallback. This class implements
					the methods to use DisplayCallback to draw IndexPrimitive with SolidColorMaterial.
					It also provides implementations of
					void SetSolidColorMaterial()
					void DoDisplay(..)
	CREATED BY:     

	HISTORY:

	*>	Copyright (c) 2011, All Rights Reserved.
	**********************************************************************/
#ifndef __DrawIndexedPrimitiveCallback__H
#define __DrawIndexedPrimitiveCallback__H

#include "BasePrimitiveDisplayCallback.h"
#include <Graphics/SolidColorMaterialHandle.h>
#include <Graphics/MaterialRequiredStreams.h>

namespace MaxSDK { namespace Graphics {

class DrawIndexedPrimitiveCallback : public BasePrimitiveDisplayCallback
{
public:
	DrawIndexedPrimitiveCallback();	
	virtual ~DrawIndexedPrimitiveCallback();

protected:
	/* From BasePrimitiveDisplayCallback.
	\param[in] t Current time
	\param[in] renderer Instance of IPrimitiveRender. User can use this class 
		to draw geometries, markers, and texts.
	\param[in] displayContext The display context. 
	*/
	virtual void DoDisplay(
		TimeValue t, 
		IPrimitiveRenderer& renderer, 
		const DisplayCallbackContext& displayContext);
	/* Provide implementations of method to add solid color Material.	
	\param[in] renderer Instance of IPrimitiveRender. User can use this class 
		to draw geometries, markers, and texts.	
	*/
	void SetSolidColorMaterial(IPrimitiveRenderer& renderer);
	
	// Attributes
	SolidColorMaterialHandle hSolidColorMaterial;	
};

} } // namespaces
#endif
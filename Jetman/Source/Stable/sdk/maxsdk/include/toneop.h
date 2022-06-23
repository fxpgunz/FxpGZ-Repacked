/**********************************************************************
 *<
	FILE: toneop.h

	DESCRIPTION: Definitions for tone operator. Tone operators are used
	             to map high dynamic range images into RGB. Usually they
	             are used with physical energy values.

	CREATED BY: Cleve Ard

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#pragma once

#include "Rendering/ToneOperator.h"

/*=====================================================================
 * Tone Operator Core Interface class
 *===================================================================*/

// This class is used to get access to the tone operator and
// its UI.
#define TONE_OPERATOR_INTERFACE	Interface_ID(0x1563269c, 0x7ec41d84)

/*! \sa  Class FPStaticInteface,  Class ToneOperator\n\n
\par Description:
This class allows plug-ins and the scripter to get access to the tone operator
assigned to a scene. You can get a pointer to the interface using the global
interface pointer in this manner:\n\n
<b>ToneOperatorInterace* toneOpInt =
static_cast\<ToneOperatorInterface*\>( GetCOREInterface(TONE_OPERATOR_INTERFACE));</b>\n\n
If the return value is NULL, the running version of 3ds Max doesn't support
tone operators. If the return value is not NULL, you can use these methods to
perform some scene management.  */
class ToneOperatorInterface : public FPStaticInterface {
public:
	typedef void (*ToneChangeCallback)(
		ToneOperator*  newOp,
		ToneOperator*  oldOp,
		void*          param
	);

	// Get and Set the tone operator in the scene
	/*! \remarks This method returns the current tone operator assigned to a
	scene. If no tone operator is assigned NULL is returned. */
	virtual ToneOperator* GetToneOperator() const = 0;
	/*! \remarks This method assigns a tone operator to the scene. To remove a
	tone operator, assign NULL. When a new tone operator is assigned the
	current operator is removed automatically. */
	virtual void SetToneOperator(ToneOperator* op) = 0;

	/*! \remarks This method registers a callback that is called when the tone
	operator is changed. Note the definition of ToneChangeCallback:\n\n
	<b>typedef void (*ToneChangeCallback)(ToneOperator* newOp, ToneOperator*
	oldOp, void* param);</b>
	\par Parameters:
	<b>ToneChangeCallback callback</b>\n\n
	The callback to register.\n\n
	<b>void* param</b>\n\n
	This parameter is passed to the callback function as the parameter argument
	when it is called. */
	virtual void RegisterToneOperatorChangeNotification(
		ToneChangeCallback   callback,
		void*                param
	) = 0;
	/*! \remarks This method un-registers a callback that was registered by
	RegisterToneOperatorChangeNotification. Note the definition of
	ToneChangeCallback:\n\n
	<b>typedef void (*ToneChangeCallback)(ToneOperator* newOp, ToneOperator*
	oldOp, void* param);</b>
	\par Parameters:
	<b>ToneChangeCallback callback</b>\n\n
	The callback to un-register.\n\n
	<b>void* param</b>\n\n
	This parameter is passed to the callback function as the parameter argument
	when it is called. */
	virtual void UnRegisterToneOperatorChangeNotification(
		ToneChangeCallback   callback,
		void*                param
	) = 0;
};


/*=====================================================================
 * class IToneOperatorExtension
 *
 * This class is an extension to allow tone operators to work more
 * closely with radiosity engines. The interface supports tone operator
 * that can choose to display illuminance (irradiance) or luminance
 * (radiance). This is especially useful for performing lighting analysis
 * through special-purpose tone operators.
 *===================================================================*/
#define ITONEOPERATOR_EXTENSION_INTERFACE Interface_ID(0x512b3541, 0x1c413aad)

class IToneOperatorExtension : public BaseInterface {

public:

    enum Quantity {
        kQuantity_Illuminance = 0,
        kQuantity_Luminance = 1
    };

    virtual Quantity GetUsedQuantity() const = 0;
    virtual void SetUsedQuantity(Quantity q) = 0;

    // -- from BaseInterface
    virtual Interface_ID GetID() { return ITONEOPERATOR_EXTENSION_INTERFACE; }
    
};


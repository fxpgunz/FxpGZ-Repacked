//**************************************************************************/
// Copyright (c) 2014 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/

#pragma once
#include "iFnPub.h"
#include "GetCOREInterface.h"

class IParamPubMgr;

#define I_PARAMETER_PUBLISH_MANAGER   Interface_ID(0x416d0201, 0xaa0fc23)
inline IParamPubMgr* GetParamPubMgr() { return (IParamPubMgr*)GetCOREInterface(I_PARAMETER_PUBLISH_MANAGER); }

//==============================================================================
// class IParamPubMgr
//    parameter publish manager interface 
/*! \sa  Class FPStaticInterface,  Class Control, Class ReferenceTarget\n\n
\par Description:
This class represents the interface that provides general access to the
parameter publish functions. You can obtain a pointer to the Parameter Publish
Manager interface using; 
\code
	IParamPubMgr* GetParamPubMgr();
\endcode. 
All methods of this class are Implemented by the System.  
*/
//==============================================================================
class IParamPubMgr : public FPStaticInterface 
{
public:
	// function IDs 
	enum { startParamPublish,
		   addParameter,
		}; 

	/*! \remarks This method will launch the parameter publish UI mode. */
	virtual void StartParamPublish()=0;
	/*! \remarks This method will add parameter into ready-to-publish parameter list.
	Only publishable parameters will be added successfully, and they'll be published
	when StartParamPublish() is called.
	\par Parameters:
	<b>ReferenceTarget* parent</b>\n\n
	Parent of the selected animatable parameter in track view.\n\n
	<b>int subNum</b>\n\n
	SubNum of the selected animatable parameter in track view.
	\par Description:
	The method can be used by MaxScript code like:
	\code
		local anim = trackviews.current.getParentOfSelected i
		local subnum = trackviews.current.getSelectedSubNum i
		paramPublishMgr.addParameterBySubNum anim subnum			
	\endcode.
	parent and subNum is obtained from track view, similar to Parameter Collector. 
	Then StartParamPublish() will publish parameter added by AddParameter().
	*/
	virtual void AddParameter(ReferenceTarget* parent, int subNum)=0;
}; 
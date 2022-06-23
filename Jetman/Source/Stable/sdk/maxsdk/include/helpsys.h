// Copyright 2010 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.

#pragma once
#include "maxheap.h"
#include <WTypes.h>
#include "ifnpub.h"
#include "GetCOREInterface.h"
#include "contextids.h"

//! \brief IHelpSystem interface ID
#define IID_IHelpSystem Interface_ID(0x3a4c02c3, 0x4980c58)

namespace MaxSDK {
/*! \brief This interface provides access to the 3ds Max Help system.
 * To display help related to the product, use the MaxSDK::IHelpSystem::ShowProductHelpForTopic() function.
 * The 3ds Max product help can either be displayed from online servers (default behavior) or from a local path.
 * Can be use to get and set help location preferences.
 * Maxscript help is always accessed online.
 * This is not meant to control "Additional Help" (help for installed third-party plug-ins and add-ons).
\code
// Set the help location to be online
MaxSDK::IHelpSystem * helpSys = MaxSDK::IHelpSystem::GetInstance();
helpSys->SeProducttHelpLocation( MaxSDK::IHelpSystem::OnlineHelp );
\endcode
 */
class IHelpSystem : public FPStaticInterface
{
public:

	/*! \brief Possible 3ds Max product help locations.
	 */
	enum ProductHelpLocationType {
		//! \brief Help is displayed from Autodesk's servers. Actual online location can not be changed.
		OnlineHelp
		//! \brief Help is displayed from a local path. The actual path can be controlled using MaxSDK::IHelpSystem::SetLocalProductHelpPath().
		, LocalHelp
	};

	//! \brief Returns the current 3ds Max product help location method.
	virtual ProductHelpLocationType ProductHelpLocation() const = 0;
	//! \brief Sets the product help location to be Autodesk web site or local network.  Saves the value to the ini file.
	virtual void SeProducttHelpLocation( ProductHelpLocationType location = OnlineHelp ) = 0;

	//! \brief Get the current local product help path, that is used when MaxSDK::IHelpSystem::ProductHelpLocation() is OnlineHelp.
	virtual const MCHAR * LocalProductHelpPath() const = 0;
	//! \brief Sets the current local help location, that is used when MaxSDK::IHelpSystem::ProductHelpLocation() is LocalHelp.  Saves the value to the ini file.
	virtual void SetLocalProductHelpPath( const MCHAR * path ) = 0;
	//! \brief Get the default local product help path to the default local help path setting.
	virtual const MCHAR * DefaultLocalProductHelpPath() const = 0;

	//! \brief Shows the 3ds Max Product Help. The current settings are used to determine if help is displayed from an online or a local location.
	// \return Returns true if the product help was displayed
	virtual bool ShowProductHelp() const = 0;

	//! \brief Shows the 3ds Max Product Help for a specific topic. The current settings are used to determine if help is displayed from an online or a local location.
	// \param contextId Can be one of the values defined in contextids.h
	// \return Returns true if the product help was displayed
	virtual bool ShowProductHelpForTopic( int contextId ) const = 0;

	//! \brief Shows the 3ds Max Product Help for a specific topic provided as a string (if some help system is available).	
	// \remarks Reserved for internal use.
	// \param topic The topic string that to uniquely identify the desired topic.
	// \return Returns true if the product help was displayed
	virtual bool ShowProductHelpForTopic( const MCHAR * topic ) const = 0;

	//! \brief Shows the 3ds Max Product Help for a search term (if some help system is available).	
	// \param searchFor String to search for in product help. When searchFor is NULL the table of contents is shown.
	// \return Returns true if the product help was displayed
	virtual bool ShowProductHelpSearch(const MCHAR * searchFor) const = 0;

	//! \brief Shows the online Maxscript documentation.
	// \param searchFor String to search for in Maxscript help. When searchFor is NULL the table of contents is shown.
	// \return Returns true if the maxscript help was displayed
	virtual bool ShowMaxscriptHelp( const MCHAR * searchFor = nullptr ) const = 0;

	//! \brief Shows the online Python documentation.
	// \param searchFor String to search for in Python help. When searchFor is NULL the table of contents is shown.
	// \return Returns true if the python help was displayed
	virtual bool ShowPythonHelp(const MCHAR * searchFor = nullptr) const = 0;

	//! \brief Opens a chm file.
	// \remarks 3rd party developers can use this API to open their chm help and search in it. In the event the indexString is not found, the chm path
	// should include a default topic to jump to.  For example: "3dsmax-2017-pythonapi-help.chm::/developer/about_the_3ds_max_python_api.html"
	// \param path Pointer to a path to a chm file. The chm file will be opened. 
	// \param indexString String to search for in the help file's index. When indexString is NULL, empty, or not found in the index, the table of contents is shown.
	// \return Returns true if the help file was displayed
	virtual bool OpenChmFile(const MCHAR * path, const MCHAR * indexString = nullptr) const = 0;

	//! \brief Returns the IHelpSystem interface 
	static IHelpSystem * GetInstance() {
		return static_cast<IHelpSystem*>(GetCOREInterface(IID_IHelpSystem));
	}
};

}



//
// Copyright 2016 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
//

#pragma once

#include "maxtypes.h"
#include "strclass.h"
#include "export.h"

class ClassDesc;
class ReferenceTarget;

/*!
\brief Class OperationDesc
This class contains data used to report usage analytics through theHold.Accept method and the ADP
mechanism. Each of the data members value are included in the way point report.
*/
class CoreExport OperationDesc : public MaxHeapOperators
{
public:
	//! internal use only. Used internally to interface with legacy code
	explicit OperationDesc(int commandResId);
	explicit OperationDesc(const MCHAR* commandName);

	// new interface
	//! Constructor to report only a commandResId and a module name to ADP.
	//! \param commandResId Resource string ID identifying the command name string. Must not be -1.
	//! \param commandName This parameter must not be null should be a localized string.
	//! \param moduleInst HINSTANCE of the dll or exe which instantiated the OperationDesc and initiated the command.
	explicit OperationDesc(int commandResId, const MCHAR* commandName, HINSTANCE moduleInst);
	//! Constructor to report the impacted object class ids(derived from the ReferenceTarget), a commandResId
	//!     and a module name to ADP.
	//! \param commandResId Resource string ID identifying the command name string. Must not be -1.
	//! \param commandName This parameter must not be null should be a localized string.
	//! \param moduleInst HINSTANCE of the dll or exe which instantiated the OperationDesc and initiated the command.
	//! \param refTarget ReferenceTarget of the scene entity currently being modified or created by the Accept
	//!     invocation and from which the Class_ID and the SClass_ID will be extracted and used as such in the
	//!     way point.
	explicit OperationDesc(int commandResId, const MCHAR* commandName, HINSTANCE moduleInst, ReferenceTarget* refTarget);
	//! Constructor to report the impacted object class ids (derived from the ClassDesc), a commandResId and 
	//!     a module name to ADP.
	//! \param commandResId Resource string ID identifying the command name string. Must not be -1.
	//! \param commandName This parameter must not be null should be a localized string.
	//! \param moduleInst HINSTANCE of the dll or exe which instantiated the OperationDesc and initiated the command.
	//! \param classDesc Class Descriptor of the scene entity currently being modified or created by the Accept
	//!     invocation and from which the Class_ID and the SClass_ID will be extracted and used as such in the
	//!     way point.
	explicit OperationDesc(int commandResId, const MCHAR* commandName, HINSTANCE moduleInst, ClassDesc* classDesc);
	//! Constructor to report the impacted object class ids, a commandResId and a module name to ADP.
	//! \param commandResId Resource string ID identifying the command name string. Must not be -1.
	//! \param commandName This parameter must not be null should be a localized string.
	//! \param moduleInst HINSTANCE of the dll or exe which instantiated the OperationDesc and initiated the command.
	//! \param scId Super Class Id of the scene entity currently being modified or created by the Accept invocation.
	//!     Used as such in the way point.
	//! \param cId Class Id of the scene entity currently being modified or created by the Accept invocation.
	//!     Used as such in the way point.
	explicit OperationDesc(int commandResId, const MCHAR* commandName, HINSTANCE moduleInst, const SClass_ID& scId, const Class_ID& cId);
	//! Constructor to report only an arbitrary details string, a commandResId and a module name to ADP.
	//! \param commandResId Resource string ID identifying the command name string. Must not be -1.
	//! \param commandName This parameter must not be null should be a localized string.
	//! \param moduleInst HINSTANCE of the dll or exe which instantiated the OperationDesc and initiated the command.
	//! \param commandDetails Gives additional information on the command when other information like class 
	//!	    ids, ClassDesc or ReferenceTarget are not readily available to the user.
	//!     For instance: For a CREATE command, the type of object created. 
	//!     commandDetails must NOT be a localized string.
	explicit OperationDesc(int commandResId, const MCHAR* commandName, HINSTANCE moduleInst, MSTR commandDetails);

	//| \return Resource string identifier of the command. 
	//|     Paired with the module name retrieved from the moduleInst allow to uniquely identify the 
	//|     operation\command in Max.
	//|     -1 means unknown and invalid. -1 occurs only when OperationDesc is used internally in Max.
	int  getCommandResId() const { return commandResId; }

	//! \return Command name string. Likely to be localized.
	//!     Empty only when OperationDesc is used internally in Max.
	const MSTR getCommandName() const { return commandName; }

	//| \return HINSTANCE of the dll or exe which instantiated the OperationDesc and initiated the command. 
	//|     The module name retrieve from the HINSTANCE, paired with the commandResId, allow to uniquely 
	//|     identify a operation\command in Max.
	HINSTANCE getModuleInstance() const { return moduleInst; }

	//! \return Super class id of the object influenced by the command. The one created, modified or deleted.
	//!     May be 0.
	SClass_ID getCommandDetailsSid() const { return commandDetailsSid; }
	//! \return Class id of the object impacted by the command. The one created, modified or deleted.
	//!     May be invalid.
	const Class_ID getCommandDetailsCid() const { return commandDetailsCid; }

	//! \return Additional command details string that have been provided at construction by the user. 
	//!     Shall not be localized. Can be anything. May be empty.
	const MSTR getCommandDetails() const { return commandDetails; }

private:
	const int  commandResId;
	const MSTR commandName;
	const HINSTANCE moduleInst;
#pragma warning(disable: 4251)
	const SClass_ID commandDetailsSid;
	const Class_ID commandDetailsCid;
#pragma warning(default: 4251)
	const MSTR commandDetails;
};

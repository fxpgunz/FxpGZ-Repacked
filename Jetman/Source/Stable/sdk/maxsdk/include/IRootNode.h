//
// Copyright 2016 Autodesk, Inc.  All rights reserved. 
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//
//
#pragma once

#include "baseInterface.h"

// forward declaration
class INode;

#define IROOTNODE_INTERFACE_ID Interface_ID(0x3a0a3ad9, 0xb2e6dd7)

// IRootNode interface
/*! 
\par Description:
The IRootNode interface is the interface to root nodes in the scene. There is a root node 
at the top of the scene, and each scene xref has a root node at the top of its tree. 
Each root node holds its immediate xref scene children root nodes.
All methods of this class are implemented by the system.
*/
class IRootNode : public BaseInterface
{
public:
	IRootNode() {}
	virtual ~IRootNode() {}

	virtual Interface_ID GetID() { return IROOTNODE_INTERFACE_ID; }

	/*! \remarks Returns the root node of the tree for the specified XRef.
	This method, when called on a root node, will access the various
	XRef scenes. Note that these can be nested so calling this on the
	root node of the scene will return the root node of one of the
	scene XRefs. Calling it on the root node of the scene XRef will get
	the root node of a nested XRef and so on. Note that this is not the
	parent of the XRef (see <b>GetXRefParent()</b> below).
	\param i The zero based index of the XRef.
	*/
	virtual INode *GetXRefTree(int i) = 0;
	/*! \remarks Returns the parent node. This is the
	node in the scene (if any) which the scene XRef is linked to
	through the special bind function in the scene XRef dialog.
	*/
	virtual INode* GetXRefParent() const = 0;
	/*! \remarks Returns the number of scene xrefs. 
	*/
	virtual int GetXRefNodeCount() = 0;
	/*! \remarks Returns the state of flags for the specified XRef.
	\param i The zero based index of the XRef whose flags are returned.
	\return See \ref xrefFlagBits.
	*/
	virtual DWORD GetXRefFlags(int i) = 0;
	/*! \remarks Returns the owning node. This is the root node that holds
	this root node as a nested XRef. The owning node of the top scene root
	node is null.
	*/
	virtual INode* GetXRefOwner() const = 0;

	/*! \remarks Returns the xref index of this xref in the owning node. 
	*/
	virtual int GetXRefOwnerIndex() const = 0;

};


/**********************************************************************
 *<
	FILE: CreaseSetContainer.h

	DESCRIPTION:  Crease Set data container for OpenSubDiv etc.

	CREATED BY: Tom Hudson

	HISTORY: created May 1, 2014

 *>	Copyright (c) 2014 Autodesk Inc., All Rights Reserved.
 **********************************************************************/

#pragma once

#include "maxtypes.h"

// Crease Set SuperClass ID
// This class is not saved/loaded -- Completely transient
#define CREASE_SET_CONTAINER_CLASS_ID 0xbac2b2aa

// Crease Set Manager class ID
#define CREASE_SET_MANAGER_CLASS_ID Class_ID(0x7251bc12, 0x623db898)

// Crease Set Interface ID
#define CreaseSetInterfaceID Interface_ID(0x8f329cb1, 0xec19c003)

// Crease set types
enum { CREASE_SET_TYPE_VERTEX=1, CREASE_SET_TYPE_EDGE };

/*! \remarks CreaseSetContainer interface class

In 3ds Max, subdivision surface creasing is set up around the concept of "Crease Sets" -- Groups
of edges or vertices which share common crease values.  Establishing groups which share common
crease values makes it easier for users to manage them and control creasing of various parts of
their geometry in unison.

The Crease Explorer allows users to view all Crease Sets in all objects and manipulate them. 
For convenience in managing them, a hierarchy is created in which each Crease Set resides in a
"Crease Set Container", which may contain any number of Crease Sets.  A Crease Set Container
can be anything in Max which defines a Crease Set.  Presently, this includes the CreaseSet and
Crease modifiers, as well as the SubdivBox primitive.

Any plugin which defines Crease Sets should derive from CreaseSetContainer, implement all methods
of the CreaseSetContainer class and register each instance with the Crease Set Manager using the
CreaseSetManager::RegisterCreaseSetContainer method.  This will allow the system to access the
Crease Sets it contains and display them in the Crease Explorer for the user to examine and/or alter.

When an instance of a CreaseSetContainer is destroyed or removed from the scene, it should 
unregister itself by calling CreaseSetManager::UnregisterCreaseSetContainer.

When Crease Sets are created or destroyed in a CreaseSetContainer, the CreaseSetContainer should
notify the CreaseSetManager via CreaseSetManager::NotifyStructureChanged so that the Crease Explorer,
if active, may update to reflect the change.

Any time an existing Crease Set changes in any way, the CreaseSetContainer should notify the
CreaseSetManager via CreaseSetManager::NotifyComponentChanged.

A CreaseSetContainer exposes the number of Crease Sets it contains (see CreaseSetContainer::GetNumberOfCreaseSets).
Each Crease Set may be queried for its name, as well as its container's name (see CreaseSetContainer::GetCreaseName
and CreaseSetContainer::GetCreaseContainerName).  Usually, all Crease Sets within a CreaseSetContainer
will share the same container name (for example, a CreaseSet modifier in Max returns its name whenever
any of the Crease Sets it contains are queried for their container's name), but this is up to the
CreaseSetContainer.  The Crease Set Manager, which itself is a specialized CreaseSetContainer, wraps all
CreaseSetContainers in the system and will return the proper CreaseSetContainer name for any Crease Set
in the scene.

For an example of how to implement a CreaseSetContainer, see the SubdivBox plugin in MaxSDK/Objects/PolyObjects/

*/

class CreaseSetContainer : public BaseInterface {
private:
public:
	/*! \remarks Get the ReferenceTarget for this Crease Set Container
    \returns ReferenceTarget that represents this Crease Set Container 
	  */
	virtual ReferenceTarget *GetCreaseSetContainerReferenceTarget() = 0;		// This should be an Object or Modifier (or other ReferenceTarget) so others can reference us
	/*! \remarks Get the number of crease sets in this container
    \returns The number of crease sets
	  */
	virtual int GetNumberOfCreaseSets() = 0;
	/*! \remarks Get the name of the crease set container for a given crease set.  Usually the same name for all sets
	contained by a given container (i.e. For Max's Crease Set Modifier, the container name for all crease sets is the
	modifier name) but the Crease Set Manager, which allows accessing all crease sets in the system, will return the
	appropriate container name for each crease set.
	\param setIndex - Index of the crease set
	\returns The name of the crease set
	  */
	virtual MSTR GetCreaseContainerName(int setIndex) = 0;
	/*! \remarks Query whether the crease set container containing a given set can be renamed
	\param setIndex - Index of the crease set
    \returns true if the crease set container can be renamed
	  */
	virtual bool CanSetCreaseContainerName(int setIndex) { UNREFERENCED_PARAMETER(setIndex); return false; }
	/*! \remarks Set the name of a crease set container which contains a given set
	\param setIndex - Index of the crease set
	\param name - Pointer to new name string
	  */
	virtual void SetCreaseContainerName(int setIndex, const MCHAR *name) { UNREFERENCED_PARAMETER(setIndex); UNREFERENCED_PARAMETER(name); DbgAssert(0); }
	/*! \remarks Get the name of a given crease set
	\param setIndex - Index of the crease set 
    \returns The name of the crease set
	  */
	virtual MSTR GetCreaseName(int setIndex) = 0;
	/*! \remarks Query whether a given crease set can be renamed
	\param setIndex - Index of the crease set 
    \returns true if the crease set can be renamed
	  */
	virtual bool CanSetCreaseName(int setIndex) { UNREFERENCED_PARAMETER(setIndex); return false; }
	/*! \remarks Set the name of a given crease set
	\param setIndex - Index of the crease set  
	\param name - Pointer to new name string
	  */
	virtual void SetCreaseName(int setIndex, const MCHAR *name) { UNREFERENCED_PARAMETER(setIndex); UNREFERENCED_PARAMETER(name); DbgAssert(0); }
	/*! \remarks Get the type of a crease set
	\param setIndex - Index of the crease set 
    \returns The crease set type (CREASE_SET_TYPE_XXX, defined above)
	  */
	virtual int GetCreaseType(int setIndex) = 0;
	/*! \remarks Get the number of entities in a crease set
	\param setIndex - Index of the crease set 
    \returns The number of items in the set
	  */
	virtual int GetCreaseEntityCount(int setIndex) = 0;
	/*! \remarks Get the crease value for a given set
	\param setIndex - Index of the crease set
	\param t - The time at which to sample
	\param valid - Validity interval, updated by the query
    \returns The crease value, 0.0-1.0
	  */
	virtual float GetCreaseValue(int setIndex, TimeValue t, Interval &valid) = 0;
	/*! \remarks Query whether the crease value of a given set can be changed
	\param setIndex - Index of the crease set
    \returns true if the set value may be changed
	  */
	virtual bool CanSetCreaseValue(int setIndex) { UNREFERENCED_PARAMETER(setIndex); return false; }
	/*! \remarks Set the crease value of a crease set
	\param setIndex - Index of the crease set
	\param t - The time at which to set the crease value
	\param value - The crease value to set
	  */
	virtual void SetCreaseValue(int setIndex, TimeValue t, float value) { UNREFERENCED_PARAMETER(setIndex); UNREFERENCED_PARAMETER(t); UNREFERENCED_PARAMETER(value); DbgAssert(0); }
	/*! \remarks Get the color used to represent a given crease set
	\param setIndex - Index of the crease set
    \returns The color used for the crease set
	  */
	virtual const Color &GetCreaseColor(int setIndex) = 0;
	/*! \remarks Query whether the color of a given set can be changed
	\param setIndex - Index of the crease set
    \returns true if the color may be set
	  */
	virtual bool CanSetCreaseColor(int setIndex) { UNREFERENCED_PARAMETER(setIndex); return false; }
	/*! \remarks Sets the color of a given crease set
	\param setIndex - Index of the crease set
	\param color - The color to set
	  */
	virtual void SetCreaseColor(int setIndex, const Color &color) { UNREFERENCED_PARAMETER(setIndex); UNREFERENCED_PARAMETER(color); DbgAssert(0); }
	/*! \remarks Get a list of nodes affected by this container
	\param nodeList - An INodeTab that will receive the list of nodes 
	  */
	virtual void GetNodeList(INodeTab &nodeList)=0;
	/*! \remarks Get a list of nodes affected by the given crease set
	\param setIndex - Index of the crease set
	\param nodeList - An INodeTab that will receive the list of nodes 
	  */
	virtual void GetNodeList(int setIndex, INodeTab &nodeList)=0;
	/*! \remarks Container may contain animated Crease values.  Get the validity interval.
	\param t - The time at which to sample the validity
    \returns The validity interval for the given time
	  */
	virtual Interval GetCreaseSetContainerValidity(TimeValue t) { UNREFERENCED_PARAMETER(t); return FOREVER; };
	/*! \remarks Query whether the crease set color will be shown in the viewport
	\param setIndex - Index of the crease set
    \returns true if the visibility switch can be set
	  */
	virtual bool CanSetCreaseColorVisibility(int setIndex) { UNREFERENCED_PARAMETER(setIndex); return false; }
	/*! \remarks Get the crease set color visibility switch for a given crease set
	\param setIndex - Index of the crease set
    \returns The state of the visibility switch (true = visible)
	  */
	virtual bool GetCreaseColorVisibility(int setIndex) { UNREFERENCED_PARAMETER(setIndex); return false; }
	/*! \remarks Set the crease set color visibility switch for a given crease set
	\param setIndex - Index of the crease set
	\param state - The visibility state for the set (true = visible)
	  */
	virtual void SetCreaseColorVisibility(int setIndex, bool state) { UNREFERENCED_PARAMETER(setIndex); UNREFERENCED_PARAMETER(state); }
};

// Crease Set Manager
// 
// This is a specialized Crease Set that acts as a collection point for all regular Crease Sets in the system.
// It allows the Crease Explorer to always have access to all Crease Sets in the system.
//
// This class is transient -- it gets created automatically and is not saved in any files

class CreaseSetManager : public ReferenceTarget, public CreaseSetContainer {
public:
	// The Crease Set Manager is a specialized Crease Set Container -- implement those methods
	ReferenceTarget *GetCreaseSetContainerReferenceTarget() override =0;
	int GetNumberOfCreaseSets() override =0;
	MSTR GetCreaseContainerName(int setIndex) override =0;
	bool CanSetCreaseContainerName(int setIndex) override =0;
	void SetCreaseContainerName(int setIndex, const MCHAR *name) override =0;
	MSTR GetCreaseName(int setIndex) override =0;
	bool CanSetCreaseName(int index) override =0;
	void SetCreaseName(int setIndex, const MCHAR *name) override =0;
	int GetCreaseType(int setIndex) override =0;
	int GetCreaseEntityCount(int setIndex) override =0;
	float GetCreaseValue(int setIndex, TimeValue t, Interval &valid) override =0;
	bool CanSetCreaseValue(int setIndex) override =0;
	void SetCreaseValue(int setIndex, TimeValue t, float value) override =0;
	const Color &GetCreaseColor(int setIndex) override =0;
	bool CanSetCreaseColor(int setIndex) override =0;
	void SetCreaseColor(int setIndex, const Color &color) override =0;
	void GetNodeList(INodeTab &nodeList) override =0;
	void GetNodeList(int setIndex, INodeTab &nodeList) override =0;
	Interval GetCreaseSetContainerValidity(TimeValue t) override =0;
	bool CanSetCreaseColorVisibility(int setIndex) override =0;
	bool GetCreaseColorVisibility(int setIndex) override =0;
	void SetCreaseColorVisibility(int setIndex, bool state) override =0;

	// Crease Set Manager methods

	/*! \remarks Get the CreaseSetContainer for a given crease set
	\param setIndex - Index of the crease set
    \returns A pointer to the CreaseSetContainer
	  */
	virtual CreaseSetContainer *GetCreaseSetContainer(int setIndex)=0;
	/*! \remarks Register a CreaseSetContainer.  All CreaseSetContainers must register themselves so that the
	CreaseSetManager can access them and their contents.
	\param container - Pointer to the CreaseSetContainer being registered
	  */
	virtual void RegisterCreaseSetContainer(CreaseSetContainer *container)=0;
	/*! \remarks Unregister a CreaseSetContainer.  All CreaseSetContainers must unregister themselves when they
	are destroyed so that the CreaseSetManager will no longer try to access them.
	\param container - Pointer to the CreaseSetContainer being registered
	  */
	virtual void UnregisterCreaseSetContainer(CreaseSetContainer *container)=0;
	/*! \remarks Query whether a given CreaseSetContainer is registered
	\param container - Pointer to the CreaseSetContainer being registered
    \returns true if the container is registered
	  */
	virtual bool IsContainerRegistered(CreaseSetContainer *container)=0;
	
	// These methods let Crease Sets notify the Manager that they have changed

	/*! \remarks Notify the CreaseSetManaager that a container's structure has changed.  Crease Set Containers should
	call this when the number of crease sets they contain has changed.
	\param container - Pointer to the CreaseSetContainer that has changed
	  */
	virtual void NotifyStructureChanged(CreaseSetContainer *container)=0;
	/*! \remarks Notify the CreaseSetManaager that a set's content has changed.  Crease Set Containers should
	call this when the any aspect of the crease sets they contain has changed.
	\param container - Pointer to the CreaseSetContainer that has changed
	\param componentIndex - Index of the crease set, or -1 if more than one has changed
	  */
	virtual void NotifyComponentChanged(CreaseSetContainer *container, int componentIndex = -1)=0;
};

/*! \remarks Get a pointer to the system Crease Set Manager.
Plugins that work as Crease Set Containers will use this pointer to register/unregister themselves with the system so that the
crease sets they contain will appear in the Crease Explorer when appropriate.  Crease Set Containers will also use this pointer
to notify the system when their crease sets have changed in any way, or when the number of crease sets they contain has changed.
See CreaseSetManager, above.
\returns A pointer to the system Crease Set Manager.
*/
CoreExport CreaseSetManager *GetCreaseSetManager();

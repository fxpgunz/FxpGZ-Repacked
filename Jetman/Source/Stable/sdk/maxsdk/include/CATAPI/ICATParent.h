//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2009 Autodesk, Inc.  All rights reserved.
//  Copyright 2003 Character Animation Technologies.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../ifnpub.h"
#include "../INodeTab.h"

// Internal-only classes
class CATClipRoot;
class CATControl;

namespace CatAPI
{
	static const Interface_ID CATPARENT_INTERFACE_FP(0x686d5371, 0xaa835ec);
#define GetCATICATParentRootFPInterface(anim)  (anim ? ((CatAPI::ICATParent*)anim->GetInterface(CatAPI::CATPARENT_INTERFACE_FP)) : nullptr)

	class ILayerRoot;
	struct HIKDefinition;

	//! The colour modes define how CATs rig nodes will be coloured in the veiwport
	enum CATColourMode {
		COLOURMODE_CLASSIC,	//! original CAT colourings
		COLOURMODE_ACTIVE,	//! active layer colour shaded with effective weights
		// The following were never implemented, but might be a good idea some day
		COLOURMODE_BLEND,	//! resulting blend of all layer colours with effective weights (aka psycheldelic)
		COLOURMODE_SYMMETRY,	//! for showing symmetric bone pairs.
		COLOURMODE_STRETCHY,	//! to show stretch weights on stretchy bones.
	};

	//! The CATMode defines how the character will behave when interacted with.
	enum CATMode
	{
		//! SetupMode allows the user to modify the rig, changing dimensions, and adding/removing rig elements
		SETUPMODE,
		//! Normal (Animation) Mode allows a user to animate a rig, setting keyframes, scaling the character etc.
		//! It is not possible to enter normal mode without an animation layer
		NORMAL,
	};

	class IHub;

	/// ICATParent
	/// This is the interface class to the manager of the CAT Rig.
	/// Here you will find functions create/load/save a rig,
	/// and iterate its members.  A CATParent is not part of a rig,
	/// and not meant to be animated.  It is simply a storage vessel
	/// for the data common to the entire rig.
	/// Sample usage:
	/// \code
	/// INode* pCATParentNode = ...;
	/// Object* pCATParentObj = pCATParentNode->GetBaseObject();
	/// ICATParent* pCATParent = GetCATICATParentRootFPInterface(pCATParentObj);
	/// \endcode
	class ICATParent : public FPMixinInterface
	{
	public:

		/// Load a CAT Rig file (.rg3 or .rig).
		/// This function should only be called if a rig does not
		/// currently exist on the CATParent
		/// \param filename The rig definition to load
		/// \returns The INode of the CATParent after file load.
		///			 Loading an RG3 file will delete the current
		///			 CATParent and INode, and replace them with
		///			 the version from the file.  If you wish to continue
		///			 accessing the CATParent after calling this function
		///			 refresh your pointer with the returned value.
		/// \code
		/// INode* pCATParentNode = ...;
		/// Object* pCATParentObj = pCATParentNode->GetBaseObject();
		/// ICATParent* pCATParent = GetCATICATParentRootFPInterface(pCATParentObj);
		/// pCATParentNode = pCATParent->LoadRig("rigfile.rg3");
		/// // Refresh the CATParent pointer, our old one may have been deleted.
		/// pCATParentObj = pCATParentNode->GetBaseObject();
		/// pCATParent = GetCATICATParentRootFPInterface(pCATParentObj);
		/// \endcode
		virtual INode*	LoadRig(const MSTR& filename) = 0;
		/// Save the current rig to a rig definition file
		/// \param The file to save the rig too.  This can
		///			either be a .rig filename for an ascii file
		///			or a rg3 binary file
		/// \return TRUE if the rig was successfully saved.
		virtual BOOL	SaveRig(const MSTR& filename) = 0;


		/// Get the name of the CAT character.  This name is
		/// used as a prefix for all rig elements
		/// \return The name of the CAT character
		virtual MSTR	GetCATName() = 0;
		/// Set the name of the CAT charater.  This name is
		/// used as a prefix for all rig elements.  Note
		/// that setting this value will trigger a rename
		/// of all the rig elements
		/// \param name The new base name of the character
		virtual	void	SetCATName(const MSTR& name) = 0;

		/// Returns the rigs CATUnit scale.
		/// \return The rigs CATUnits scale
		virtual float	GetCATUnits() const = 0;
		/// Set the rigs CATUnits scale
		/// \param catunits the new CATUnits scale factor
		virtual void	SetCATUnits(float catunits) = 0;

		/// Returns the current mode of the character
		/// \return the current CATMode
		virtual CATMode	GetCATMode() const = 0;
		/// Sets the current mode of the character
		/// \params the current CATMode
		virtual void	SetCATMode(CATMode mode) = 0;

		/// Returns the current colour mode of the character
		/// \return the current colour mode
		virtual CATColourMode	GetColourMode() = 0;
		/// Sets the current colour mode of the character
		/// \param mode the new colour mode
		virtual void			SetColourMode(CATColourMode mode) = 0;

		/// Return the current axis which defines our length.  This is the
		/// axis that bones look down.
		/// \return 0 for the X axis, or 2 for the Z axis.
		virtual int	GetLengthAxis() = 0;
		/// Sets the current axis which defines our length.  This is the
		/// axis that bones look down.  It is not possible to change the
		/// Length axis after the rig has 
		/// \param lengthAxis 0 for the X axis, or 2 for the Z axis.
		virtual void	SetLengthAxis(int lengthAxis) = 0;

		/// Returns the aproximate location of the root of the character
		/// This value is used by the root node to find its transform
		/// \param t The time to find the transform at.
		/// \return The world space approximation of the root of the character
		virtual Matrix3 ApproxCharacterTransform(TimeValue t) = 0;

		/// Create a new hub (pelvis) for the character
		/// This function will only succeed if the CATParent has
		/// no character yet.
		/// \return TRUE if the hub was successfully created, else FALSE
		virtual BOOL		AddHub() = 0;

		/// Returns the controller for the root hub (pelvis) for the character.
		/// \return The controller of the root hub, or NULL
		virtual Control*	GetRootIHub() = 0;

		/// Find a member of the CAT rig by its address.
		/// \param Address the address of the rig node
		/// \return The inode of the addressed rig element, or NULL if not found
		virtual INode*	GetBoneByAddress(const MSTR& address) = 0;
		/// Get the address of this node (the CATParent node)
		/// \return The address of this controller.
		virtual MSTR	GetBoneAddress() = 0;

		/// TODO (Not sure what this does.
		virtual void	UpdateUserProps() = 0;

		/// Internal as well.  Find a way to hide this. 
		virtual int		GetVersion() = 0;
		virtual int		GetBuildNumber() = 0;

		/// Get the INode of this CATParent.
		/// \return The INode of this CATParent.  This should never be NULL
		virtual INode*	GetNode() = 0;

		/// Finds a complete list of all nodes in this rig, including extra bones and other stuff.
		/// \return A Tab filled with all rig elements.
		virtual INodeTab		GetCATRigNodes() = 0;
		/// Finds a complete list of all layer controls, including extra controllers
		/// \return A Tab filled with all layer controllers.
		virtual Tab <Control*>	GetCATRigLayerCtrls() = 0;

		/// Create a node to be placed under the CAT character.  This node can be used as a
		/// root for the character (for example, if for exporting the character to a game engine).
		virtual void	AddRootNode() = 0;
		/// Returns the root node, if it exists, else NULL
		/// \return the root node, if it exists, else NULL
		virtual INode*	GetRootNode() = 0;
		virtual void	RemoveRootNode() = 0;

		// Returns the class that controls the CAT layering system.  All
		// changes to the CAT Layers can be accessed through this class.
		virtual ILayerRoot* GetLayerRoot() = 0;

		/// Fills out the definition structure with the CAT rig structure.
		/// It is not guaranteed that all members of the class will be filled out.
		/// \param definition The definition to fill out
		/// \return TRUE if the definition was filled out correctly, FALSE if not.  Note this does NOT guarantee
		///			all the parameters are filled out, it will return TRUE if the CAT rig is valid.
		virtual bool FillHIKDefinition(HIKDefinition& definition) = 0;

		// Internal only.
		virtual CATClipRoot* GetCATLayerRoot() = 0;
		virtual INode* LoadClip(MSTR filename, int layerindex, int flags, TimeValue startT, CATControl* bodypart) = 0;
		virtual BOOL SaveClip(MSTR filename, int flags, Interval timerange, Interval layerrange, CATControl* bodypart) = 0;
	};
}
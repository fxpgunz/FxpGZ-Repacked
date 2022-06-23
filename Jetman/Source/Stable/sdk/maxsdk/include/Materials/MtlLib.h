//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Ref.h"
#include "MtlBaseList.h"
#include "Mtl.h"

class Texmap;
class ClassDesc;

typedef Mtl* MtlHandle;
typedef Texmap* TexmapHandle;

// Simple list of materials
/*! \sa  Template Class Tab, Class Mtl.\n\n
\par Description:
A simple list of materials. All methods of this class are implemented by the
system.\n\n
Note the following typedefs:\n\n
<b>typedef MtlBase* MtlBaseHandle;</b>\n\n
<b>typedef Mtl* MtlHandle;</b>\n\n
<b>typedef Texmap* TexmapHandle;</b>  */
class MtlList: public Tab<MtlHandle> {
	 public:
			/*! \remarks Adds the specified material to this list of materials.
			\par Parameters:
			<b>Mtl *m</b>\n\n
			The material to add.\n\n
			<b>BOOL checkUnique=TRUE</b>\n\n
			If TRUE this method checks to make sure the material is unique, and will
			only add it if so.
			\return  Nonzero if the material was added; otherwise zero. */
			CoreExport int AddMtl(Mtl *m, BOOL checkUnique=TRUE);
			/*! \remarks Finds the specified material in this material list and
			returns its index (0 through the number of materials in the library).
			Returns -1 if not found.
			\par Parameters:
			<b>Mtl *m</b>\n\n
			The material to find. */
			CoreExport int FindMtl(Mtl *m);
			/*! \remarks Finds the material whose name is passed in this material
			list and returns its index (0 through the number of materials in the
			library). Returns -1 if not found.
			\par Parameters:
			<b>MSTR\& name</b>\n\n
			The material to find. */
			CoreExport int FindMtlByName(MSTR& name);
			/*! \remarks Removes the material whose index is passed.
			\par Parameters:
			<b>int n</b>\n\n
			The index of the material to remove. */
			void RemoveEntry(int n) { Delete(n,1); }
			/*! \remarks Empties (clears) the material list. */
			void Empty() { Resize(0); }
	 };

// Materials library
/*! \sa  Class ReferenceTarget, Class MtlList.\n\n
\par Description:
A materials library. All methods of this class are implemented by the system.
 */
class MtlLib: public ReferenceTarget, public MtlList {
	 public:
			MtlLib() {}
			CoreExport SClass_ID SuperClassID(); 
			CoreExport Class_ID ClassID();
			/*! \remarks Removes all Mtls from the library and deletes all
			references. */
			CoreExport void DeleteAll();
			CoreExport void GetClassName(MSTR& s);
			/*! \remarks Destructor. All references are deleted. */
			CoreExport ~MtlLib();

			int NumSubs() { 
				 return Count(); 
				 }  
			Animatable* SubAnim(int i) { 
				 return (*this)[i]; 
				 }
			CoreExport MSTR SubAnimName(int i);
			/*! \remarks Removes the specified material from the library.
			\par Parameters:
			<b>Mtl *m</b>\n\n
			The material to remove. */
			CoreExport virtual void Remove(Mtl *m);
			/*! \remarks Adds the specified material from the library.
			\par Parameters:
			<b>Mtl *m</b>\n\n
			The material to add. */
			CoreExport virtual void Add(Mtl *m);

			// From ref
			RefResult AutoDelete() { return REF_SUCCEED; }
			CoreExport void DeleteThis();
			int NumRefs() { return Count();}
			RefTargetHandle GetReference(int i) { return (RefTargetHandle)(*this)[i];}
protected:
			CoreExport virtual void SetReference(int i, RefTargetHandle rtarg);
public:
			CoreExport RefTargetHandle Clone(RemapDir &remap);
			CoreExport RefResult NotifyRefChanged( const Interval& changeInt, RefTargetHandle hTarget, 
				 PartID& partID, RefMessage message, BOOL propagate );
			// IO
			CoreExport IOResult Save(ISave *isave);
				CoreExport IOResult Load(ILoad *iload);

	 };


/*! \sa  Class ReferenceTarget, Class MtlBaseList, Class MtlBase, Class Interface.\n\n
\par Description:
This class is a library of <b>MtlBase</b> entries. To get a list of the
currently loaded materials use:\n\n
<b> MtlBaseLib\& Interface::GetMaterialLibrary()</b>.\n\n
When you load a material library, you can enumerate all the materials that it
contains. <b>MtlBaseLib</b> is subclassed off of <b>MtlBaseList</b> which is a
<b>Tab\<MtlHandle\></b>, so for example, you can do the following to work with
each material:\n\n
 <b>for (int i=0; i\<mlib.Count(); i++) { DoSomething(mlib[i]); }</b>\n\n
For additional information on this class see the Advanced Topics section
~{ Materials, Textures and Maps }~. All methods of this class are implemented by the system.  */
class MtlBaseLib : public ReferenceTarget, public MtlBaseList
{
public:
	CoreExport MtlBaseLib();
	/*! \remarks Destructor. All references are deleted. */
	CoreExport virtual ~MtlBaseLib();

	// from Animitable
	CoreExport virtual SClass_ID SuperClassID();
	CoreExport virtual Class_ID ClassID();
	CoreExport virtual void GetClassName(MSTR& s);
	CoreExport virtual int NumSubs();
	CoreExport virtual Animatable* SubAnim(int i);
	CoreExport virtual MSTR SubAnimName(int i);

	// From ReferenceTarget
	CoreExport virtual RefResult AutoDelete();
	CoreExport virtual void DeleteThis();
	CoreExport virtual int NumRefs();
	CoreExport virtual RefTargetHandle GetReference(int i);
protected:
	CoreExport virtual void SetReference(int i, RefTargetHandle rtarg);
public:

	CoreExport virtual RefTargetHandle Clone(RemapDir &remap);
	CoreExport virtual RefResult NotifyRefChanged(
		const Interval& changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message, 
		BOOL propagate );

	// IO
	CoreExport virtual IOResult Save(ISave *isave);
	CoreExport virtual IOResult Load(ILoad *iload);

	/*! \remarks Removes all MtlBases from the library and deletes all
	references. */
	CoreExport virtual void DeleteAll();
	/*! \remarks Removes the specified MtlBase from the library.
	\par Parameters:
	<b>MtlBase *m</b>\n\n
	The MtlBase to remove. */
	CoreExport virtual void Remove(MtlBase *m);
	/*! \remarks Adds the specified MtlBase to the library.
	\par Parameters:
	<b>MtlBase *m</b>\n\n
	The MtlBase to add. */
	CoreExport virtual void Add(MtlBase *m);
	/*! \remarks Removes all duplicate MtlBases from the library. */
	CoreExport virtual void RemoveDuplicates();

protected:
	//! \brief Returns the count of materials under this category.
	int GetMaterialCount() const;
	//! \brief Get a pointer to the material with the given index.
	/*! 
	\pre i must be greater than or equal to 0 and less than the count of materials.
	\param[in] i - The index of the material.
	\return  The pointer to the material. */
	MtlBase* GetMaterial(int i) const;
	//! \brief Find the child material with the given name.
	/*! 
	\pre name must not be NULL and must points to a string which is NULL terminated.
	\param[in] name - The name of the child material to find. 
	\return The pointer to the found child material. */
	MtlBase* FindMaterial(const MCHAR* name) const;
	//! \brief Add a material to the children list of this category.
	/*! 
	\param[in] pMaterial - The material to add to this category. Can be NULL.
	\return  true if the material is succeesfully added to this category, false 
	otherwise. */
	bool AddMaterial(MtlBase* pMaterial);
	//! \brief Removes the specified MtlBase from this category.
	/*! 
	\param[in] pMaterial - The MtlBase to remove. Can be NULL. */
	void RemoveMaterial(MtlBase* pMaterial);

	void SetMaterialCount(int count);

	void* mpReserved;
};

CoreExport ClassDesc* GetMtlLibCD();
CoreExport ClassDesc* GetMtlBaseLibCD();

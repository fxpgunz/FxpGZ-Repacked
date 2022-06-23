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

#include "../MaxHeap.h"
#include "../Matrix3.h"
#include "../Box3.h"

class Mtl;
class Mesh;
class LightDesc;
class Object;
class ObjLightDesc;

/// \defgroup Render_Instance_Flags Render Instance Flags
//@{
#define INST_HIDE	  		(1<<0) //!< instance is hidden
#define INST_CLIP			(1<<1) //!< clip instance: ray tracers should skip it 
#define INST_BLUR			(1<<2) //!< secondary motion blur instance 
#define INST_RCV_SHADOWS	(1<<3) //!< instance receives shadows
#define INST_TM_NEGPARITY	(1<<4) //!< mesh is inside-out: need to reverse normals on-the-fly
#define INST_MTL_BYFACE     (1<<5) //!<instance's object supports mtl-by-face interface ( chkmtlapi.h)
//@}

/*! 
This class provides information about a single node being rendered. This
includes information such as the mesh of the object, its material, unique node
ID, object space bounding extents, number of lights affecting it, material
requirements, and normals and vertex coordinates in various spaces (object and
camera).
\par Data Members:
<b>ULONG flags;</b>\n\n
The flags that describe the properties of this instance. See \ref Render_Instance_Flags .\n\n
<b>Mtl *mtl;</b>\n\n
This is the material from the node.\n\n
<b>float wireSize;</b>\n\n
The wireframe size.\n\n
<b>Mesh *mesh;</b>\n\n
The mesh to be rendered. This is the result of
GeomObject::GetRenderMesh().\n\n
<b>float vis;</b>\n\n
Object visibility (between 0.0 and 1.0). This is the value the visibility track
evaluates to at a particular time.\n\n
<b>int nodeID;</b>\n\n
A unique ID associated with the node. It's unique within the scene during a
render.\n\n
<b>int objMotBlurFrame;</b>\n\n
This will be equal to <b>NO_MOTBLUR</b> for all non-blurred objects. For
blurred objects, it takes on the values (<b>0..nBlurFrames-1</b>) for the
successive blur-instances.\n\n
<b>int objBlurID;</b>\n\n
The purpose of this is to differentiate blur-instances generated from different
nodes. All the blur-instances for an object-motion-blurred object will have the
same <b>objBlurID</b>. This is as distinct from <b>nodeID</b>, which is
different for every instance. This makes it possible to easily avoid
intersecting a ray with all blur-instances for an object. If
RenderGlobalContext::IntersectWorld() is being used, then passing in the
<b>objBlurID</b> for the parameter <b>skipID</b> will have this effect.\n\n
The basic technique is this: When reflecting or refracting rays, and object
motion blur is enabled, choose sub-frame times randomly for the different rays
(effectively giving a coarse stochastic sampling of time).\n\n
<b>Matrix3 objToWorld;</b>\n\n
This matrix can be used to transform object coordinates to world
coordinates.\n\n
<b>Matrix3 objToCam;</b>\n\n
This matrix can be used to transform object coordinates to camera
coordinates.\n\n
<b>Matrix3 normalObjToCam;</b>\n\n
This matrix can be used for transforming surface normals from object space to
camera space.\n\n
<b>Matrix3 camToObj;</b>\n\n
This matrix can be used to transform camera coordinates to object
coordinates.\n\n
<b>Box3 obBox;</b>\n\n
The object space extents of the object being rendered.\n\n
<b>Point3 center;</b>\n\n
The object bounding sphere center (in camera coordinates)\n\n
<b>float radsq;</b>\n\n
The square of the bounding sphere's radius.  
\see  Class Mtl, Class LightDesc, Class INode,  Class Object,  Class Mesh, Class Matrix3,  Class Box3,  Class Point3, Class Interval.
*/
class RenderInstance: public MaxHeapOperators
{
public:
	ULONG flags;
	Mtl *mtl;       		// from inode, for convenience
	float wireSize;         // Mtl wireframe size
	Mesh *mesh;				// result of GetRenderMesh call
	float vis;				// Object visibility
	int nodeID;				// unique within scene during render- corresponds to ShadeContext::NodeID()
	int objMotBlurFrame;  	// Object motion blur sub frame (= NO_MOTBLUR for non-blurred objects)
	int objBlurID;		    // Blur instances for an object share a objBlurID value.
	Matrix3 objToWorld;		// transforms object coords to world coords
	Matrix3 objToCam;		// transforms object coords to cam coords
	Matrix3 normalObjToCam; // for transforming surface normals from obj to camera space
	Matrix3 camToObj;    	// transforms camera coords to object coords
	Box3 obBox;				// Object space extents
	Point3 center;			// Bounding sphere center (camera coords)
	float radsq;			// square of bounding sphere's radius

	/*! Destructor. */
	virtual ~RenderInstance() {}

	/*! Sets the specified flag(s) to the state passed.
	\par Parameters:
	<b>ULONG f</b>\n\n
	The flags to set. See \ref Render_Instance_Flags .\n\n
	<b>BOOL b</b>\n\n
	The state to set; TRUE for on; FALSE for off. */
	void SetFlag(ULONG f, BOOL b) { if (b) flags |= f; else flags &= ~f; }
	/*! Sets the specified flag(s) to on.
	\par Parameters:
	<b>ULONG f</b>\n\n
	The flags to set. See \ref Render_Instance_Flags . */
	void SetFlag(ULONG f) {  flags |= f; }
	/*! Clears the specified flag(s).
	\par Parameters:
	<b>ULONG f</b>\n\n
	The flags to set to zero. See \ref Render_Instance_Flags . */
	void ClearFlag(ULONG f) {  flags &= ~f; }
	/*! Returns TRUE if the specified flag(s) are set; otherwise
	FALSE.
	\par Parameters:
	<b>ULONG f</b>\n\n
	The flags to set to zero. See \ref Render_Instance_Flags */
	BOOL TestFlag(ULONG f) { return flags&f?1:0; }
	BOOL Hidden() { return TestFlag(INST_HIDE); }
	BOOL IsClip() { return TestFlag(INST_CLIP); }

	/*! Returns a pointer to the next in <b>RenderInstance</b> in the
	list. A pointer to the first element in the list may to retrieved from
	RenderGlobalContext::InstanceList(). */
	virtual RenderInstance *Next()=0;

	/*! Returns the validity interval of the mesh of this render
	instance. */
	virtual Interval MeshValidity()=0;
	/*! Returns the number of lights affecting the node. */
	virtual int NumLights()=0;
	/*! Returns a pointer to the LightDesc for the 'i-th' light
	affecting the node.
	\par Parameters:
	<b>int n</b>\n\n
	Specifies which light. */
	virtual LightDesc *Light(int n)=0; 

	/*! \remarks	Returns TRUE if this particular instance will cast shadows from the
	particular light based on the light's Exclusion/Inclusion list; FALSE if it
	won't cast shadows.
	\par Parameters:
	<b>const ObjLightDesc\& lt</b>\n\n
	Describes the light. See Class ObjLightDesc. */
	virtual BOOL CastsShadowsFrom(const ObjLightDesc& lt)=0; 

	/*! Returns the INode pointer for the instance. */
	virtual INode *GetINode()=0;  						 
	/*! Returns a pointer to the evaluated object for the instance.
	You can use this to get more information about the type of object being
	rendered. For instance you could look at the <b>Class_ID</b> and recognize
	it as a sphere, a box, a torus, etc. */
	virtual Object *GetEvalObject()=0; 					 
	/*! Returns the material requirements of the material assigned to
	the node. See \ref materialRequirementsFlags.
	\par Parameters:
	<b>int mtlNum</b>\n\n
	Specifies the number of the sub-material whose requirements should be
	returned. A value of <b>-1</b> may be passed to return a value generated by
	looping over all the sub-materials and ORing together the requirements.\n\n
	<b>int faceNum</b>\n\n
	This is the integer face number for objects which support material per face
	(if flag <b>INST_MTL_BYFACE</b> is set). See
	Class IChkMtlAPI. */
	virtual ULONG MtlRequirements(int mtlNum, int faceNum)=0;  	 
	/*! Returns the geometric normal of the specified face in object
	space.
	\par Parameters:
	<b>int faceNum</b>\n\n
	Zero based index of the face whose normal is returned. */
	virtual Point3 GetFaceNormal(int faceNum)=0;         
	/*! Returns the vertex normal of the specified face in camera
	coordinates.
	\par Parameters:
	<b>int faceNum</b>\n\n
	Zero based index of the face in the mesh.\n\n
	<b>int vertNum</b>\n\n
	Zero based index of the vertex in the face. */
	virtual Point3 GetFaceVertNormal(int faceNum, int vertNum)=0;  
	/*! Returns the three vertex normals of the specified face in
	camera coordinates.
	\par Parameters:
	<b>int faceNum</b>\n\n
	Zero based index of the face in the mesh.\n\n
	<b>Point3 n[3]</b>\n\n
	The normals are returned here. */
	virtual void GetFaceVertNormals(int faceNum, Point3 n[3])=0;   
	/*! Returns the coordinate for the specified vertex in camera
	coordinates
	\par Parameters:
	<b>int vertnum</b>\n\n
	The zero based index of the vertex in the mesh. */
	virtual Point3 GetCamVert(int vertnum)=0; 			 
	/*! Returns the vertices of the specified face in object
	coordinates.
	\par Parameters:
	<b>int fnum</b>\n\n
	Zero based index of the face in the mesh.\n\n
	<b>Point3 obp[3]</b>\n\n
	The three vertices of the face in object coordinates. */
	virtual void GetObjVerts(int fnum, Point3 obp[3])=0; 
	/*! Returns the vertices of the specified face in camera (view)
	coordinates.
	\par Parameters:
	<b>int fnum</b>\n\n
	Zero based index of the face in the mesh.\n\n
	<b>Point3 cp[3]</b>\n\n
	The three vertices of the face in camera coordinates. */
	virtual void GetCamVerts(int fnum, Point3 cp[3])=0; 
	/*! \remarks	This is a general purpose function that allows the API to be extended in the
	future. The 3ds Max development team can assign new <b>cmd</b> numbers and
	continue to add functionality to this class without having to 'break' the API.
	\par Parameters:
	<b>int cmd</b>\n\n
	The index of the command to execute.\n\n
	<b>ULONG_PTR arg1=0</b>\n\n
	Optional argument 1. See the documentation where the <b>cmd</b> option is
	discussed for more details on these parameters.\n\n
	<b>ULONG_PTR arg2=0</b>\n\n
	Optional argument 2.\n\n
	<b>ULONG_PTR arg3=0</b>\n\n
	Optional argument 3.
	\return  An integer return value. See the documentation where the <b>cmd</b>
	option is discussed for more details on the meaning of this value.
	\par Default Implementation:
	<b>{ return 0; }</b> */
	virtual INT_PTR Execute(int /*cmd*/, ULONG_PTR /*arg1*/=0, ULONG_PTR /*arg2*/=0, ULONG_PTR /*arg3*/=0) { return 0; } 

	/*! \remarks	Objects can provide a material as a function of face number via the
	interface provided by Class IChkMtlAPI. This method will return <b>RenderInstance::mtl</b> if flag
	<b>INST_MTL_BYFACE</b> is not set. If <b>INST_MTL_BYFACE</b> is set it will
	return the proper by-face material. See \ref Render_Instance_Flags .
	\par Parameters:
	<b>int faceNum</b>\n\n
	The zero based index of the face in the mesh.\n\n
	Objects can provide a material as a function of face number via the
	IChkMtlAPI interface (chkmtlapi.h). */
	virtual Mtl *GetMtl(int faceNum)=0;  
};

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

#pragma warning(push)
#pragma warning(disable:4100)

class ShadeContext;
class IReshadeFragment;
class IllumParams;

// must be greater than I_USERINTERFACE in AnimatableInterfaceIDs.h
#define IID_IReshading  0xabe00001

// Materials support this interface to support preshading/reshading
/*! \sa  Class IReshadeFragment, Class ShadeContext, Class IllumParams\n\n
\par Description:
This interface class is the interface that must be supported by materials and
shaders to support interactive rendering in 3ds Max. If this interface is not
returned when requested through <b>GetInterface()</b> on the mtl or shader,
then it is determined they do not support reshading. Any material that does not
support reshading is evaluated only at preshading time.\n\n
In 3ds Max, interactive rendering is implemented as fragment based caching
scheme. It's very much like a giant multi-layer g-buffer with enough
information stored with each fragment to re-run the material shading process
without re-rendering, all stored in a compressed form.\n\n
The rendering process in divided into 2 parts: preshading and reshading.
Preshading builds the scene and renders fragments to the compressed g-buffer,
called the <b>rsBuffer</b>. To do this, it saves a minimal amount of
information with each fragment, then gives each material and texture the
opportunity to pre-shade itself.\n\n
Basically, the reshader populates a minimal <b>ShadeContext</b>, with the
fragment center position in camera space (where shading is done), the shading
normal, the sub-pixel mask, the coverage, pointers to the original material and
<b>INode</b>, light lists, screen bounding box of object, the renderID, and
screen integer x and y.\n\n
Any other values from the shade context that a material, shader,
texture(future), atmosphere(future) or light(future) needs to save, it does so
at pre-shade time, and retrieves the data at reshading time. So, for example,
the multi-materials save which material is visible, anisotropic shaders save
the Anisotropic reference vector.  */
class IReshading: public MaxHeapOperators
{
public:
			// if supporting shaders, plug-in mtl should inherit from StdMat2
			enum ReshadeRequirements{ RR_None = 0, RR_NeedPreshade, RR_NeedReshade };
			/*! \remarks Destructor. */
			virtual ~IReshading() {;}

			// GetReshadeRequirements is called by the interactive reshader after being notified that the material has changed.
			// the return value indicates if the material is still valid, or needs to be preshaded or reshaded. the value
			// should pertain only to the latest change to the material
			// if a material doesn't support reshading, it doesn't need to override this function -- any change will cause
			// the nodes to which it's attached to be rerendered.
			/*! \remarks This method is called by the interactive reshader after
			being notified that the material has changed. The return value indicates
			if the material is still valid, or needs to be preshaded or reshaded. The
			value should pertain only to the latest change to the material. If a
			material doesn't support reshading, it doesn't need to override this
			function -- any change will cause the nodes to which it's attached to be
			re- preShaded.
			\return  One of the following values;\n\n
			<b>RR_None</b>\n\n
			No actions needs to be taken.\n\n
			<b>RR_NeedPreshade</b>\n\n
			A pre-shade is needed.\n\n
			<b>RR_NeedReshade</b>\n\n
			A re-shade is needed.
			\par Default Implementation:
			<b>{ return RR_NeedPreshade; }</b> */
			virtual ReshadeRequirements GetReshadeRequirements() { return RR_NeedPreshade; }
			/*! \remarks This method will pre-shade the object and cache any needed
			values in the fragment.\n\n
			This method is called on the materials/shaders/textures as the reshading
			buffer is being built at the same point in the rendering process where
			the materials shade function would normally be called. Note that at this
			time the shade context is completely valid, as in a normal rendering. Any
			values the material wishes to cache can be attached to the reshading
			fragment passed in, and retrieved later at postShade time.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			A reference to the shade context.\n\n
			<b>IReshadeFragment* pFrag</b>\n\n
			A pointer to the fragment.
			\par Default Implementation:
			<b>{ }</b> */
			virtual void PreShade(ShadeContext& sc, IReshadeFragment* pFrag ) { }
			/*! \remarks This method will retrieve cached values and compute shade
			\& transparency for the fragment.\n\n
			This method is called for the material of each fragment as the reshading
			buffer is being traversed. Materials retrieve whatever they may have
			stored to complete the minimal shade context and compute a shade for the
			fragment. <b>PostShade()</b> calls to shaders contain an additional
			parameter, the <b>IllumParams</b>, filled with the textured/blended but
			unshaded values for each texture. The shade context passed into
			<b>PostShade()</b> is the minimal shade context outlined above.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			A reference to the shade context.\n\n
			<b>IReshadeFragment* pFrag</b>\n\n
			A pointer to the fragment.\n\n
			<b>int\& nextTexIndex</b>\n\n
			A reference to the next texture index.\n\n
			<b>IllumParams* ip = NULL</b>\n\n
			A pointer to the IllumParams containing textured/blended but unshaded
			values for each texture.
			\par Default Implementation:
			<b>{ }</b> */
			virtual void PostShade(ShadeContext& sc, IReshadeFragment* pFrag, 
									int& nextTexIndex, IllumParams* ip = NULL ) { }

};

#pragma warning(pop)

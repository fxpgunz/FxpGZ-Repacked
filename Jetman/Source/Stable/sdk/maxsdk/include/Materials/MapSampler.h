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
#include "../AColor.h"

#pragma warning(push)
#pragma warning(disable:4100)

class ShadeContext;

//  A texture map implements this class and passes it into  EvalUVMap,
//  EvalUVMapMono, and EvalDeriv to evaluate itself with tiling & mirroring
/*! \sa  Class UVGen,
Class ShadeContext.
\par Description:
A texture map implements this class and passes it into the <b>UVGen</b> methods
<b>EvalUVMap()</b>, <b>EvalUVMapMono()</b>, and <b>EvalDeriv()</b> to evaluate
itself with tiling \& mirroring. Each of the methods of this class are used to
sample the map at a single UV coordinate.  */
class MapSampler: public MaxHeapOperators {
	 public:
			/*! \remarks Destructor. */
			virtual ~MapSampler() {;}
			// required:
			/*! \remarks This method is required. It is called to sample the map at a single UV
			coordinate. This method should not try to antialias the map, it should
			just return the value. Take for example a bitmap texture implementing
			this method. If the <b>u</b> values passed was 0.5 and the <b>v</b> value
			passed was 0.5, the plug-in would just return the pixel value at the
			center of the bitmap as an <b>AColor</b>.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			The ShadeContext.\n\n
			<b>float u</b>\n\n
			The U value in the range of 0.0 to 1.0\n\n
			<b>float v</b>\n\n
			The V value in the range of 0.0 to 1.0
			\return  The sampled value as an <b>AColor</b>. */
			virtual  AColor Sample(ShadeContext& sc, float u,float v)=0;
			/*! \remarks This method is required. It is called to return a value from the map that
			is the averaged value over the specified region of width <b>du</b> and
			height <b>dv</b>, centered at <b>u, v</b>. Certain map types may use the
			<b>du</b> and <b>dv</b> values directly to perform antialiasing of the
			map.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			The ShadeContext.\n\n
			<b>float u</b>\n\n
			The U value in the range of 0.0 to 1.0\n\n
			<b>float v</b>\n\n
			The V value in the range of 0.0 to 1.0\n\n
			<b>float du</b>\n\n
			This parameter can be considered the width of a small rectangle center on
			u,v. The u and v value are guaranteed to be inside this rectangle.\n\n
			<b>float dv</b>\n\n
			This parameter can be considered the height of a small rectangle center
			on u,v. The u and v value are guaranteed to be inside this rectangle.
			\return  The sampled value as an <b>AColor</b>. */
			virtual  AColor SampleFilter(ShadeContext& sc, float u,float v, float du, float dv)=0;
			// optional:
			virtual void SetTileNumbers(int iu, int iv) {} // called before SampleFunctions to tell what tile your in
			/*! \remarks This method is optional as a default implementation is provided. It may
			be implemented however if a certain map type can be more efficient in
			evaluating a mono channel. For example a noise function that produces a
			single floating point value as a function of UV. This method is called to
			sample the map at a single UV coordinate. This method should not try to
			antialias the map, it should just return the value.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			The ShadeContext.\n\n
			<b>float u</b>\n\n
			The U value in the range of 0.0 to 1.0\n\n
			<b>float v</b>\n\n
			The V value in the range of 0.0 to 1.0
			\return  The sampled value as an float.
			\par Default Implementation:
			<b>{ return Intens(Sample(sc,u,v)); }</b> */
			virtual  float SampleMono(ShadeContext& sc, float u,float v) { return Intens(Sample(sc,u,v)); }
			/*! \remarks This method is optional. It is called to return a value from the map that
			is the averaged value over the specified region of width <b>du</b> and
			height <b>dv</b>, centered at <b>u, v</b>. Certain map types may use the
			<b>du</b> and <b>dv</b> values directly to perform antialiasing of the
			map.
			\par Parameters:
			<b>ShadeContext\& sc</b>\n\n
			The ShadeContext.\n\n
			<b>float u</b>\n\n
			The U value in the range of 0.0 to 1.0\n\n
			<b>float v</b>\n\n
			The V value in the range of 0.0 to 1.0\n\n
			<b>float du</b>\n\n
			This parameter can be considered the width of a small rectangle center on
			<b>u,v</b>. The <b>u</b> and <b>v</b> value are guaranteed to be inside
			this rectangle.\n\n
			<b>float dv</b>\n\n
			This parameter can be considered the height of a small rectangle center
			on <b>u,v</b>. The <b>u</b> and <b>v</b> value are guaranteed to be
			inside this rectangle.
			\return  The sampled value as an float.
			\par Default Implementation:
			<b>{ return Intens(SampleFilter(sc,u,v,du,dv)); }</b> */
			virtual  float SampleMonoFilter(ShadeContext& sc, float u,float v, float du, float dv){
				 return Intens(SampleFilter(sc,u,v,du,dv)); 
				 }
	 };

#pragma warning(pop)

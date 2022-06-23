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

#include "MtlBase.h"

// This class is used by texture maps to put up the output filter 
// rollup, and perform the output filtering.
/*! \sa  Class MtlBase, Class AColor, Class Point3.\n\n
\par Description:
This class is used by texture maps to put up the 'Output' rollup in the
materials editor, and perform the output filtering. Currently this provides
control over the Output Amount, RGB Level, and RGB Offset. In the future this
may be enhanced to include other things that are often desirable on the output
stage. These are things like tinting, color shifting, etc. All methods of this
class are implemented by the system.\n\n
A plug-in will typically call these methods from the implementations of
<b>EvalColor()</b>, <b>EvalMono()</b>, etc. This is just done as a final
adjustment to scale or offset the value. The code below is from the Gradient
texture map.
\code
AColor Gradient::EvalColor(ShadeContext& sc) {
	return texout->Filter(uvGen->EvalUVMap(sc, &mysamp));
}
float Gradient::EvalMono(ShadeContext& sc) {
	return texout->Filter(uvGen->EvalUVMapMono(sc, &mysamp));
}
\endcode */
class TextureOutput: public MtlBase {
	 public:
			/*! \remarks Filters the specified AColor and returns it.
			\par Parameters:
			<b>AColor c</b>\n\n
			The color to filter.
			\return  The filtered color as an AColor. */
			virtual AColor Filter(AColor c) = 0;
			/*! \remarks Filters the specified float value and returns it.
			\par Parameters:
			<b>float f</b>\n\n
			The value to filter.
			\return  The filtered value. */
			virtual float Filter(float f) = 0;
			/*! \remarks Filters the specified Point3 value and returns it.
			\par Parameters:
			<b>Point3 p</b>\n\n
			The Point3 to filter.
			\return  The filtered Point3. */
			virtual Point3 Filter(Point3 p) = 0;
			/*! \remarks Returns the output level (amount) at the specified time.
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The time at which to retrieve the output level. */
			virtual float GetOutputLevel(TimeValue t) = 0;
			/*! \remarks Sets the output level at the specified time.
			\par Parameters:
			<b>TimeValue t</b>\n\n
			The time to set the output level.\n\n
			<b>float v</b>\n\n
			The value of the output level. */
			virtual void SetOutputLevel(TimeValue t, float v) = 0;
			/*! \remarks Sets the state of the 'Invert' toggle in the Output rollup.
			\par Parameters:
			<b>BOOL onoff</b>\n\n
			TRUE to turn on; FALSE to turn off. */
			virtual void SetInvert(BOOL onoff)=0;
			/*! \remarks Returns the state of the 'Invert' toggle in the Output
			rollup.
			\return  TRUE is on; FALSE is off. */
			virtual BOOL GetInvert()=0;
			/*! \remarks			Sets the 'Output' rollup page to open or closed.
			\par Parameters:
			<b>BOOL open</b>\n\n
			TRUE to open the rollup; FALSE to close it. */
			virtual void SetRollupOpen(BOOL open)=0;
			/*! \remarks			Returns the open or closed state of the 'Output' rollup.
			\return  TRUE for open; FALSE for closed. */
			virtual BOOL GetRollupOpen()=0;
			SClass_ID SuperClassID() { return TEXOUTPUT_CLASS_ID; }
			CoreExport SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
			CoreExport MSTR SvGetName(IGraphObjectManager *gom, IGraphNode *gNode, bool isBeingEdited)
			{
				return Animatable::SvGetName(gom, gNode, isBeingEdited);
			}
};

CoreExport TextureOutput* GetNewDefaultTextureOutput();

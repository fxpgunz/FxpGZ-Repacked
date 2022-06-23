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

namespace CatAPI
{
	static const Interface_ID LAYERROOT_INTERFACE_FP(0x53b56707, 0x26d31718);
#define GetCATILayerRootFPInterface(anim)  (anim ? ((CatAPI::ILayerRoot*)anim->GetInterface(CatAPI::LAYERROOT_INTERFACE_FP)) : nullptr)

	class ILayerRoot : public FPMixinInterface
	{
	public:
		virtual int AppendLayer(const MSTR& name, const MCHAR *method) = 0;
		virtual BOOL InsertLayer(const MSTR& name, int layer, const MCHAR *method) = 0;
		virtual void RemoveLayer(int layer) = 0;
		virtual void MoveLayerUp(int layer) = 0;
		virtual void MoveLayerDown(int layer) = 0;
		virtual int NumLayers() const = 0;

		/// Get the indexed layers color.  
		/// This is the color that will be set to a nodes wire 
		/// color when "Rig Coloring Mode" is set to "by layer".
		/// \param index - The index of the layer to get the color of.  This must be
		///					the index of an absolute layer.
		/// \return The color of the specified layer, if valid, or black
		virtual Color GetLayerColor(int index) = 0;

		/// Set the indexed layers color.  
		/// This is the color that will be set to a nodes wire 
		/// color when "Rig Coloring Mode" is set to "by layer".
		/// \param index - The index of the layer to get the color of.  This must be
		///					the index of an absolute layer.
		/// \param newColor - The color being set to the layer.
		/// \return True if the color is set correctly, false if the method fails
		virtual BOOL SetLayerColor(int index, Color *newColor) = 0;

		virtual void SelectLayer(int layer) = 0;
		virtual void SelectNone() = 0;
		virtual int GetSelectedLayer() const = 0;

		virtual void SoloLayer(int layer) = 0;
		virtual void SoloNone() = 0;
		virtual int GetSoloLayer() const = 0;
		virtual BOOL IsLayerSolo(int layer) = 0;

		virtual BOOL SaveClip(const MSTR& filename, TimeValue start_t, TimeValue end_t, int from_layer, int to_layer) = 0;
		virtual BOOL SavePose(const MSTR& filename) = 0;

		virtual INode* LoadClip(const MSTR& filename, TimeValue t, BOOL scale_data = TRUE, BOOL transformData = TRUE, BOOL mirrorData = FALSE, BOOL mirrorWorldX = FALSE, BOOL mirrorWorldY = FALSE) = 0;
		virtual INode* LoadPose(const MSTR& filename, TimeValue t, BOOL scale_data = TRUE, BOOL transformData = TRUE, BOOL mirrorData = FALSE, BOOL mirrorWorldX = FALSE, BOOL mirrorWorldY = FALSE) = 0;

		virtual MSTR GetFileTagValue(const MSTR& filename, const MSTR& tag) = 0;

		virtual BOOL LoadHTR(const MSTR& filename, const MSTR& camfile) = 0;
		virtual BOOL LoadBVH(const MSTR& filename, const MSTR& camfile) = 0;
		virtual BOOL LoadFBX(const MSTR& filename, const MSTR& camfile) = 0;
		virtual BOOL LoadBIP(const MSTR& filename, const MSTR& camfile) = 0;

		virtual void CollapsePoseToCurLayer(TimeValue t) = 0;
		virtual BOOL CollapseTimeRangeToLayer(TimeValue start_t, TimeValue end_t, TimeValue iKeyFreq, BOOL regularplot, int numpasses, float posdelta, float rotdelta) = 0;


		virtual int  GetTrackDisplayMethod() const = 0;
		virtual void SetTrackDisplayMethod(int n) = 0;

		virtual void CopyLayer(int n) = 0;
		virtual void PasteLayer(BOOL instance, BOOL duplicatelayerinfo) = 0;
		virtual INode* CreatePasteLayerTransformNode() = 0;
	};
}
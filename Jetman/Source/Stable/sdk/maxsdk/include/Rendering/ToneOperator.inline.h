//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2014 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

//==================================================================================================
// class ToneOperator
//==================================================================================================

inline ToneOperator::ToneOperator() 
{ 
    ClearAFlag(A_TONEOP_PROCESS_BG); 
}

inline RefResult ToneOperator::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle /*hTarget*/,  PartID& /*partID*/, RefMessage /*message*/, BOOL /*propagate*/) 
{
    return REF_SUCCEED;
}

inline SClass_ID ToneOperator::SuperClassID() 
{ 
    return TONE_OPERATOR_CLASS_ID; 
}

inline IOResult ToneOperator::Save(ISave *isave) 
{ 
    return SpecialFX::Save(isave); 
}

inline IOResult ToneOperator::Load(ILoad *iload) 
{ 
    return SpecialFX::Load(iload); 
}

inline BOOL ToneOperator::Active(TimeValue /*t*/) 
{ 
    return !TestAFlag(A_TONEOP_DISABLED); 
}

inline void ToneOperator::SetActive(
    bool		active,
    TimeValue	/*t*/
    ) {
        if (active ^ (TestAFlag(A_TONEOP_DISABLED) == 0)) {
            if (active) {
                ClearAFlag(A_TONEOP_DISABLED);
            }
            else {
                SetAFlag(A_TONEOP_DISABLED);
            }
            NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
        }
}

inline void ToneOperator::SetProcessBackground(bool active) {
    if (active ^ (TestAFlag(A_TONEOP_PROCESS_BG) != 0)) {
        if (active) {
            SetAFlag(A_TONEOP_PROCESS_BG);
        }
        else {
            ClearAFlag(A_TONEOP_PROCESS_BG);
        }
        NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
    }
}

inline bool ToneOperator::GetProcessBackground() const
{ 
    return TestAFlag(A_TONEOP_PROCESS_BG) != 0; 
}

inline void ToneOperator::SetIndirectOnly(bool active) {
    if (active ^ (TestAFlag(A_TONEOP_INDIRECT_ONLY) != 0)) {
        if (active) {
            SetAFlag(A_TONEOP_INDIRECT_ONLY);
        }
        else {
            ClearAFlag(A_TONEOP_INDIRECT_ONLY);
        }
        NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
    }
}

inline bool ToneOperator::GetIndirectOnly() const
{ 
    return TestAFlag(A_TONEOP_INDIRECT_ONLY) != 0; 
}

inline void ToneOperator::RGBToScaled(float energy[3], const Point2& xyCoord) const
{
    InverseMap(energy, xyCoord);
}

inline float ToneOperator::RGBToScaled(float energy, const Point2& xyCoord) const
{
    return InverseMap(energy, xyCoord);
}

inline void ToneOperator::RGBToScaled(float energy[3]) const
{
    InverseMap(energy);
}

inline float ToneOperator::RGBToScaled(float energy) const
{
    return InverseMap(energy);
}

inline bool ToneOperator::BuildMaps(TimeValue /*t*/, RenderMapsContext& /*rmc*/)
{ 
    return true; 
}

inline void ToneOperator::SubRenderSample(float /*energy*/[3]) 
{ 
}

inline ToneOpParamDlg* ToneOperator::CreateParamDialog(IRendParams* /*ip*/)
{
    return nullptr;
}

inline BOOL ToneOperator::SetDlgThing(ToneOpParamDlg* /*dlg*/) 
{ 
    return FALSE; 
}


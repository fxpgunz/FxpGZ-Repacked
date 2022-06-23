//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

#include "../GetCOREInterface.h"

namespace MaxSDK 
{;

//==================================================================================================
// class IPhysicalSunSky
//==================================================================================================

inline Class_ID IPhysicalSunSky::GetClassID()
{
    return Class_ID(0x223349b4, 0x562a7d8b);
}

//==================================================================================================
// class IPhysicalSunSky::ShadingParameters
//==================================================================================================

inline IPhysicalSunSky::ShadingParameters::ShadingParameters()
    : horizon_height(0.0f),
    horizon_blur(0.0f),
    global_multiplier(0.0f, 0.0f, 0.0f),
    sun_illuminance(0.0f, 0.0f, 0.0f),
    sun_luminance(0.0f, 0.0f, 0.0f),
    sun_glow_intensity(0.0f),
    sky_contribution_multiplier(0.0f),
    sky_ground_contribution(0.0f, 0.0f, 0.0f),
    sun_disc_angular_radius(0.0f),
    sun_smooth_angular_radius(0.0f),
    sun_glow_angular_radius(0.0f),
    color_saturation(0.0f),
    color_tint(0.0f, 0.0f, 0.0f),
    ground_color(0.0f, 0.0f, 0.0f),
    night_falloff(1.0f),
    night_luminance(0.0f, 0.0f, 0.0f),
    sun_direction(0.0f, 0.0f, 0.0f),
    sun_direction_for_sky_contribution(0.0f, 0.0f, 0.0f),
    perez_A(0.0f, 0.0f, 0.0f),
    perez_B(0.0f, 0.0f, 0.0f),
    perez_C(0.0f, 0.0f, 0.0f),
    perez_D(0.0f, 0.0f, 0.0f),
    perez_E(0.0f, 0.0f, 0.0f),
    perez_Z(0.0f, 0.0f, 0.0f)
{

}

//==================================================================================================
// class IPhysicalSunSky::IShaderGenerator
//==================================================================================================

inline Interface_ID IPhysicalSunSky::IShaderGenerator::GetInterfaceID()
{
    return Interface_ID(0x21797946, 0x8f003b3);
}

inline IPhysicalSunSky::IShaderGenerator* IPhysicalSunSky::IShaderGenerator::GetInterface()
{
    return dynamic_cast<IPhysicalSunSky::IShaderGenerator*>(GetCOREInterface(GetInterfaceID()));
}

}   // namespace


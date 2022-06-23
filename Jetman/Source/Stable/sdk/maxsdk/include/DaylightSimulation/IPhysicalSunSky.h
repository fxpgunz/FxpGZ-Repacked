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
#include "../Interval.h"
#include "../Color.h"
#include "../ifnpub.h"
#include <memory>

namespace MaxSDK 
{;

//==================================================================================================
// class IPhysicalSunSky
//
//! \brief Access to functionality and properties of the Physical Sun & Sky environment map.
/*! This interface provides SDK access to the properties of the Physical Sun & Sky environment. 
    To retrieve a pointer to this interface, perform a dynamic_cast on a Texmap.
*/
class IPhysicalSunSky :
    public MaxHeapOperators
{
public:

    struct ShadingParameters;
    class IShader;
    class IShaderGenerator;

    //! \brief Returns the class ID for the physical sun & sky plugin.
    //! Note that IPhysicalSunSky is an interface which may be freely be implemented by any plugin. This class ID is therefore
    //! that of the standard Physical Sun & Sky plugin.
    static Class_ID GetClassID();

    //! \brief Returns the shading parameters to be used for the Sun & Sky environment.
    /*! The source code for the Physical Sun & Sky shader is available in the maxsdk samples folder: maxsdk/samples/systems/sunlight/PhysicalSunSkyShader.h.
        A third party renderer may choose to implement this shader for rendering within 3ds Max. In that case, the parameters to be passed to the shader
        may be obtained from this method. 
        \param t The time at which to evaluate the parameters.
        \param validity The interval into which the validity of the evaluated parameters is intersected.
        \return The shading parameters to be used with the standard shader. 
        \remark If possible, a third party renderer should avoid calling the shader itself, and should instead use the built-in shader returned by
            InstantiateShader(). */
    virtual ShadingParameters EvaluateShadingParameters(const TimeValue t, Interval& validity) const = 0;
    //! \brief Instantiates a object which may be used for evaluating the Physical Sun & Sky environment.
    /*! A third-party plugin developer has three options for supporting the Physical Sun & Sky Environment:
        -# Calling Texmap::EvalColor().
        -# Implementing its own version of the shader, using the source code provided in maxsdk/samples/systems/sunlight/PhysicalSunSkyShader.h.
        -# Using the instance of IShader returned here.

        The advantage of using this method is that it enables safe multi-threaded evaluation. The IShader that is returned here is completely independent
        of the the 3ds Max plugin, and may be evaluated or deleted at will, in a multi-threaded manner, by the client code.
        \param t The time at which to evaluate the parameters.
        \param validity The interval into which the validity of the evaluated parameters is intersected.
        \return A shading object which is henceforth owned by the caller. */
    virtual std::unique_ptr<IShader> InstantiateShader(const TimeValue t, Interval& validity) const = 0;
};

//==================================================================================================
// class IPhysicalSunSky::ShadingParameters
//
//! \brief Encapsulates the set of shading parameters for the Physical Sun & Sky.
/*! These are the parameters which have been derived from the UI parameters, many of them pre-computed
    to optimize the shader at run-time. These parameters may be used directly with the shading function
    compute_sunsky_env_color(), which is defined in PhysSunSky.rtsl and included through
    PhysicalSunSkyShader.h, in maxsdk/samples/systems/sunlight.
*/
struct IPhysicalSunSky::ShadingParameters :
    public MaxHeapOperators
{
    ShadingParameters();

    //! Horizon height, as the sine of the angle offset for the horizon.
    float horizon_height;
    //! Horizon blur range, as the sine of the angle over which the horizon is blurred.
    float horizon_blur;

    //! Global multiplier, applied to the entire environment map (both sun and sky, as well as night color). May include tinting.
    Color global_multiplier;

    //! The total illuminance of the sun (the integral of the contribution of the entire sun and sun glow). This is used to 
    //! fake lighting on the procedural ground plane.
    Color sun_illuminance;
    //! The luminance of the sun. This is the value which, when multiplied with the sun/glow distribution function, gives us
    //! how much light is emitted (by the sun/glow) from a point in the sky.
    Color sun_luminance;
    //! Multiplier used in the intensity of the sun glow.
    float sun_glow_intensity;

    //! Multiplier for the sky contribution
    float sky_contribution_multiplier;
    //! The (approximated) integral of the sky contribution over the entire environment, used in illuminating the procedural
    //! ground plane.
    Color sky_ground_contribution;

    //! Angular radius, in radians, of the sun disc.
    float sun_disc_angular_radius;
    //! Angular radius, in radians, of smoothing/blur between the sun disc and sun glow.
    float sun_smooth_angular_radius;
    //! Angular radius, in radians, of the sun glow region.
    float sun_glow_angular_radius;

    //! Saturation adjustment for the environment (both sun and sky). 
    //! This is applied before the night color is added (and therefore doesn't affect the night color).
    float color_saturation;
    //! Color adjustment (multiplier) for the environment (both sun and sky). 
    //! This is applied before the night color is added (and therefore doesn't affect the night color).
    //! This is applied <b>after</b> the saturation adjustment, making it possible to add some coloring back after removing
    //! saturation.
    Color color_tint;

    //! Color for the procedural/fake ground plane.
    Color ground_color;
    //! Multiplier for the sky color, to take into account that the sun has dipped below the horizon and that the sky
    //! should progressively get dimmer. This is applied to the sky contribution, before the night color is added.
    float night_falloff;
    //! Luminance for the night sky, used as a floor value for the sky luminance. In cd/m^2
    Color night_luminance;

    //! The sun direction (pointing towards the sun). This direction is already adjusted to compensate for the horizon height;
    //! If the horizon is raised, then the sun is also raised such that it still sets at the horizon.
    Point3 sun_direction;
    //! This is the sun direction used when calculating the sky contribution. This direction may differ from sun_direction: when the sun
    //! nears the horizon, this value is adjusted (with a smooth falloff) to avoid that the sky go directly to black after the sun
    //! reaches the horizon.
    Point3 sun_direction_for_sky_contribution;

    //! The Perez coefficients used in calculating the brightness and color of the sky. They are expressed as triplets:
    //! - x is for calculating the luminance Y
    //! - y and z are for calculating the chromacity xy */
    //@{
    Point3 perez_A;
    Point3 perez_B;
    Point3 perez_C;
    Point3 perez_D;
    Point3 perez_E;
    Point3 perez_Z;
    //@}
};

//==================================================================================================
// class IPhysicalSunSky::IShader
//
//! \brief An object which encapsulates the evaluation (shading) of the Physical Sun & Sky environment.
/*! This class separates the shading functionality out of the Physical Sun & Sky environment plugin, enabling
    safe evaluation of the shader while the scene is being manipulated (i.e. in a multi-threaded manner).
*/
class IPhysicalSunSky::IShader :
    public MaxHeapOperators
{
public:
    virtual ~IShader() {}

    //! \brief Evaluates the environment map in the given direction.
    /*! \param direction The direction of the ray in which to evaluate the environment. A unit vector is expected.
        \return The environment map color, evaluated in the provided direction. */
    virtual Color Evaluate(const Point3& direction) const = 0;
};

//==================================================================================================
// class IPhysicalSunSky::IShaderGenerator
//
//! \brief Provides facilities to initialize the shading parameters of the Physical Sun & Sky plugin.
/*! This class encapsulates the logic for initializing the shading parameters of the Physical Sun & Sky plugin. Its intent
    is to enabled arbitrary (third party) plugins to use the shading facilities of the Physical Sun & Sky plugin. This 
    is achieved by converting the UI-level parameters to those of the Physical Sun & Sky plugin (which should be somewhat simple),
    and letting this class massage those parameters to the internal ones used by the shader.
    For example, the ART renderer uses this interface to convert the mental ray Sun & Sky into a Physical Sun & Sky shader.
*/
class IPhysicalSunSky::IShaderGenerator : 
    public FPStaticInterface
{
public:

    //! Returns the ID of this interface.
    static Interface_ID GetInterfaceID();
    //! Returns a pointer to this interface.
    static IShaderGenerator* GetInterface();

    //! \brief Converts the given UI-level parameters into the parameters expected by the Physical Sun & Sky shader.
    /*! \param global_intensity A global multiplier for the entire environment map.
        \param haze The haze value, in range [0,1].
        \param sun_enabled Whether the sun should be rendered.
        \param sun_disc_intensity The intensity multiplier of the sun overall (both sun and sky).
        \param sun_glow_intensity The intensity multiplier for the glow that surrounds the sun.
        \param sun_disc_scale The scale for the size of the sun (1.0 == physical size).
        \param sky_intensity The intensity multiplier for the sky only (doesn't affect the sun).
        \param night_color The color of the night sky.
        \param night_intensity The intensity of the night sky (a multiplier for the night color).
        \param horizon_height_deg The horizon height in degrees.
        \param horizon_blur_deg The size of the horizon blurred area, in degrees.
        \param ground_color The color of the fake ground plane.
        \param saturation The saturation of the environment map result.
        \param tint A color multiplier for the environment map result.
        \param sun_direction A unit vector pointing towards the sun.
        \param use_custom_perez_coefficients Set to true if and only if custom Perez coefficients are provided. These coefficients are used to calculate
            the luminance of the sky; the Preetham coefficients are still used to compute the chromacity.
        \param custom_perez_A The custom value for the A coefficient of the Perez equation, to be supplied if use_custom_perez_coefficients is true.
        \param custom_perez_B The custom value for the B coefficient of the Perez equation, to be supplied if use_custom_perez_coefficients is true.
        \param custom_perez_C The custom value for the C coefficient of the Perez equation, to be supplied if use_custom_perez_coefficients is true.
        \param custom_perez_D The custom value for the D coefficient of the Perez equation, to be supplied if use_custom_perez_coefficients is true.
        \param custom_perez_E The custom value for the E coefficient of the Perez equation, to be supplied if use_custom_perez_coefficients is true.
        \param custom_perez_diffuse_horizontal_illuminance The custom diffuse horizontal illuminance to be used with the custom Perez coefficients.
            The sky illuminance will be normalized to match this. A value of 0.0 may be passed to disable the normalization.
        \param custom_perez_direct_normal_illuminance The custom direct normal illuminance to be used with the custom Perez coefficients.
            The sun illuminance will be normalized to match this. A value of 0.0 may be passed to disable the normalization.
        \param t The time at which certain parameters (such as exposure control) are to be evaluated.
        \param validity The interval into which the validity of the evaluated parameters is intersected.
        \return The set of parameters which may be used directly with the Physical Sun & Sky shader. */
    virtual ShadingParameters CreateShadingParameters(
        const float global_intensity,
        const float haze,
        const bool sun_enabled,
        const float sun_disc_intensity,
        const float sun_glow_intensity,
        const float sun_disc_scale,
        const float sky_intensity,
        const Color night_color,
        const float night_intensity,
        const float horizon_height_deg,
        const float horizon_blur_deg,
        const Color ground_color,
        const float saturation,
        const Color tint,
        const Point3 sun_direction,
        const bool use_custom_perez_coefficients,
        const float custom_perez_A,
        const float custom_perez_B,
        const float custom_perez_C,
        const float custom_perez_D,
        const float custom_perez_E,
        const float custom_perez_diffuse_horizontal_illuminance,
        const float custom_perez_direct_normal_illuminance,
        const TimeValue t,
        Interval& validity) const = 0;

    //! \brief Instantiates a shader from shading parameters obtained through CreateShadingParameters().
    //! See IPhysicalSunSky::InstantiateShader() for more details.
    virtual std::unique_ptr<IShader> InstantiateShader(const ShadingParameters& shading_parameters) const = 0;
};

}   // namespace

#include "IPhysicalSunSky.inline.h"
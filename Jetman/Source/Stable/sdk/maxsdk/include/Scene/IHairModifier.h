//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../color.h"
#include "../interval.h"
#include "../maxtypes.h"
#include "../containers/Array.h"

class INode;

namespace MaxSDK 
{;

//==================================================================================================
// class IHairModifier
//
//! \brief Provides access to hair strand definitions for a modifier that generates hair.
/*! The standard model for generating hair in 3ds Max is by applying a Hair Modifier to an existing
    geometry object. Renderer plugins that need the hair strand definition directly may use this interface
    to gather those definitions.

    To retrieve a pointer to this interface, use EnumGeomPipeline() on a given node and perform a dynamic_cast<IHairModifier>
    on the pointer returned by IDerivedObject::GetModifier().
*/
class IHairModifier
{
public:

    //! Defines the standard set of shading parameters for hair strands.
    struct ShadingParameters
    {
        //! The material to be used for shading this hair. If null, then the shading parameters below should be used
        //! instead.
        Mtl* shader = nullptr;
        //! The ambient diffuse term
        float ambdiff = 0.0f;
        //! Glossiness
        float gloss = 0.0f;
        //! Specularity
        float specular = 0.0f;
        //! Specular color
        Color specular_tint = Color(0.0f, 0.0f, 0.0f);
        //! Specular color for secondary highlights
        Color specular_tint2 = Color(0.0f, 0.0f, 0.0f);
    };

    /*! Returns whether this hair modifier is enabled.
        If this returns false, then this interface should be ignore altogether. This may happen, for
        example, if the hair modifier is set to generate geometry rather than hair primitives, in
        which case the Mesh evaluated for the node will contain the hair strands. */
    virtual bool IsHairEnabled() = 0;

    /*! Returns the shading parameters defined by this hair modifier.
        These are the shading parameters that should be used when rendering the hair. The material 
        assigned to the INode should be ignored in this case.
        \param t The time at which to fetch the shading parameters, which may be animated.
        \param [in, out] inout_validity The validity of the returned value is intersected into the input value of this parameter. */
    virtual ShadingParameters GetShadingParameters(const TimeValue t, Interval& inout_validity) = 0;

    /*! Fetches the hair modifier's definition of hair strands.
        The hair strands are defined as flat surfaces, facing the given normals. Each hair
        strand may have any number of vertices, the hair segments being linearly defined between
        the vertices.
		\param t The time at which to fetch the hair strands definition.
		\param [out] node The hair strand node.
        \param [out] per_hair_vertex_count The number of vertices present in each hair strand, with one value for each hair strand.
        \param [out] vertices The position of each vertex. The number of vertices equals the sum of all values in per_hair_vertex_count.
        \param [out] per_vertex_radius The radius of the hair strand, at each vertex.
        \param [out] per_vertex_color The color of the hair strand, at each vertex.
        \param [out] per_vertex_normal The facing direction of the hair strand, at each vertex.
        \param [out] per_vertex_velocity The velocity of each vertex - that is, the vector defining the direction
            and amplitude of movement for one full frame of animation.
        \param [out] per_vertex_opacity The opacity of each vertex.
        \param [out] per_vertex_uv The UV coordinate at each vertex.
        \return True on success, false on failure - for example, on failing to allocate memory. */
    virtual bool GetHairDefinition(
        const TimeValue t,
        INode& node,
        MaxSDK::Array<unsigned int>& per_hair_vertex_count,
        MaxSDK::Array<Point3>& vertices,
        MaxSDK::Array<float>& per_vertex_radius,
        MaxSDK::Array<Color>& per_vertex_color,
        MaxSDK::Array<Point3>& per_vertex_normal,
        MaxSDK::Array<Point3>& per_vertex_velocity,
        MaxSDK::Array<float>& per_vertex_opacity,
        MaxSDK::Array<Point2>& per_vertex_uv) = 0;
};

}   // namespace


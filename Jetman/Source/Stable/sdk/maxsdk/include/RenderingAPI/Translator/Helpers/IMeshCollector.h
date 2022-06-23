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

#include "../../RenderingAPI.h"

// Max SDK
#include <MaxHeap.h>
#include <maxtypes.h>
#include <Point3.h>
#include <gutil.h>
// std
#include <memory>
#include <vector>

class INode;
class View;
class Interval;

namespace MaxSDK
{;
namespace RenderingAPI
{;
namespace TranslationHelpers
{;

//==================================================================================================
// class IMeshCollector
//
/*! Utility interface for collecting the definition of a 3ds max mesh.

    This class abstracts the logic necessary for reading and interpreting class Mesh. Given an INode,
    it automatically evaluates its render mesh and generates well formatted, standardized definitions for
    vertex positions, normals, UVWs, tangents, etc.

    This class may not be the optimal way to read a render mesh: it stores its data in intermediate buffers
    which entail an extra copy into the renderer's database. But the fact that it's standardized and maintained
    internally should make it much easier for renderer plugin developers to translate 3ds max scenes.
*/
class IMeshCollector :
    public MaxHeapOperators
{
public:

    /*! Allocates a new mesh collector.
        The mesh collector, return by this method, should be treated as a temporary object. The mesh attributes should be extracted immediately, 
        followed by immediate discarding of the collector. This is because the mesh collector maintains references to the node and mesh, which could
        become dangling if the collector were kept around while other components were to evaluate the node.
        \param node The (geometric) node from which to evaluate the mesh.
        \param render_view The view to be passed to GeomObject::GetRenderMesh(). This view should implement View::CheckForRenderAbort() to always return
            false, because the mesh collector has no provisions for correctly supporting an abort while evaluating a mesh.
        \param t The time at which to evaluate the mesh.
        \param validity [in, out] The validity of the mesh is intersected into this interval.
        \return A new instance of a mesh collector */
    static RenderingAPIExport std::unique_ptr<IMeshCollector> AllocateInstance(INode& node, const View& render_view, const TimeValue t, Interval& validity);

    virtual ~IMeshCollector() {}

    //! Defines the vertex indices that form a triangular face in a mesh.
    struct FaceDefinition
    {
        //! Constructs a face definition using the three indices that define this triangular face.
        FaceDefinition(const unsigned int a, const unsigned int b, const unsigned int c);
        //! The vertex indices that define this triangular face.
        unsigned int indices[3];
    };

    /*! Returns the array of vertex positions used by the mesh.
        \remark The values returned are guaranteed to be valid (all NaN values get converted to 0.0). 
        \sa get_vertex_faces() */
    virtual const std::vector<Point3>& get_vertices() = 0;
    /*! Returns the array of face definitions for the vertex positions defined by get_vertices().
        \remark The values returned are guaranteed to be valid (all indices point to valid vertex indices). 
        \sa get_vertices() */
    virtual const std::vector<FaceDefinition>& get_vertex_faces() = 0;
    
    /*! Returns the array of normal vectors used by the mesh.
        \remark The values returned are guaranteed to be valid (all NaN values get converted to 0.0)
            and normalized; degenerate normals are converted to use the geometric normal.
        \sa get_normal_faces() */
    virtual const std::vector<Point3>& get_normals() = 0;
    /*! Returns the array of face definitions for the normals defined by get_normals().
        \remark The values returned are guaranteed to be valid (all indices point to valid vertex indices). 
        \remark The size of the array is guaranteed to be equal to that returng by get_vertex_faces().
        \sa get_normals() */
    virtual const std::vector<FaceDefinition>& get_normal_faces() = 0;

    /*! Returns the array of per-face material IDs.
        These material IDs are meant to be used in conjunction with multi/sub-object materials.
        \remark The size of the array is guaranteed to be equal to that returng by get_vertex_faces(). */
    virtual const std::vector<MtlID>& get_face_material_ids() = 0;

    /*! Returns the array of UVW channel IDs present on the mesh.
        These UVW channel IDs are meant to be matched with the channel IDs specified in a Texmap's
        UVW mapping parameters. */
    virtual const std::vector<int>& get_UVW_channel_ids() = 0;
    /*! Returns the array of UVW coordinates used by the mesh.
        \remark The values returned are guaranteed to be valid (all NaN values get converted to 0.0). 
        \remark This array is guaranteed to be the same size as that returned by get_UVW_channel_ids();
            each sub-array therefore corresponds to a specific UVW channel.
        \sa get_UVW_faces() */
    virtual const std::vector<std::vector<Point3>>& get_UVWs() = 0;
    /*! Returns the array of face definitions for the UVW coordinates defined by get_UVWs().
        \remark The values returned are guaranteed to be valid (all indices point to valid vertex indices).
        \remark This array is guaranteed to be the same size as that returned by get_UVW_channel_ids();
            each sub-array therefore corresponds to a specific UVW channel. The size of each sub-array 
            is guaranteed to be equal to that returng by get_vertex_faces().
        \sa get_UVWs() */
    virtual const std::vector<std::vector<FaceDefinition>>& get_UVW_faces() = 0;

    /*! Returns the array of UV tangent vectors used by the mesh.
        \remark The values returned are guaranteed to be valid (all NaN values get converted to 0.0). 
        \remark This array is guaranteed to be the same size as that returned by get_UVW_channel_ids();
            each sub-array therefore corresponds to a specific UVW channel.
        \sa get_UV_tangent_faces() */
    virtual const std::vector<std::vector<UVTangentVectors>>& get_UV_tangents() = 0;
    /*! Returns the array of face definitions for the UV tangent vectors defined by get_UV_tangents().
        \remark The values returned are guaranteed to be valid (all indices point to valid vertex indices). 
        \remark This array is guaranteed to be the same size as that returned by get_UVW_channel_ids();
            each sub-array therefore corresponds to a specific UVW channel. The size of each sub-array 
            is guaranteed to be equal to that returng by get_vertex_faces().
        \sa get_UV_tangents() */
    virtual const std::vector<std::vector<FaceDefinition>>& get_UV_tangent_faces() = 0;
};

//==================================================================================================
// class IMeshCollector inlined definitions
//==================================================================================================

inline IMeshCollector::FaceDefinition::FaceDefinition(const unsigned int a, const unsigned int b, const unsigned int c)
{
    indices[0] = a;
    indices[1] = b;
    indices[2] = c;
}

}}}      // namespace
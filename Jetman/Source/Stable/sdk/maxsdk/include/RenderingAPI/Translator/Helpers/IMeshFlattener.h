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

#include "../../RenderingAPI.h"

// Max SDK
#include <maxtypes.h>
#include <point2.h>
#include <point3.h>
#include <ipoint3.h>

// std
#include <vector>
#include <memory>
#include <functional>

class Mesh;
class MeshNormalSpec;
class INode;
class Interval;
class View;

namespace MaxSDK
{;
namespace RenderingAPI
{;
namespace TranslationHelpers
{;

//==================================================================================================
// class IMeshFlattener
//
/*! Utility interface for creating a flattened representation of a mesh.

    3ds Max meshes (class Mesh) are represented in a way that is optimized for manipulation and modelling. Renderers typically need a flattened
    representation, with separate arrays for vertices, normals, uvs, etc. - all with a one-to-one relationship. This interface performs this flattening
    process, as well as correctly evaluating the mesh (with normals, uvs, etc.) for the purpose of rendering.    
*/
class IMeshFlattener :
    public MaxHeapOperators
{
public:

    struct TextureCoordChannel;
    
    /*! Allocates a new mesh flattener and performs the flattening process.
        The mesh flattener, return by this method, should be treated as a temporary object. The mesh attributes should be extracted immediately, 
        followed by immediate discarding of the flattener. This is because the mesh flattener maintains references to the node and mesh, which could
        become dangling if the flattener were kept around while other components were to evaluate the node.
        \param node The (geometric) node from which to evaluate the mesh.
        \param render_view The view to be passed to GeomObject::GetRenderMesh(). This view should implement View::CheckForRenderAbort() to always return
            false, because the mesh flattener has no provisions for correctly supporting an abort while evaluating a mesh.
        \param t The time at which to evaluate the mesh.
        \param validity [in, out] The validity of the mesh is intersected into this interval.
        \return A new instance of a mesh flattener.
    */
    static RenderingAPIExport std::unique_ptr<IMeshFlattener> AllocateInstance(INode& node, const View& render_view, const TimeValue t, Interval& validity);

    virtual ~IMeshFlattener() {}

    //! Returns the number of sub-meshes that may be queried with GetSubMesh()
    virtual size_t GetNumSubMeshes() = 0;

    /*! Returns the flattened representation of the sub-mesh with the given index.
        The mesh flattener splits the mesh into multiple sub-meshes, according to material ID. Each material ID gets its own sub-mesh. This
        enables renderers, which may not support multiple materials per mesh, to easily support the multi-material feature by creating separate 
        geometric instances for each material used by the object.
        \param subMeshIndex The index of the sub-mesh to fetch.
        \param [out] materialId Returns the material ID associated with this sub-mesh.
        \param [out] faceIndices Returns the face definitions, with each face defined as a set of three indices into the vertices array.
        \param [out] vertices Returns the set of vertices.
        \param [out] normals The set of vertex normals, one for each vertex found in <i>vertices</i>.
        \param [out] textureCoordChannels The set of texture coordinate channel found on the mesh, each defining the vertex UVWs of a single channel.
    */
    virtual void GetSubMesh(
        const size_t subMeshIndex, 
        MtlID& materialId, 
        std::vector<IPoint3>& faceIndices, 
        std::vector<Point3>& vertices, 
        std::vector<Point3>& normals, 
        std::vector<TextureCoordChannel>& textureCoordChannels) = 0;
};

//==================================================================================================
// class IMeshFlattener::TextureCoordChannel
//
/*! Contains the vertex UVW coordinates for a single texture coordinate channel.

    A mesh may define an arbitrary number of UVW coordinate channels. Each channel associates a single UVW coordinate to each vertex in the mesh.
    Channels are defined by an ID (not an index), and such IDs are defined sparsely - i.e. a mesh may define UVW channels 0,1,5.
*/
struct IMeshFlattener::TextureCoordChannel
{
    //! The default constructor doesn't initialize anything explicitly.
    TextureCoordChannel() {}

    //! Initializes the container with the given channel ID and number of vertices.
    TextureCoordChannel(const unsigned int p_channel_id, const size_t num_coords) : channel_id(p_channel_id), coords(num_coords), tangentsU(num_coords), tangentsV(num_coords) {}

    //! The ID of the texture channel, as used with Mesh::mapVerts()
    unsigned int channel_id;

    //! The texture coordinate (UVW) values, one for each vertex in the mesh.
    std::vector<Point3> coords;

    //! The tangent basis vector, pointing in the U direction - one for each vertex in the mesh.
    std::vector<Point3> tangentsU;
    //! The tangent basis vector, pointing in the V direction - one for each vertex in the mesh.
    std::vector<Point3> tangentsV;
};

}}}      // namespace
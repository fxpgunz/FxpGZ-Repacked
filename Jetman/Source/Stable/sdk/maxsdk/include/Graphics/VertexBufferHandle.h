//
// Copyright 2010 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.   
//
//

#pragma once

#include "SmartHandle.h"
#include "GeometryEnums.h"
#include "RenderEnums.h"
#include "../Containers/Array.h"

namespace MaxSDK { namespace Graphics {

/** VertexBufferHandle is a memory buffer that contain vertex data. Vertex buffers can 
contain any vertex type - transformed or untransformed, lit or unlit - that can 
be rendered through the use of the rendering methods. 
*/
class VertexBufferHandle : public SmartHandle
{
public:
	GraphicsDriverAPI VertexBufferHandle();
	GraphicsDriverAPI VertexBufferHandle(const VertexBufferHandle& from);
	GraphicsDriverAPI VertexBufferHandle& operator = (const VertexBufferHandle& from);
	GraphicsDriverAPI virtual ~VertexBufferHandle();

public:
	/** Initialize this vertex buffer. 
	\param stride The stride in bytes of each vertex of the newly created buffer. 
	\return true if successfully initialized, false otherwise.
	*/
	//! \deprecated Deprecated method in 3ds Max 2017. Use full parameters to initialize vertex buffer
	MAX_DEPRECATED GraphicsDriverAPI bool Initialize(size_t stride);

	/** Initialize a vertex buffer with initial data
	\param stride The stride in bytes of each vertex of the newly created buffer.
	\param numVertices number of vertices in this buffer
	\param initialData initial data, can be nullptr
	\param usage usage of the buffer
	\param access access flag of the buffer
	\param pool pool state of the buffer	
	\param dataType if this buffer is used as "buffer" shader type, then it must specify data type.
	\return true if successfully initialized, false otherwise.
	*/
	GraphicsDriverAPI bool Initialize(size_t stride, 
		size_t numVertices, 
		void* initialData = nullptr, 
		BufferUsageType usage = BufferUsageStatic, 
		AccessType access = ReadWriteAcess, 
		BufferPoolType pool = BufferPoolTypeSystemBuffer,
		VertexFieldType dataType = VertexFieldTypeUnknown);

	/** Clear all vertices in the buffer and set the size of the buffer to zero.
	*/
	GraphicsDriverAPI void Clear();
	
	/** Get the size of each vertex of this buffer. Measured in bytes.
	\return the size in bytes of each vertex.
	*/
	GraphicsDriverAPI size_t GetVertexStride() const;

	/** Get the real number of vertices in the buffer
	\return the number of vertices.
	*/
	GraphicsDriverAPI size_t GetNumberOfVertices() const;

	/** Sets the real number of vertices in the buffer.
	\param numberOfVertices the new number of vertices.
	*/
	GraphicsDriverAPI void SetNumberOfVertices(size_t numberOfVertices);

	/** Get the capacity of the buffer
	\return the capacity
	*/
	GraphicsDriverAPI size_t GetCapacity() const;

	/** Sets the reserved capacity of the buffer.
	\param capacity capacity of the buffer
	*/
	MAX_DEPRECATED GraphicsDriverAPI void SetCapacity(size_t capacity);
	
	/** This function is used for reading/writing data from/into the vertex buffer.

	\remarks if the input parameter is invalid (eg. vertexIndex + numberOfVertices 
	is greater than the total number of vertices of the buffer), the function returns 
	NULL.

	\param vertexIndex the vertex index that the lock operation starts
	\param numberOfVertices the number of vertices that this function will lock.
	\param accessType behavior of the locking. Default is write access.
	\return the buffer for developers to read/write vertex data. The size 
	of the buffer is numberOfVertices * vertexStride.
	*/
	GraphicsDriverAPI unsigned char* Lock(size_t vertexIndex, size_t numberOfVertices, MaxSDK::Graphics::AccessType accessType = MaxSDK::Graphics::WriteAcess);

	/** Unlock the buffer to update. This function must be called after Lock. 
	Once this function is called, the buffer that returned by Lock become invalid 
	and should not be used any more.
	*/
	GraphicsDriverAPI void Unlock();

	/** Sets the buffer usage type.
	\param bufferType buffer usage type
	*/
	MAX_DEPRECATED GraphicsDriverAPI void SetBufferUsageType(BufferUsageType bufferType);

	/** Gets the buffer usage type
	\return the usage type
	*/
	GraphicsDriverAPI BufferUsageType GetBufferUsageType() const;

	/** Gets the buffer pool type.
	\return buffer pool type
	*/
	GraphicsDriverAPI BufferPoolType GetBufferPoolType() const;

	/** Gets the buffer access type.
	\return buffer access type
	*/
	GraphicsDriverAPI AccessType GetBufferAccessType() const;

	/** Clone the buffer
	\return the cloned buffer handle
	*/
	GraphicsDriverAPI VertexBufferHandle Clone();

	/** Copy data from another vertex buffer
	Note the source buffer format must match this buffer
	\param srcBuffer the source buffer to copy to this buffer
	*/
	GraphicsDriverAPI bool CopyFrom(SmartHandle& srcBuffer);

	/** Copy partial data from source buffer to specified location of this buffer
	Note both buffer should ensure they have enough space to copy the data
	\param srcBuffer the source buffer
	\param start offset of the source buffer
	\param numVertices number of vertices to copy
	\param destStart start offset  of this buffer	
	*/
	GraphicsDriverAPI bool CopyFrom(SmartHandle& srcBuffer, size_t start, size_t numVertices, size_t destStart);

	/** Copy data from hardware buffer to system buffer
	\param keepHWBuffer true to reserve hardware memory, false to release the memory.
	\return true if the data copy is success. Otherwise false.
	*/
	GraphicsDriverAPI bool RealizeToSysMemory(bool keepHWBuffer = false);

	/** Copy data from system buffer to hardware buffer
	\param keepSysBuffer true to reserve system memory, false to release the memory.
	\return true if the data copy is success. Otherwise false.
	*/
	GraphicsDriverAPI bool RealizeToHWMemory(bool keepSysBuffer = false);
};

typedef MaxSDK::Array<VertexBufferHandle> VertexBufferHandleArray;
GraphicsDriverAPI extern VertexBufferHandle EmptyVertexBufferHandle;
GraphicsDriverAPI extern MaxSDK::Array<VertexBufferHandle> EmptyVertexBufferHandleArray;
} } // end namespace

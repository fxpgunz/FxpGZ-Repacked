//
// Copyright 2011 Autodesk, Inc.  All rights reserved.
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

namespace MaxSDK { namespace Graphics {

/** IndexBufferHandle is a memory buffer that contain index data. Index data, or 
indices, are integer offsets into vertex buffers and are used to render primitives. 
*/
class IndexBufferHandle : public SmartHandle
{
public:
	GraphicsDriverAPI IndexBufferHandle();
	GraphicsDriverAPI IndexBufferHandle(const IndexBufferHandle& from);
	GraphicsDriverAPI IndexBufferHandle& operator = (const IndexBufferHandle& from);
	GraphicsDriverAPI virtual ~IndexBufferHandle();

public:
	/** Initialize the index buffer. 
	\param type The type of the newly created buffer. 
	\return true if successfully initialized, false otherwise.
	\deprecated Deprecated method in terms of implementation as of 3ds Max 2016. 
	*/ 
	MAX_DEPRECATED GraphicsDriverAPI bool Initialize(IndexType type);

	/** Initialize a index buffer with initial data
	\param type The type of the newly created buffer. 
	\param initialData initial data, can be nullptr
	\param numIndices number of indices in this buffer
	\param usage usage of the buffer
	\param access access flag of the buffer
	\param pool pool state of the buffer	
	\return true if successfully initialized, false otherwise.
	*/
	GraphicsDriverAPI bool Initialize(IndexType type, 
		size_t numIndices,
		void* initialData = nullptr, 
		BufferUsageType usage = BufferUsageStatic, 
		AccessType access = ReadWriteAcess, 
		BufferPoolType pool = BufferPoolTypeSystemBuffer);

	/** Get the type of the index buffer.
	\return The type of the index buffer as an IndexType 
	*/
	GraphicsDriverAPI IndexType GetType() const;

	/** Get the number of indices of this buffer.
	\return The number of indices
	*/
	GraphicsDriverAPI size_t GetNumberOfIndices() const;

	/** Set the number of indices of this buffer.
	\param numberOfIndices The size of the buffer
	*/
	MAX_DEPRECATED GraphicsDriverAPI void SetNumberOfIndices(size_t numberOfIndices);
	
	/** This function is used for reading/writing data from/into the index buffer.

	\param offset the start index that the lock operation starts
	\param numberOfIndices the number of indices that this function will lock.
	\param accessType indicate how to operate on the buffer.
	\return the buffer for developers to read/write index data. The size 
	of the buffer is numberOfIndices * indexStride.
	*/
	GraphicsDriverAPI unsigned char* Lock(size_t offset, size_t numberOfIndices, AccessType accessType);

	/** Unlock the buffer to update. This function must be called after Lock. 
	Once this function is called, the buffer that returned by Lock become invalid 
	and should not be used any more.
	*/
	GraphicsDriverAPI void Unlock();

	/** Sets the buffer usage type.
	\param usageType buffer usage type
	*/
	MAX_DEPRECATED GraphicsDriverAPI void SetBufferUsageType(BufferUsageType usageType);

	/** Gets the buffer usage type.
	\return buffer usage type
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
	GraphicsDriverAPI IndexBufferHandle Clone();

	/** Get start offset of the index buffer. 
	\return The offset
	*/
	GraphicsDriverAPI size_t GetStartOffset() const;

	/** Set start offset of the index buffer. This offset will be add to all indices.
	\param offset of the index buffer
	*/
	GraphicsDriverAPI void SetStartOffset(size_t offset);

	/** Copy data from another index buffer
	Note the source buffer format must match this buffer
	\param srcBuffer the source buffer to copy to this buffer
	*/
	GraphicsDriverAPI bool CopyFrom(IndexBufferHandle& srcBuffer);

	/** Copy partial data from source buffer to specified location of this buffer
	Note both buffer should ensure they have enough space to copy the data
	\param srcBuffer the source buffer
	\param start offset of the source buffer
	\param numIndices number of indices to copy
	\param destStart start offset  of this buffer
	*/
	GraphicsDriverAPI bool CopyFrom(IndexBufferHandle& srcBuffer, size_t start, size_t numIndices, size_t destStart);

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

GraphicsDriverAPI extern IndexBufferHandle EmptyIndexBufferHandle;
} } // namespaces

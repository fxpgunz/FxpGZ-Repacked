//
// Copyright 2010 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.   
//
//

#pragma once

#include "Types.h"
#include "DataBridgeExport.h"

namespace MaxSDK { namespace Graphics {

	/** types available for vertex fields, e.g. Float4
	*/
	enum VertexFieldType
	{
		/** single float field
			see DXGI_FORMAT_R32_FLOAT
		*/
		VertexFieldFloat1 = 0, 

		/** double float field. x,y
			see DXGI_FORMAT_R32G32_FLOAT
		*/
		VertexFieldFloat2 = 1, 

		/** triple float field
			see DXGI_FORMAT_R32G32B32_FLOAT
		*/
		VertexFieldFloat3 = 2, 

		/** quad float field
			see DXGI_FORMAT_R32G32B32A32_FLOAT
		*/
		VertexFieldFloat4 = 3, 

		/** color field
			see DXGI_FORMAT_R8G8B8A8_UNORM
		*/
		VertexFieldColor = 4, 

		/** unsigned byte 4 field
		    see DXGI_FORMAT_R8G8B8A8_UINT
		*/
		VertexFieldByte4 = 5, 

		/** short 2 field
			see DXGI_FORMAT_R16G16_SINT
		*/
		VertexFieldShort2 = 6, 

		/** short 4 field
			see DXGI_FORMAT_R16G16B16A16_SINT
		*/
		VertexFieldShort4 = 7, 
		/** FP16 x 2 type
			see DXGI_FORMAT_R16G16_FLOAT
		*/
		VertexFieldShortFloat2 = 15, 

		/** FP16 x 4 type
			see DXGI_FORMAT_R16G16B16A16_FLOAT
		*/
		VertexFieldShortFloat4 = 16,

		/** int field
		    see DXGI_FORMAT_R32_SINT
		*/
		VertexFieldInt = 20,
		/** int2 field
			see DXGI_FORMAT_R32G32_SINT
		*/
		VertexFieldInt2 = 21,
		/** int3 field
			see DXGI_FORMAT_R32G32B32_SINT
		*/
		VertexFieldInt3 = 22,
		/** int4 field
			see DXGI_FORMAT_R32G32B32A32_SINT
		*/
		VertexFieldInt4 = 23,

		/** unknown field
		*/
		VertexFieldTypeUnknown, 

		VertexFieldTypeCount, 
	};

	/** Indicates the usage of a vertex field of a vertex buffer. This enum is 
	used to make hardware shaders understand the usage of vertex buffer.
	*/
	enum VertexFieldUsage
	{
		/** Position
		*/
		VertexFieldUsagePosition = 0, 

		/** Normal
		*/
		VertexFieldUsageNormal, 

		/** Texture coordinate
		*/
		VertexFieldUsageTexcoord, 

		/** tangent
		*/
		VertexFieldUsageTangent, 

		/** bitangent
		*/
		VertexFieldUsageBitangent, 
		
		/** Unknown usage.
		*/
		VertexFieldUsageUnknown, 

		VertexFieldUsageCount, 
	};

	/** Indicates the data source of a vertex field of a mesh vertex buffer. e.g. 
	Position, Normal, Texture, etc. 
	*/
	enum MeshChannelCategory
	{
		/** position
		*/
		MeshChannelPosition = 0, 

		/** vertex normal components
		*/
		MeshChannelVertexNormal, 

		/** texture coordinate components
		*/
		MeshChannelTexcoord, 
		/** tangent components
		*/
		MeshChannelTangent, 

		/** bitangent components
		*/
		MeshChannelBitangent, 
		
		/** vertex color components
		*/
		MeshChannelVertexColor, 
	
		/** Unknown data source.
		*/
		MeshChannelUnknown, 

		/** face map components
		*/
		MeshChannelFaceMap, 

		MeshChannelCategoryCount, 
	};

	/** Buffers are composed of arrays of elements all of the same type.
	This enum describes the size and interpretation of the elements in the buffer.
	Index buffers can have ushort or uint elements. 
	*/
	enum IndexType
	{
		/** not defined
		*/
		IndexTypeUnknown,

		/** unsigned 16 bit short integer, used for index buffers.
		*/
		IndexTypeShort, 

		/** unsigned 32 bit integer, used for index buffers.
		*/
		IndexTypeInt, 

		IndexTypeCount, 
	};

	/** Get the vertex stride by a given vertex type, in bytes. 
	\param type the vertex type.
	\return the vertex stride, in bytes.
	*/
	DataBridgeAPI size_t GetVertexStride(const VertexFieldType type);

	/** Get the index stride by a given index type, in bytes. 
	\param type the index type.
	\return the index stride, in bytes.
	*/
	DataBridgeAPI size_t GetIndexStride(const IndexType type);

	/** BufferUsageType Types for OGS buffer usage.
	*/
	enum BufferUsageType
	{
		/** static usage type. Data stores in GPU memory.
		*/
		BufferUsageStatic = 0,
		
		/** Dynamic buffer.
		    If the resource contents are to be changed often, EDynamicUsage is recommended to achieve fast update.
		    Used with the correct map flag( WriteDiscard or WriteNoOverwrite), maximum performance to change the
			contents will be achieved.
			Meaning of WriteNoOverwrite:
			WriteNoOverwrite signifies that the application promises not to write to data that the GPU is using. In exchange, 
			the GPU allows the application to write to other parts of the same buffer. The application must ensure that it does 
			not write over any data in use by the GPU.        
			Common Usage of WriteDiscard with WriteNoOverwrite as map flags:
			WriteDiscard and WriteNoOverwrite are normally used in conjunction with dynamic index/vertex buffers, 
			although they can also be used with dynamic textures. A common use of these two flags involves filling 
			dynamic index/vertex buffers with geometry that can be seen from the camera's current position. The first 
			time that data is entered into the buffer on a given frame, Map is called with WriteDiscard; doing so invalidates 
			the previous contents of the buffer. The buffer is then filled with all available data. Subsequent writes to the 
			buffer within the same frame should use WriteNoOverwrite. This will enable the CPU to access a resource that is
			potentially being used by the GPU as long as the restrictions described previously are respected.    
		*/
		BufferUsageDynamic,

		/* Staging usage, means it's a system memory resource
		*/
		BufferUsageStaging,

		/*for anding off the high bits
		*/
		BufferUsageMask = 0xf,       

		/** Usage for stream output
			for vb & ibs, may be used as a texture
		*/
		BufferUsageStreamTexture = 0x10,

		/** Usage for render target, it means the resouce created from video memory and
		    can be used as render-target
		    for vb & ibs & textures, may be used as a render target/stream output buffer
		*/
		BufferUsageTarget = 0x20,

		/** Usage for unordered access of the resource.
			This allows the resource to be both read and written by a shader, through the use of
			atomic functions.  Check the device capabilities to determine support.
		*/
		BufferUsageUnorderedAccess = 0x80,

		/** Usage for structured buffer.
		If this flag is specified, the buffer must use BufferPoolTypeHardwareBuffer for pool type
		*/
		BufferUsageStructure = 0x100,

		/** Usage for bytes address buffer.
		*/
		BufferUsageRaw = 0x200,

		/** Usage for standard buffer type
		If this flag is specified, user must specify dataType parameter of VertexBufferHandle::Initialize()
		*/
		BufferUsageBuffer = 0x400
	};

	enum BufferPoolType
	{	
		/** Buffer state unknown 
		*/
		BufferPoolTypeUnKnown = 0,

		/** Buffer on disk 
		*/
		BufferPoolTypeDiskBuffer = 0x1,

		/** Buffer in system memory
		*/
		BufferPoolTypeSystemBuffer = 0x1 << 1 ,

		/** Buffer on GPU memory 
		*/
		BufferPoolTypeHardwareBuffer = 0x1 << 2,

		/** Buffer in memory mask, both system and GPU memory.
		*/
		BufferPoolTypeInMemory = BufferPoolTypeSystemBuffer | BufferPoolTypeHardwareBuffer,
	};

} } // end namespace


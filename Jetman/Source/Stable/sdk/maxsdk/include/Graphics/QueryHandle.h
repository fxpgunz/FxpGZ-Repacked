//
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.   
//
//

#pragma once

#include "SmartHandle.h"
#include "RenderEnums.h"

namespace MaxSDK { namespace Graphics {

	struct Query_Data_SO_Statistics {
		UINT64 NumPrimitivesWritten;
		UINT64 PrimitivesStorageNeeded;
	} ;

/** QueryHandle is a query object used for submitting hardware queries.
    Typically use QueryType_Occlusion to query any pixels rendered,
	use QueryType_SO_Stats to query geometry shader output statistics etc.
*/
class QueryHandle : public SmartHandle
{
public:
	GraphicsDriverAPI QueryHandle();
	GraphicsDriverAPI QueryHandle(const QueryHandle& from);
	GraphicsDriverAPI QueryHandle& operator = (const QueryHandle& from);
	GraphicsDriverAPI virtual ~QueryHandle();

public:
	/** Initialize this device query 
	\param type query type to issue.
	\return true if successfully initialized, false otherwise.
	*/
	GraphicsDriverAPI bool Initialize(QueryType type);

	/** Issue the query
	    Call this function before IVirtualDevice::Draw() to get information from the Draw call.
	*/
	GraphicsDriverAPI void Begin();

	/** End the query
	    Call this function after IVirtualDevice::Draw() to end the query
	*/
	GraphicsDriverAPI void End();

	/** Get the query data's size
	*/
	GraphicsDriverAPI size_t GetDataSize() const;

	/** Get the query data
	\param data A pointer to the returned data, which depends on the query type.  Only filled if the
				function returns true.  Must not be nullptr.
	\param size size of the data, query from GetDataSize()
	\param flush Whether to flush the device's command buffer when checking for data.  If flushing is not
				performed, the client may have to wait indefinitely for a result.
	\return False if the data is not ready, and the client must keep waiting.  True if the data is
			ready to use.
	*/
	GraphicsDriverAPI bool GetData(void* data, size_t size, bool flush);

};

} } // end namespace

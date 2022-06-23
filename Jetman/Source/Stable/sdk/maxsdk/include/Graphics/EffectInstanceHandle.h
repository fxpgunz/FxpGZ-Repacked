//
// Copyright 2013 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.   
//
//

#pragma once

#include "TextureHandle.h"
#include "TargetHandle.h"
#include "VertexBufferHandle.h"
#include "../strclass.h"
#include "../point4.h"
#include "../point3.h"
#include "../Ipoint2.h"
#include "../Ipoint3.h"

namespace MaxSDK { namespace Graphics {

class Matrix44;
class EffectHandle;
class DrawContext;

/** EffectInstanceHandle is a class which stores values of parameters in an effect.
Typically, EffectHandle is a singleton which represent a single ".fx" file. And each render item
contains an EffectInstanceHandle to store their own values. Use EffectInstanceHandle::Apply() to 
apply values to the device.
This class is used for viewport display and Quicksilver.
How to use:
\code
//Initialize the effect
EffectHandle hEffect;
EffectInstanceHandle hEffectInstance;
MSTR errorInfo;
hEffect.InitializeWithFile(shaderFileName, &errorInfo);
hEffectInstance = hEffect.CreateEffectInstance();
hEffectInstance.SetFloatParameter(_M("myFloatShaderParam"), floatValue);
...

//Before draw calls:
//Render with the effect
hEffect.Activate(dc); //Activate the effect first
hEffectInstance0.Apply(dc);  //Apply values to the effect
for (int i = 0; i < hEffect.GetPassCount(dc); ++i)
{
	hEffect.ActivatePass(dc, i); //Apply each pass
	pVirtualDevice.Draw(...);
}
hEffect.PassesFinished(dc); Finish this effect instance

hEffectInstance1.Apply(dc); //Apply next instance, which parent effect is same as above.
for (int i = 0; i < hEffect.GetPassCount(dc); ++i)
{
	hEffect.ActivatePass(dc, i); //Apply and draw this instance
	pVirtualDevice.Draw(...);
}
hEffect.PassesFinished(dc);

hEffect.Terminate(); //End of the effect usage.
\endcode
*/
class EffectInstanceHandle : public SmartHandle
{
	friend class EffectHandle;
protected:
	GraphicsDriverAPI EffectInstanceHandle(EffectHandle& parent);
public:
	GraphicsDriverAPI EffectInstanceHandle();
	GraphicsDriverAPI EffectInstanceHandle(const EffectInstanceHandle& from);
	GraphicsDriverAPI EffectInstanceHandle& operator = (const EffectInstanceHandle& from);
	GraphicsDriverAPI virtual ~EffectInstanceHandle();
	GraphicsDriverAPI EffectHandle ParentEffect();
	GraphicsDriverAPI void Apply(DrawContext& dc);

		
	/** Returns the vertex format of this effect instance
	\return the material required streams.
	*/
	GraphicsDriverAPI virtual const MaterialRequiredStreams& GetRequiredStreams() const;

	/** Sets int parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetIntParameter(const MSTR& parameterName, int value);

	/** Sets int parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetIntParameter(int index, int value);

	/** Sets int2 parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetIntParameter(const MSTR& parameterName, IPoint2& value);
	/** Sets int2 parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetIntParameter(int index, IPoint2& value);

	/** Sets int3 parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetIntParameter(const MSTR& parameterName, IPoint3& value);

	/** Sets int3 parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetIntParameter(int index, IPoint3& value);
	
	/** Sets int array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetIntArrayParameter(const MSTR& parameterName, const int* value, unsigned int count);

	/** Sets int array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetIntArrayParameter(int index, const int* value, unsigned int count);

	/** Sets int2 array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetIntArrayParameter(const MSTR& parameterName, const IPoint2* value, unsigned int count);
	
	/** Sets int2 array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetIntArrayParameter(int index, const IPoint2* value, unsigned int count);

	/** Sets int3 array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetIntArrayParameter(int index, const IPoint3* value, unsigned int count);

	/** Sets int3 array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetIntArrayParameter(const MSTR& parameterName, const IPoint3* value, unsigned int count);

	/** Sets uint parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetUIntParameter(const MSTR& parameterName, unsigned int value);

	/** Sets uint parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The int parameter value.
	*/
	GraphicsDriverAPI void SetUIntParameter(int index, unsigned int value);

	/** Sets uint array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetUIntArrayParameter(const MSTR& parameterName, const unsigned int* value, unsigned int count);

	/** Sets uint array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of int array parameter value.
	\param[in] count The count of int array.
	*/
	GraphicsDriverAPI void SetUIntArrayParameter(int index, const unsigned int* value, unsigned int count);

	/** Sets float parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(const MSTR& parameterName, float value);

	/** Sets float parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(int index, float value);
			
	/** Sets float2 parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(const MSTR& parameterName, const Point2& value);

	/** Sets float2 parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(int index, const Point2& value);

	/** Sets float3 parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(const MSTR& parameterName, const Point3& value);

	/** Sets float3 parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(int index, const Point3& value);	
	
	/** Sets float4 parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(const MSTR& parameterName, const Point4& value);

	/** Sets float4 parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetFloatParameter(int index, const Point4& value);
	
	/** Sets float array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of float array parameter value.
	\param[in] count The count of float array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(int index, const float* value, unsigned int count);

	/** Sets float array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of float array parameter value.
	\param[in] count The count of float array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(const MSTR& parameterName, const float* value, unsigned int count);

	/** Sets float2 array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of float array parameter value.
	\param[in] count The count of float array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(int index, const Point2* value, unsigned int count);

	/** Sets float2 array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of float array parameter value.
	\param[in] count The count of float array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(const MSTR& parameterName, const Point2* value, unsigned int count);

	/** Sets float3 array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of float3 array parameter value.
	\param[in] count The count of float3 array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(int index, const Point3* value, unsigned int count);
	
	/** Sets float3 array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of float3 array parameter value.
	\param[in] count The count of float3 array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(const MSTR& parameterName, const Point3* value, unsigned int count);

	/** Sets float4 array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of float4 array parameter value.
	\param[in] count The count of float4 array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(const MSTR& parameterName, const Point4* value, unsigned int count);

	/** Sets float4 array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of float4 array parameter value.
	\param[in] count The count of float4 array.
	*/
	GraphicsDriverAPI void SetFloatArrayParameter(int index, const Point4* value, unsigned int count);
	
	/** Sets bool parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetBoolParameter(const MSTR& parameterName, bool value);

	/** Sets bool parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetBoolParameter(int index, bool value);

	/** Sets bool array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of bool array parameter value.
	\param[in] count The count of bool array.
	*/
	GraphicsDriverAPI void SetBoolArrayParameter(const MSTR& parameterName, const bool* value, unsigned int count);
	/** Sets bool array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of bool array parameter value.
	\param[in] count The count of bool array.
	*/
	GraphicsDriverAPI void SetBoolArrayParameter(int index, const bool* value, unsigned int count);
	
	/** Sets matrix parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetMatrixParameter(const MSTR& parameterName, const Matrix44& value);

	/** Sets matrix parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The parameter value.
	*/
	GraphicsDriverAPI void SetMatrixParameter(int index, const Matrix44& value);
		
	/** Sets matrix array parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] value The pointer of matrix array parameter value.
	\param[in] count The count of matrix array.
	*/
	GraphicsDriverAPI void SetMatrixArrayParameter(const MSTR& parameterName, const Matrix44* value, unsigned int count);

	/** Sets matrix array parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] value The pointer of matrix array parameter value.
	\param[in] count The count of matrix array.
	*/
	GraphicsDriverAPI void SetMatrixArrayParameter(int index, const Matrix44* value, unsigned int count);

	/** Sets texture parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] textureHandle The parameter texture handle value.
	*/
	GraphicsDriverAPI void SetTextureParameter(const MSTR& parameterName, const TextureHandle& textureHandle);

	/** Sets texture parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] textureHandle The parameter texture handle value.
	*/
	GraphicsDriverAPI void SetTextureParameter(const MSTR& parameterName, const TargetHandle& textureHandle);

	/** Sets texture parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] textureHandle The parameter texture handle value.
	*/
	GraphicsDriverAPI void SetTextureParameter(int index, const TextureHandle& textureHandle);

	/** Sets texture parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] targetHandle The parameter target handle value.
	*/
	GraphicsDriverAPI void SetTextureParameter(int index, const TargetHandle& targetHandle);
	
	/** Sets buffer parameter.
	\param[in] parameterName The unique parameter name of the parameter.
	\param[in] bufferHandle The parameter VertexBufferHandle or IndexBufferHandle value.
			   Note this buffer should be created with BufferUsageStatic | BufferUsageTarget usage flag
	*/
	GraphicsDriverAPI void SetBufferParameter(const MSTR& parameterName, const SmartHandle& bufferHandle);
	
	/** Sets buffer parameter.
	\param[in] index The parameter index of the parameter.
	\param[in] bufferHandle The parameter VertexBufferHandle or IndexBufferHandle value.
			   Note this buffer should be created with BufferUsageStatic | BufferUsageTarget usage flag
	*/
	GraphicsDriverAPI void SetBufferParameter(int index, const SmartHandle& bufferHandle);
};


} } // end namespace
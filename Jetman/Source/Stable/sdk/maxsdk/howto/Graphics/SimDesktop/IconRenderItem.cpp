/*===========================================================================*\
 |    File: IconRenderItem.cpp
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./IconRenderItem.h"
#include <Graphics/IVirtualDevice.h>

namespace MaxSDK { namespace Graphics {

const float ICON_SIZE = 128.0f;

IconRenderItem::IconRenderItem()
{
	// do nothing.
}

IconRenderItem::~IconRenderItem()
{
	// do nothing, since the destructor of smart handles will release itself.
}

bool IconRenderItem::SetMaterial(BaseMaterialHandle hMaterial)
{
	if (!hMaterial.IsValid())
	{
		// Invalid material handle
		return false;
	}

	const MaterialRequiredStreams* pRequiredStreams = hMaterial.GetRequiredStreams();
	if (pRequiredStreams == NULL)
	{
		// The material doesn't have requirement
		return false;
	}

	if (pRequiredStreams->GetNumberOfStreams() == 0)
	{
		// Invalid material requirement
		return false;
	}

	// Construct vertex buffers
	ConstructBuffers();

	mMultiStreamRequirements.Clear();
	mVertexBufferArray.removeAll();

	{
		MaterialRequiredStreamElement multiStreamElement;
		multiStreamElement.SetType(VertexFieldFloat3);
		multiStreamElement.SetChannelCategory(MeshChannelPosition);
		multiStreamElement.SetStreamIndex((unsigned char)(mVertexBufferArray.length()));
		mVertexBufferArray.append(mPositionBuffer);
		mMultiStreamRequirements.AddStream(multiStreamElement);
	}
	{
		MaterialRequiredStreamElement multiStreamElement;
		multiStreamElement.SetType(VertexFieldFloat3);
		multiStreamElement.SetChannelCategory(MeshChannelVertexNormal);
		multiStreamElement.SetStreamIndex((unsigned char)(mVertexBufferArray.length()));
		mVertexBufferArray.append(mNormalBuffer);
		mMultiStreamRequirements.AddStream(multiStreamElement);
	}
	{
		MaterialRequiredStreamElement multiStreamElement;
		multiStreamElement.SetType(VertexFieldFloat3);
		multiStreamElement.SetChannelCategory(MeshChannelTexcoord);
		multiStreamElement.SetStreamIndex((unsigned char)(mVertexBufferArray.length()));
		mVertexBufferArray.append(mTexcoordBuffer);
		mMultiStreamRequirements.AddStream(multiStreamElement);
	}

	/*
	for (size_t i = 0; i < pRequiredStreams->GetNumberOfStreams(); ++i)
	{
		const MaterialRequiredStreamElement& requiredElement = pRequiredStreams->GetStreamElement(i);
		MaterialRequiredStreamElement multiStreamElement = requiredElement;
		multiStreamElement.SetStreamIndex((unsigned char)(mVertexBufferArray.length()));
		switch (requiredElement.GetChannelCategory())
		{
		case MeshChannelPosition:
			mVertexBufferArray.append(mPositionBuffer);
			mMultiStreamRequirements.AddStream(multiStreamElement);
			break;
		case MeshChannelVertexNormal:
			mVertexBufferArray.append(mNormalBuffer);
			mMultiStreamRequirements.AddStream(multiStreamElement);
			break;
		case MeshChannelTexcoord:
			mVertexBufferArray.append(mTexcoordBuffer);
			mMultiStreamRequirements.AddStream(multiStreamElement);
			break;
		default:
			// The material requires channel that this render item doesn't support.
			return false;
		}
	}
	*/

	mMaterial = hMaterial;

	return true;
}

BaseMaterialHandle IconRenderItem::GetMaterial() const
{
	return mMaterial;
}

void IconRenderItem::Realize(DrawContext& drawContext)
{
	// do nothing.
}

void IconRenderItem::Display(DrawContext& drawContext)
{
	if (!mMaterial.IsValid())
	{
		return;
	}

	IVirtualDevice& vd = drawContext.GetVirtualDevice();
	vd.SetStreamFormat(mMultiStreamRequirements);
	vd.SetVertexStreams(mVertexBufferArray);
	vd.SetIndexBuffer(mIndexBuffer);
	BlendState oldBlendState = vd.GetBlendState();
	BlendState newBlendState = oldBlendState;
	TargetBlendState& targetState = newBlendState.GetTargetBlendState(0);
	targetState.SetBlendEnabled(true);
	targetState.SetSourceBlend(BlendSelectorSourceAlpha);
	targetState.SetDestinationBlend(BlendSelectorInvSourceAlpha);
	targetState.SetAlphaSourceBlend(BlendSelectorSourceAlpha);
	targetState.SetAlphaDestinationBlend(BlendSelectorInvSourceAlpha);
	targetState.SetAlphaBlendOperation(BlendOperationAdd);
	targetState.SetColorBlendOperation(BlendOperationAdd);

	mMaterial.Activate(drawContext);
	unsigned int passCount = mMaterial.GetPassCount(drawContext);
	for (unsigned int i = 0; i < passCount; ++i)
	{
		mMaterial.ActivatePass(drawContext, i);
		//vd.SetBlendState(newBlendState);
		vd.Draw(PrimitiveTriangleList, 0, 2);
		//vd.SetBlendState(oldBlendState);
		mMaterial.PassesFinished(drawContext);
	}
	mMaterial.Terminate();
}

size_t IconRenderItem::GetPrimitiveCount() const
{
	return 2;
}

void IconRenderItem::ConstructBuffers()
{
	if (mPositionBuffer.IsValid())
	{
		// avoid duplicate construction.
		return;
	}

	// Construct the buffer in left-hand coordinate system
	//  y
	//  ^  z
	//  | /
	//  |/
	//  +-----> x

	mPositionBuffer.Initialize(sizeof(Point3),4);
	Point3* pPosition = (Point3*)(mPositionBuffer.Lock(0, 0, WriteAcess));
	pPosition[0] = Point3(-ICON_SIZE/2.0f,  ICON_SIZE/2.0f, 0.0f);
	pPosition[1] = Point3( ICON_SIZE/2.0f,  ICON_SIZE/2.0f, 0.0f);
	pPosition[2] = Point3(-ICON_SIZE/2.0f, -ICON_SIZE/2.0f, 0.0f);
	pPosition[3] = Point3( ICON_SIZE/2.0f, -ICON_SIZE/2.0f, 0.0f);
	mPositionBuffer.Unlock();

	mNormalBuffer.Initialize(sizeof(Point3),4);
	Point3* pNormal = (Point3*)(mNormalBuffer.Lock(0, 0, WriteAcess));
	pNormal[0] = Point3(0, 0, -1);
	pNormal[1] = Point3(0, 0, -1);
	pNormal[2] = Point3(0, 0, -1);
	pNormal[3] = Point3(0, 0, -1);
	mNormalBuffer.Unlock();

	mTexcoordBuffer.Initialize(sizeof(Point3),4);
	Point3* pTexcoord = (Point3*)(mTexcoordBuffer.Lock(0, 0, WriteAcess));
	pTexcoord[0] = Point3(0.01f, 0.01f, 0.0f);
	pTexcoord[1] = Point3(0.99f, 0.01f, 0.0f);
	pTexcoord[2] = Point3(0.01f, 0.99f, 0.0f);
	pTexcoord[3] = Point3(0.99f, 0.99f, 0.0f);
	mTexcoordBuffer.Unlock();

	mIndexBuffer.Initialize(IndexTypeShort,6);
	unsigned short* pIndices = (unsigned short*)(mIndexBuffer.Lock(0, 0, WriteAcess));
	pIndices[0] = 0;
	pIndices[1] = 2;
	pIndices[2] = 1;
	pIndices[3] = 2;
	pIndices[4] = 3;
	pIndices[5] = 1;
	mIndexBuffer.Unlock();
}

} } // namespaces

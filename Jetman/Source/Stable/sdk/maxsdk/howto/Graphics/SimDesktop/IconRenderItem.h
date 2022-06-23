/*===========================================================================*\
 |    File: IconRenderItem.h
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/ICustomRenderItem.h>
#include <Graphics/BaseMaterialHandle.h>
#include <Graphics/VertexBufferHandle.h>
#include <Graphics/IndexBufferHandle.h>

namespace MaxSDK { namespace Graphics {

extern const float ICON_SIZE;

class IconRenderItem : public ICustomRenderItem
{
public:
	IconRenderItem();
	virtual ~IconRenderItem();

public:
	bool SetMaterial(BaseMaterialHandle hMaterial);
	BaseMaterialHandle GetMaterial() const;

public:
	virtual void Realize(DrawContext& drawContext);
	virtual void Display(DrawContext& drawContext);
	virtual size_t GetPrimitiveCount() const;

private:
	void ConstructBuffers();

private:
	BaseMaterialHandle mMaterial;

	VertexBufferHandle mPositionBuffer;
	VertexBufferHandle mNormalBuffer;
	VertexBufferHandle mTexcoordBuffer;

	MaterialRequiredStreams mMultiStreamRequirements;
	VertexBufferHandleArray mVertexBufferArray;
	IndexBufferHandle mIndexBuffer;
};

} } // namespaces

/*===========================================================================*\
 |    File: IconWorld.cpp
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./IconWorld.h"
#include "./IconRenderItem.h"
#include "./IconMaterial.h"

// Declaration of ghInstance
#include "./SimDesktopMain.h"
// Definition of IDR_JPG*
#include "./resource.h"

#include <Graphics/SolidColorMaterialHandle.h>

namespace MaxSDK { namespace Graphics {

IconWorld::IconWorld()
{
	mInitialized = false;
}

IconWorld::~IconWorld()
{
	for (size_t i = 0; i < mBitmaps.size(); ++i)
	{
		delete mBitmaps[i];
	}
	mBitmaps.clear();

	mInitialized = false;
}

bool IconWorld::Initialize()
{
	if (mInitialized)
	{
		// Cannot initialize twice!
		return false;
	}

	mInitialized = true;

	mRenderWorld.Initialize();

	// Load bitmaps.
	for (size_t i = 0; i < 31; ++i)
	{
		CGdiBitmap* pBitmap = new CGdiBitmap;
		if (!pBitmap->loadResource((UINT)(IDR_JPG1 + i), _T("JPG"), ghInstance))
		{
			delete pBitmap;
			continue;
		}

		TextureHandle hTexture = CreateTexture(pBitmap);
		mTextures.push_back(hTexture);
		mBitmaps.push_back(pBitmap);

		RenderNodeHandle hNode = CreateRenderNode(hTexture);
		if (!hNode.IsValid())
		{
			continue;
		}

		// Specify the node matrix
		Matrix44 matWorld;
		matWorld.MakeIdentity();
		hNode.SetWorldMatrix(matWorld);

		mRenderWorld.AddNode(hNode);
	}

	mRenderWorld.UpdateNodes();

	return true;
}

RenderWorldHandle IconWorld::GetWorldHandle()
{
	return mRenderWorld;
}

void IconWorld::UpdateIcons(const ViewParameter& view)
{
	int viewWidth = (int)(view.GetWidth());
	int viewHeight = (int)(view.GetHeight());

	int iconGap = 10;

	int iconBeginX = -viewWidth / 2 + iconGap;
	int iconBeginY = viewHeight / 2 - iconGap - 15;

	int iconEndX = viewWidth / 2 - (int)ICON_SIZE - iconGap;
	int iconEndY = -viewHeight / 2 + (int)ICON_SIZE + iconGap;

	int x = iconBeginX;
	int y = iconBeginY;

	RenderNodeIterator it = mRenderWorld.NodeBegin();
	for (; it != mRenderWorld.NodeEnd(); it.Increment())
	{
		RenderNodeHandle hRenderNode = (*it);

		float centerX = (float)x + ICON_SIZE / 2;
		float centerY = (float)y - ICON_SIZE / 2;
		Matrix44 matWorld;
		matWorld.MakeTranslation(Point3(centerX, centerY, 0.0f));

		hRenderNode.SetWorldMatrix(matWorld);

		x += (int)(iconGap + ICON_SIZE);
		if (x > iconEndX)
		{
			x = iconBeginX;
			y -= (int)(iconGap + ICON_SIZE);
		}
	}

	mRenderWorld.UpdateNodes();
}

RenderNodeHandle IconWorld::CreateRenderNode(TextureHandle hTexture)
{
	BaseMaterialHandle hMaterial = CreateMaterial(hTexture);

	IconRenderItem* pRenderItemImpl = new IconRenderItem;
	pRenderItemImpl->SetMaterial(hMaterial);

	CustomRenderItemHandle hRenderItem;
	hRenderItem.Initialize();
	hRenderItem.SetCustomImplementation(pRenderItemImpl);
	hRenderItem.SetVisibilityGroup(RenderItemVisible_Shaded);

	RenderNodeHandle hRenderNode;
	hRenderNode.Initialize();
	hRenderNode.AddRenderItem(hRenderItem);
	Box3 bbox(
		Point3(-ICON_SIZE/2.0f, -ICON_SIZE/2.0f, -1.0f), 
		Point3(ICON_SIZE/2.0f, ICON_SIZE/2.0f, 1.0f));
	hRenderNode.SetObjectBox(bbox);

	return hRenderNode;
}

} } // namespaces

/*===========================================================================*\
 |    File: CustomBackgroundUtils.h
 |
 | Purpose: Demonstrate custom background in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./CustomBackgroundUtils.h"
#include "./CResource.h"
#include "./resource.h"
#include "./CustomBackgroundMain.h"
#include <Graphics/SimpleRenderGeometry.h>
#include <Graphics/GeometryRenderItemHandle.h>
#include <Graphics/RenderNodeHandle.h>
#include <Graphics/DeviceCaps.h>
#include <Graphics/IDisplayManager.h>

namespace MaxSDK { namespace Graphics {

	bool loadShader(HLSLMaterialHandle& hMaterial, UINT shaderID)
	{
		CResource resource;
		if (resource.load(shaderID, _T("SHADER"), ghInstance))
		{
			CStr str;
			str.Resize((int)(resource.getSize() + 1));
			if (resource.getSize() > 0)
			{
				char* pStr = str.dataForWrite();
				memcpy(pStr, resource.getData(), resource.getSize());

				pStr[resource.getSize()] = '\0';
			}

			hMaterial.InitializeWithString(str);
		}

		return true;
	}

	bool isDX11()
	{
		MaxSDK::Graphics::DeviceCaps caps;
		GetIDisplayManager()->GetDeviceCaps(caps);
		if (Level5_0 == caps.FeatureLevel)
		{
			return true;
		}

		return false;
	}

	UINT GetShaderID_Draw()
	{
		if (isDX11())
		{
			return IDR_FX_DX11_DRAW;
		}
		else
		{
			return IDR_FX_DX9_DRAW;
		}
	}

	HLSLMaterialHandle CreateMaterial_Draw()
	{
		HLSLMaterialHandle hMaterial;
		loadShader(hMaterial, GetShaderID_Draw());
		return hMaterial;
	}

	Box3 GetObjectBox()
	{
		float QUAD_SIZE = 2.0f;
		Box3 bbox(
			Point3(-QUAD_SIZE/2.0f, -QUAD_SIZE/2.0f, -1.0f), 
			Point3(QUAD_SIZE/2.0f, QUAD_SIZE/2.0f, 1.0f));

		return bbox;
	}

	void ConstructRenderGeometry(SimpleRenderGeometry& simpleGeometry)
	{
		simpleGeometry.SetPrimitiveType(PrimitiveTriangleList);
		simpleGeometry.SetPrimitiveCount(2);

		MaterialRequiredStreams multiStreamRequirements;
		{
			MaterialRequiredStreamElement multiStreamElement;
			multiStreamElement.SetType(VertexFieldFloat3);
			multiStreamElement.SetChannelCategory(MeshChannelPosition);
			multiStreamElement.SetStreamIndex(0);
			multiStreamRequirements.AddStream(multiStreamElement);
		}
		{
			MaterialRequiredStreamElement multiStreamElement;
			multiStreamElement.SetType(VertexFieldFloat3);
			multiStreamElement.SetChannelCategory(MeshChannelVertexNormal);
			multiStreamElement.SetStreamIndex(1);
			multiStreamRequirements.AddStream(multiStreamElement);
		}
		{
			MaterialRequiredStreamElement multiStreamElement;
			multiStreamElement.SetType(VertexFieldFloat3);
			multiStreamElement.SetChannelCategory(MeshChannelTexcoord);
			multiStreamElement.SetStreamIndex(2);
			multiStreamRequirements.AddStream(multiStreamElement);
		}
		simpleGeometry.SetSteamRequirement(multiStreamRequirements);

		// Construct the buffer in left-hand coordinate system
		//  y
		//  ^  z
		//  | /
		//  |/
		//  +-----> x

		float QUAD_SIZE = 2.0f;

		VertexBufferHandle positionBuffer;
		positionBuffer.Initialize(sizeof(Point3),4);
		Point3* pPosition = (Point3*)(positionBuffer.Lock(0, 0, WriteAcess));
		pPosition[0] = Point3(-QUAD_SIZE/2.0f,  QUAD_SIZE/2.0f, 0.0f);
		pPosition[1] = Point3( QUAD_SIZE/2.0f,  QUAD_SIZE/2.0f, 0.0f);
		pPosition[2] = Point3(-QUAD_SIZE/2.0f, -QUAD_SIZE/2.0f, 0.0f);
		pPosition[3] = Point3( QUAD_SIZE/2.0f, -QUAD_SIZE/2.0f, 0.0f);
		positionBuffer.Unlock();
		simpleGeometry.AddVertexBuffer(positionBuffer);

		VertexBufferHandle normalBuffer;
		normalBuffer.Initialize(sizeof(Point3),4);
		Point3* pNormal = (Point3*)(normalBuffer.Lock(0, 0, WriteAcess));
		pNormal[0] = pNormal[1] = pNormal[2] = pNormal[3] = 
			Point3(0.0f,0.0f,1.0f);
		normalBuffer.Unlock();
		simpleGeometry.AddVertexBuffer(normalBuffer);

		VertexBufferHandle texcoordBuffer;
		texcoordBuffer.Initialize(sizeof(Point3),4);
		Point3* pTexcoord = (Point3*)(texcoordBuffer.Lock(0, 0, WriteAcess));
		pTexcoord[0] = Point3(0.0f, 0.0f, 0.0f);
		pTexcoord[1] = Point3(1.0f, 0.0f, 0.0f);
		pTexcoord[2] = Point3(0.0f, 1.0f, 0.0f);
		pTexcoord[3] = Point3(1.0f, 1.0f, 0.0f);
		texcoordBuffer.Unlock();
		simpleGeometry.AddVertexBuffer(texcoordBuffer);

		IndexBufferHandle indexBuffer;
		indexBuffer.Initialize(IndexTypeShort,6);
		unsigned short* pIndices = (unsigned short*)(indexBuffer.Lock(0, 0, WriteAcess));
		pIndices[0] = 0;
		pIndices[1] = 2;
		pIndices[2] = 1;
		pIndices[3] = 2;
		pIndices[4] = 3;
		pIndices[5] = 1;
		indexBuffer.Unlock();
		simpleGeometry.SetIndexBuffer(indexBuffer);
	}

	RenderNodeHandle CreateRenderNode()
	{
		GeometryRenderItemHandle hRenderItem;
		hRenderItem.Initialize();
		SimpleRenderGeometry* pRenderGeometry = new SimpleRenderGeometry;
		ConstructRenderGeometry(*pRenderGeometry);
		hRenderItem.SetRenderGeometry(pRenderGeometry);
		hRenderItem.SetVisibilityGroup(RenderItemVisible_Shaded);

		RenderNodeHandle hRenderNode;
		hRenderNode.Initialize();
		hRenderNode.AddRenderItem(hRenderItem);
		hRenderNode.SetObjectBox(GetObjectBox());

		return hRenderNode;
	}

	DrawQuadWorld CreateRenderWorld()
	{
		RenderWorldHandle renderWorld;
		renderWorld.Initialize();

		RenderNodeHandle hNode = CreateRenderNode();
		if (hNode.IsValid())
		{
			// Specify the node matrix
			Matrix44 matWorld;
			matWorld.MakeIdentity();
			hNode.SetWorldMatrix(matWorld);

			renderWorld.AddNode(hNode);
		}

		renderWorld.UpdateNodes();

		DrawQuadWorld drawQuadWorld;
		drawQuadWorld.hWorld = renderWorld;
		drawQuadWorld.hDraw = CreateMaterial_Draw();

		return drawQuadWorld;
	}

} } // namespaces

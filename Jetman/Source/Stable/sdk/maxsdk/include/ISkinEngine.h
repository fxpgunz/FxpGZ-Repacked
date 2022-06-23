/**********************************************************************
*<
FILE: SkinEngine.h

DESCRIPTION:  Declaration of the SkinEngine

CREATED BY: Nikolai Sander

HISTORY: created 7/10/99
12/21/2015 Moved into Geom

*>	Copyright (c) 1999, All Rights Reserved.
**********************************************************************/

#ifndef __SKINENGINE_H_
#define __SKINENGINE_H_

#include "GeomExport.h"
#include <math.h>
#include <geom.h>
#include <containers/Array.h>

class BoneModData;
class VertexListClass;
	
namespace MaxSDK
{
	namespace SkinEngine
	{

		template <class T> class StrideArray {

		protected:

			T *m_pT;
			DWORD m_stride;
		public:

			StrideArray(T *pT, DWORD stride) { m_pT = pT; m_stride = stride; }
			StrideArray() { m_pT = NULL; m_stride = 0; }

			StrideArray& operator=(const StrideArray& tb) {
				m_pT = tb.m_pT;
				m_stride = tb.m_stride;
				return *this;
			}
			T& operator[](const int i) const {       // access ith entry.
				assert(m_pT);
				return(*(T *)&((BYTE *)m_pT)[i*m_stride]);
			}
		};

		class CBoneDataClass
		{
		public:
			void SetInitTM(float *ptm);
			void SetInitTM(const Matrix3& ptm);
			void SetCurrentTM(const Matrix3& tm);
			Matrix3 const &GetCurrentTM() { return m_CurrentTM; }
			void SetFlags(DWORD flags);
			DWORD const GetFlags() { return m_flags; }
			Matrix3 &GetXFormTM(const Matrix3 &MeshTM);
		protected:
			Matrix3 m_XFormTM;
			Matrix3 m_InitTM;
			Matrix3 m_CurrentTM;
			DWORD m_flags;
			BOOL bCacheValid;
		};



		class PointDataClass
		{
		public:
			int m_numBones;
			Point3 m_InitialPos;
			StrideArray<int> m_BoneIndices;
			StrideArray<float> m_BoneWeights;

			// This stuff is only needed for Spline bones interpolation
			StrideArray<int> m_SubCurveIndices;
			StrideArray<int> m_SubSegIndices;
			StrideArray<float> m_SubSegDistance;
			StrideArray<Point3> m_Tangents;
			StrideArray<Point3> m_OPoints;
		};


		/////////////////////////////////////////////////////////////////////////////
		// CSkinEngine
		class CSkinEngine : public MaxHeapOperators
		{

			// ISkinEngine
		public:
			GEOMEXPORT CSkinEngine();
			GEOMEXPORT virtual ~CSkinEngine();

			
			GEOMEXPORT STDMETHODIMP SetNumPoints(int numPoints);
			GEOMEXPORT STDMETHODIMP SetNumBones(int numBones);

			GEOMEXPORT STDMETHODIMP SetBoneFlags(int boneIdx, DWORD flags);

			GEOMEXPORT bool UseSplineAnimation();
			GEOMEXPORT void SetUseSplineAnimation(bool splineAnim);


			GEOMEXPORT STDMETHODIMP SetInitTM( float *InitTM);
			GEOMEXPORT STDMETHODIMP SetInitTM(const Matrix3& InitTM);

			GEOMEXPORT STDMETHODIMP SetInitBoneTM(int boneIdx,  float *InitTM);
			GEOMEXPORT STDMETHODIMP SetInitBoneTM(int boneIdx, const Matrix3& InitTM);
			
			GEOMEXPORT STDMETHODIMP SetBoneTM(int boneIdx, float *currentTM);
			GEOMEXPORT STDMETHODIMP SetBoneTM(int boneIdx, const Matrix3& currentTM);

			GEOMEXPORT STDMETHODIMP SetPointData(int pointIdx, int numData,
										DWORD b_stride, int *BoneIndexArray,
										DWORD w_stride, float *WeightArray,
										DWORD sci_stride, int *SubCurveIdxArray,
										DWORD ssi_stride, int *SubSegIdxArray,
										DWORD ssd_stride, float *SubSegDistArray,
										DWORD t_stride, float *TangentsArray,
										DWORD op_stride, float *OPointsArray);

			
			GEOMEXPORT STDMETHODIMP CSkinEngine::MapPoint(int idx, float *pin, float *pout);

			GEOMEXPORT virtual Point3 GetInterpCurvePiece3D(int BoneId, int CurveId, int SegId, float distance);
			GEOMEXPORT virtual Point3 GetTangentPiece3D(int BoneId, int CurveId, int SegId, float distance);

		protected:
			Point3 SplineAnimation(int vertex, int bone, const Point3& p);


			MaxSDK::Array<CBoneDataClass> BoneData;
			MaxSDK::Array<PointDataClass> PointData;

			Matrix3 m_MeshTM;			
			bool mUseSplineAnimation;
		};

	}
}
#endif //__SKINENGINE_H_
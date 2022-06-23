
// Copyright (c) 2015 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.

/*! These classes let you define skin weighting using a voxel or height map based
	algorithm.  Samples of usage can be found in

		maxsdk/samples/modifiers/bonesdef/VoxelWeights/BMD_Command_VoxelWeight.cpp

*/

#ifndef __GEOMBIND_GEOMBIND_H__
#define __GEOMBIND_GEOMBIND_H__
#include "MaxHeap.h"
#include "GeomExport.h"

namespace MaxSDK
{
	namespace  GeomBind 
	{

		/*! Class ITransformHierarchy describes the skeleton to be used to compute the skin weights.   The skeletons is defined
		from joint to joint which is slightly different than Max's which uses bounding boxes.
		*/
		class ITransformHierarchy : public MaxHeapOperators
		{
		public:
			 /*!  This creates an instance of an ITransformHierarchy		   		*/
			GEOMEXPORT static ITransformHierarchy * create( );
			/*!  This disposes an instance of an ITransformHierarchy		   		*/
			GEOMEXPORT static void                  dispose( ITransformHierarchy * );
		   /*!  This adds a joint to the hierarchy NOTE joint names must be unique
		   \param name  name of the joint
		   \param worldTransform this is the transform of the joint NOTE only the transform component is used and the space is in openGL format
		   \param parent this is name of the parent of the joint you are adding
			*/			
			GEOMEXPORT virtual bool registerTransform(
				const char * name,
				double     * worldTransform,
				const char * parent = 0
				) = 0;
			/*! This clears out all the entries into the hierarchy */
			GEOMEXPORT virtual void clear() = 0;

		protected:
			inline ITransformHierarchy  ( ) { }
			virtual ~ITransformHierarchy( ) = 0;

		private:
			ITransformHierarchy              ( const ITransformHierarchy & );
			ITransformHierarchy & operator = ( const ITransformHierarchy & );
		};

		/*! Class IMesh describes the mesh to be used to compute the skin weights.   This is just triangle soup but the
		heatmap solver requires that this mesh be a manifold mesh and there are no checks here for that.  So that needs to be handled 
		before converting to an iMesh
		*/
		class IMesh : public MaxHeapOperators
		{
		public:
			/*!  This creates an instance of an IMesh		   		*/
			GEOMEXPORT static IMesh * create( );
			/*!  This disposes an instance of an IMesh		   		*/
			GEOMEXPORT static void    dispose( IMesh * );
			/*!  This adds a vertex to the mesh
			\param v0 x component
			\param v1 y component
			\param v2 z component
			*/	
			GEOMEXPORT virtual void appendVertex(
				float v0,
				float v1,
				float v2
				) = 0;
			/*!  This adds a face to the mesh
			\param i0 the index of the first vertex of the face
			\param i1 the index of the second vertex of the face
			\param i2 the index of the third vertex of the face
			*/	
			GEOMEXPORT virtual void appendTriangle(
				unsigned i0,
				unsigned i1,
				unsigned i2
				) = 0;

		protected:
			inline IMesh  ( ) { }
			virtual ~IMesh( ) = 0;

		private:
			IMesh              ( const IMesh & );
			IMesh & operator = ( const IMesh & );
		};


		/*! Class ISparseVertexWeights describes the weights that are assigned to a skin.  Each vertex has an array pair of
		weight and bone index to describe how skin to deform the vertices
		*/
		class ISparseVertexWeights : public MaxHeapOperators
		{
		public:
			struct Influence
			{
				inline Influence( unsigned i, double w )
					: index ( i )
					, weight( w )
				{ }

				unsigned index;
				double   weight;
			};

			/*!  This returns the total number of vertices
			*/
			GEOMEXPORT virtual unsigned count( ) const = 0;

			/*!  This returns the number of weights per vertex
			*/
			virtual unsigned influenceCount(
				unsigned vertex
				) const = 0;

			/*!  This the returns a a particular weight/bone index pair of vertex
			\param vertex the index of the vertex
			\param influence the index of the weight
			*/
			GEOMEXPORT virtual Influence influence(
				unsigned vertex,
				unsigned influence
				) const = 0;

		protected:
			inline ISparseVertexWeights  ( ) { }
			virtual ~ISparseVertexWeights( ) = 0;

		private:
			ISparseVertexWeights              ( const ISparseVertexWeights & );
			ISparseVertexWeights & operator = ( const ISparseVertexWeights & );
		};

 		/*! Class IProgress just a class that the solver uses to tell how far along the process is and allows the user
		to cancel out of the process.
		*/
		class IProgress : public MaxHeapOperators
		{
			public:
				inline virtual ~IProgress( ) { }

				/*!  Called at the start of a major operation
				\param description the text description of the operation
				\param max is the max value for the amount of time taken
				\param val is the current time
				*/
				GEOMEXPORT virtual IProgress & start(const char * description = 0, int max = 100, int val = 0 ) = 0;
				/*!  Called at the start of a major operation */
				GEOMEXPORT virtual IProgress & end() = 0;
				/*!  Sets the max and current time left
				\param max is the max value for the amount of time taken
				\param value is the current time
				*/
				GEOMEXPORT virtual IProgress & setMaxValue( int max, int value = 0 ) = 0;
				/*!  Sets the current time left
				\param value is the current time left
				*/
				GEOMEXPORT virtual bool        setValue( const int value ) = 0;
				/*!  returns if the operation should abort			*/
				GEOMEXPORT virtual bool        cancelOperation() = 0;
		};

		/*! Class IContext does all the actual skin weight computations.  NOTE this uses OGL so you have to set up an OGL
		context first and also all the data structures are in OGL format

		Examples of how to use this interface can be found in the bonesdef.vprog in maxsdk/samples/modifiers/bonesdef

		*/
		class IContext : public MaxHeapOperators
		{
		public:
			/*!  This creates an instance of a context		   		*/
			GEOMEXPORT static IContext * create ( );
			/*!  This disposes an instance of a context		   		*/
			GEOMEXPORT static void       dispose( IContext * );
			/*!  This makes the current OGL context active		   		*/
			GEOMEXPORT virtual void makeCurrent( ) = 0;

			/*!  This creates skin weights based on a voxel data data structure
				\param mesh this is the mesh that is being skinned
				\param hierarchy this the skeleton of the mesh being skinned
				\param progress the update bar to let the system know how far along the solve is and if it should cancel
				\param falloff how smooth the blend weights are 0 is the most smooth 1 is the least smooth
				\param maxInfluences the maximum number of weights per vertex that can be applied
				\param maxResolution the maximum density of the voxel data structure, the higher the more accurate but slower
				\param useWindingNumbers can produce better weight results when their are arbitrary voxel results
			*/
			GEOMEXPORT virtual const ISparseVertexWeights * bindSparseGV(
				IMesh                * mesh,
				ITransformHierarchy  * hierarchy,
				IProgress            * progress           = 0,
				float                  falloff           = 0.0f,
				int                    maxInfluences     = -1,
				int                    maxResolution     = 256,
				bool                   useWindingNumbers = true
				) = 0;

			/*!  This creates skin weights based on a heat map algorithm, restrictions are that the mesh be manifold, only the first mesh
				element will have its weight computed.  Usually the voxel algorithm produces better results.
				\param mesh this is the mesh that is being skinned
				\param hierarchy this the skeleton of the mesh being skinned
				\param progress the update bar to let the system know how far along the solve is and if it should cancel
				\param falloff how smooth the blend weights are 0 is the most smooth 1 is the least smooth
				\param maxInfluences the maximum number of weights per vertex that can be applied
			*/
			GEOMEXPORT virtual const ISparseVertexWeights * bindHeatmap(
				IMesh               * mesh,
				ITransformHierarchy * hierarchy,
				IProgress           * progress,
				float                 falloff       = 0.0f,
				int                   maxInfluences = -1
				) = 0;

		protected:
			inline IContext  ( ) { }
			virtual ~IContext( ) = 0;

		private:
			IContext              ( const IContext & );
			IContext & operator = ( const IContext & );
		};

	}
}

#endif /// __GEOMBIND_GEOMBIND_H__



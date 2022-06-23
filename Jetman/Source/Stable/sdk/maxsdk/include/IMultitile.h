//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//

#ifndef __IMULTITILE_H__
#define __IMULTITILE_H__

#include "stdmat.h"

#define MULTITILEMAP_INTERFACE Interface_ID(0x1e1d1825, 0x356d0c68)

#define GetMultiTileInterface( obj ) \
	((MultiTileMap::Interface*)obj->GetInterface(MULTITILEMAP_INTERFACE))


enum class TilePatternFormat
{
	Invalid = -1,
	First = 0,
	ZBrush = First,	// 0-based, file format starts from xxxxU0_V0
	Mudbox,			// 1-based, file format starts from xxxxU1_V1
	UDIM,			// Mari style, format: 1000 + U * 10 + V, V starts from 1, file format starts from xxxx1001
	Custom,			// No pattern, set image file and UV offset at will
	Num,
	Last = Num
};

enum class ViewportQuality
{
	Invalid = -1,
	First = 0,
	Low = First,
	Middle,
	High,
	Num,
	Last = Num
};

/*! \sa  Class IMultiTile.\n\n
\par Description:
This interface defines methods that manipulate the MultiTile map.
It is derived by MultiTileMap
*/
class IMultiTile : public MultiTex
{
public:
	/*! 
	\remarks Set TilePatternFormat enum to the MultiTile map.
	If the format is different from existing format, the MultiTile map clears existing
	tile list, then set the new format.
	Otherwise, nothing changes.
	\param[in] format The format to be set.
	*/
	virtual void SetPatternFormat( TilePatternFormat format ) = 0;

	/*! 
	\remarks Get TilePatternFormat enum from the MultiTile map.
	\return  Current TilePatternFormat value.
	*/
	virtual TilePatternFormat GetPatternFormat() const = 0;

	/*! 
	\remarks Get pattern format through specified file path.
	\param[in] filePath The patterned file path to check.
	\return Return one of the pattern format if the file path match its regulation,
	return <b>Custom</b> if match none of them.
	return <b>Invalid</b> if file path is null string.
	*/
	virtual TilePatternFormat GetFilePatternFormat( const MCHAR *filePath ) = 0;

	/*! 
	\remarks Set ViewportQuality enum to the MultiTile map.
	Set the new quality only if it is different from existing quality.
	Otherwise, nothing changes.
	\param[in] quality The viewport quality to be set.
	*/
	virtual void SetViewportQuality( ViewportQuality quality ) = 0;

	/*! 
	\remarks Get ViewportQuality enum from the MultiTile map.
	\return  Current ViewportQuality value.
	*/
	virtual ViewportQuality GetViewportQuality() const = 0;

	/*! 
	\remarks Set a patterned image file to the MultiTile map.
	It first verifies file name pattern with current pattern format, 
	return false if the file name pattern is invalid.
	If the pattern is ok, it searches the directory where file resides
	to find other image files that match the pattern, 
	and load all of them and return true.
	The MultiTile map creates a tile list with these patterned image files.
	\param[in] filePath The patterned file path to be set.
	\return Return true if current pattern format is one of the 3 valid formats of TilePatternFormat,
	and file name is matched with current pattern format. Otherwise, return false.
	*/
	virtual bool SetPatternedImageFile( const MCHAR *filePath ) = 0;
	
	/*! 
	\remarks Set an image file to the specified tile of a MultiTile map.
	If current tile pattern format is one of the 3 valid foramts of TilePatternFormat,
	the tile index <b>must</b> be zero. Otherwise return false and do nothing.
	It then follows same work flow as <b>SetPatternedImageFile</b>.
	If current tile pattern format is <b>Custom</b> format, the tile index could be zero, 
	or value other than zero. It sets image file path as it is without verification and matching.
	\param[in] tileIndex The tile index to be set with new image file.
	\param[in] filePath The image file path to be set.
	\return Return true if current pattern format is one of the 3 valid formats of TilePatternFormat,
	and file name is matched with current pattern format.
	Otherwise, return false.
	*/
	virtual bool SetImageFile( int tileIndex, const MCHAR *filePath ) = 0;

	/*! 
	\remarks Return file prefix of current pattern format.
	\return Return file prefix if current pattern format is one of the 3 valid formats of TilePatternFormat,
	return nullptr if current pattern format is <b>Custom</b>, or patterned image file has not been set.
	*/
	virtual const MCHAR* GetPatternedFilePrefix() const = 0;

	/*! 
	\remarks Set UV offset to specified tile.
	It only works in <b>Custom</b> format.
	\param[in] tileIndex The tile to be set UV offset.
	\param[in] uOffset The U offset to be set.
	\param[in] vOffset The V offset to be set.
	\return If current format is <b>Custom</b> and tile index is valid, return true.
	Otherwise, return false.
	*/
	virtual bool SetUVOffset( int tileIndex, int uOffset, int vOffset ) = 0;

	/*! 
	\remarks Get UV offset with specified tile.
	The returned UV offset are truncated to integer.
	If the texmap is 3D, return X, Y axis offset instead.
	\param[in] tileIndex The tile to be set UV offset.
	\param[out] uOffset The returned U offset, truncated to integer.
	\param[out] vOffset The returned V offset, truncated to integer.
	\return If the tile index is valid, return true.
	Otherwise, return false.
	*/
	virtual bool GetUVOffset( int tileIndex, int *uOffset, int *vOffset ) = 0;

	/*! 
	\remarks Append a new tile to existing list.
	*/
	virtual void AddTile() = 0;

	/*! 
	\remarks Delete specified tile.
	If index is invalid, or this is the last layer, no action occurred
	\param[in] Index The tile index to be deleted.
	*/
	virtual void DeleteTile( size_t Index = 0 ) = 0;
};

#endif // __IMULTITILE_H__

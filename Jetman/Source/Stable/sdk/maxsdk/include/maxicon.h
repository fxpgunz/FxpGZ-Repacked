 /**********************************************************************
 *<
   FILE: MaxIcon.h

   DESCRIPTION: Max Icon and Icon Table definitions

   CREATED BY: Scott Morrison

   HISTORY: Created 15 March, 2000,

 *>   Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#pragma once

#include "object.h"
#include "iColorMan.h"
#include "strbasic.h"

class ICustButton;

/*! \sa  Class InterfaceServer, Class ICustButton, Class MaxBmpFileIcon.\n\n
\par Description:
This abstract class represents an icon image for toolbar buttons, icons in list
boxes, etc. The class is based on Win32 ImageLists. MaxIcons must provide an
image list and index into the list for both large (24x24) and small (16x15 or 16x16) icons.
*/
class MaxIcon : public InterfaceServer {
public:
	//! Destructor
	CoreExport virtual ~MaxIcon();
    // Get the image list for the size of icons that the user has chose
    /*! \remarks Returns the handle to the image list for the size of icons
    that the user has chosen. */
    virtual HIMAGELIST GetDefaultImageList()  = 0;
    // Get the image list for small icons
    /*! \remarks Returns the image list for small icons. */
    virtual HIMAGELIST GetSmallImageList()  = 0;
    // Get the image list for large icons
    /*! \remarks Returns the image list for large icons. */
    virtual HIMAGELIST GetLargeImageList()  = 0;

    // Get the index into the image list for the small version of this
    // particular icon.
	 /*! \remarks Returns the zero based index into the image list for the small
	 version of this particular icon.
	 \par Parameters:
	 <b>bool enabledVersion = true</b>\n\n
	 Pass true to get the index of the enabled version of the icon; false to get the
	 disabled version.\n\n
	 <b>COLORREF backgroundColor = GetCustSysColor(COLOR_BTNFACE)</b>\n\n
	 The background color to use for the icon.\n\n
	 Specifies the windows color definition. See \ref standardColorIDs.
	 For a full list of windows color definitions, please refer to the Win32 API, in
	 particular the methods <b>GetSysColor()</b> and <b>SetSysColor()</b>. */
    virtual int GetSmallImageIndex(bool enabledVersion = true,
                                   COLORREF backgroundColor =
                                   GetCustSysColor( COLOR_BTNFACE) ) = 0;
    // Get the index into the image list for the large version of this
    // particular icon.
	 /*! \remarks Returns the zero based index into the image list for the large
	 version of this particular icon.
	 \par Parameters:
	 <b>bool enabledVersion = true</b>\n\n
	 Pass true to get the enabled version of the icon or false to get the disabled
	 version.\n\n
	 <b>COLORREF backgroundColor = GetCustSysColor(COLOR_BTNFACE)</b>\n\n
	 The background color to use for the icon.\n\n
	 Specifies the windows color definition. See \ref standardColorIDs.
	 For a full list of windows color definitions, please refer to the Win32 API, in
	 particular the methods <b>GetSysColor()</b> and <b>SetSysColor()</b>. */
    virtual int GetLargeImageIndex(bool enabledVersion = true,
                                   COLORREF backgroundColor =
                                   GetCustSysColor( COLOR_BTNFACE) ) = 0;
    // Get the index into the image list for the default version of this
    // particular icon.
	 /*! \remarks Returns the zero based index into the image list for the default
	 version of this particular icon.
	 \par Parameters:
	 <b>bool enabledVersion = true</b>\n\n
	 Pass true to get the enabled version of the icon or false to get the disabled
	 version.\n\n
	 <b>COLORREF backgroundColor = GetCustSysColor(COLOR_BTNFACE)</b>\n\n
	 The background color to use for the icon.\n\n
	 Specifies the windows color definition. See \ref standardColorIDs.
	 For a full list of windows color definitions, please refer to the Win32 API, in
	 particular the methods <b>GetSysColor()</b> and <b>SetSysColor()</b>. */
            int GetDefaultImageIndex(bool enabledVersion = true,
                                     COLORREF backgroundColor =
                                     GetCustSysColor( COLOR_BTNFACE) );

    // returns true if the icons has an alpha mask that needs to be blended
    // with the background color.
    /*! \remarks This method returns true if the icons has an alpha mask that
    needs to be blended with the background color and false if it doesn't use
    an alpha mask. */
    virtual bool UsesAlphaMask() = 0;
};

// This implementation of MaxIcon is for the icon images that are stored
// as ".bmp" files in MAX's UI directory.  This is used by the macroScript
// facility in MAXSrcipt to specify icons.  See the documentation for
// "macroScript" for the exact meaning of the filename and index.

/*! 
\brief Implements icon support for icon images stored as image files on disk,
or compiled resources in .rcc files.

\details This class supports two types of icons: legacy icons that are stored as image list ".bmp"
files in 3ds Max's UI directory, and multi-resoution icons that support 4K / high DPI
displays. 

This class is used by the macroScript facility in
MAXScript to specify icons. See the MAXScript documentation for "macroScript"
for the exact meaning of the filename and index and details. 

\sa  Class MaxIcon, Class ICustButton.
 */
class MaxBmpFileIcon: public MaxIcon {
public:
	//! Default Destructor
	CoreExport ~MaxBmpFileIcon();
    /*! \remarks Constructor.    
    \param pFilePrefix The file prefix to initialize with.
    \param index The index of the icon.
	\note This constructor is for legacy icons stored as image list .bmp files in the UI_ln directory.
	*/
    CoreExport MaxBmpFileIcon(const MCHAR* pFilePrefix, int index);

	/*! \remarks Constructor.
	Creates a MaxBmpFileIcon which refers to a multi-resolution icon.
	\param iconName The name of the icon. 
	\see QIcon MaxSDK::LoadMaxMultiResIcon( const QString& iconName ) for more details
	about how to specify the icon path and how to register resources to load icons from.  
	The **howto/ui/menudemo** sample contains an example of how to use this API.
	\snippet howto/ui/menudemo/maxMenuDemo.cpp MaxBmpFileIcon Example
	*/
	CoreExport MaxBmpFileIcon( const MCHAR* iconName );
	/*! \remarks Constructor.
	\param sid 	The superclass ID\n\n
	\param cid The class ID/
	*/
	CoreExport MaxBmpFileIcon(SClass_ID sid, Class_ID cid);

    /*! \remarks Returns the handle to the image list for the size of icons
    that the user has chosen. */
    CoreExport HIMAGELIST GetDefaultImageList();
    /*! \remarks Returns the image list for small icons. */
    CoreExport HIMAGELIST GetSmallImageList();
    /*! \remarks Returns the image list for large icons. */
    CoreExport HIMAGELIST GetLargeImageList();

	 /*! \remarks Returns the zero based index into the image list for the small
	 version of this particular icon.
	 
	 \param enabledVersion Pass true for the enabled version of the icon; false for the disabled
	 version.
	 \param backgroundColor The background color for use in alpha blending. The files that define these
	 icons always have an alpha mask, and so a background color is needed to blend
	 it with. */
    CoreExport int GetSmallImageIndex(bool enabledVersion = true,
                                      COLORREF backgroundColor =
                                      GetCustSysColor( COLOR_BTNFACE) );
	 /*! \remarks Returns the zero based index into the image list for the large
	 version of this particular icon.
	 
	 \param enabledVersion Pass true for the enabled version of the icon; false for the disabled
	 version.
	 \param backgroundColor The background color for use in alpha blending. The files that define these
	 icons always have an alpha mask, and so a background color is needed to blend
	 it with. */
    CoreExport int GetLargeImageIndex(bool enabledVersion = true,
                                      COLORREF backgroundColor =
                                      GetCustSysColor( COLOR_BTNFACE) );

    /*! \remarks Returns true if the icon uses an alpha mask; otherwise false.
    */
    CoreExport bool UsesAlphaMask();
    /*! \remarks Returns the directory of the icon. */
    CoreExport MSTR& GetFilePrefix() { return mFilePrefix; }
    /*! \remarks Returns the index of the icon in the image file list. */
    CoreExport int GetIndex() { return mIndex; }

	/*! \remarks Returns the name of the multi-resolution icon.
	\see QIcon MaxSDK::LoadMaxMultiResIcon( const QString& iconName )
	*/
	CoreExport const MSTR& GetIconName() const;

    /*! \remarks Tells the icon to limit color correction. */
    CoreExport void SetKeepColor() { mKeepColor = true; }

    /*! \return Tells the icon to limit color correction. */
    CoreExport bool GetKeepColor() { return mKeepColor; }

	/*! /return Returns the full path of a Bmp icon file. */
	CoreExport MSTR GetIconPath();

private:
    int     mIndex;
    MSTR    mFilePrefix;
    bool    mKeepColor;
};

CoreExport HIMAGELIST GetIconManDefaultImageList();
CoreExport HIMAGELIST GetIconManSmallImageList();
CoreExport HIMAGELIST GetIconManLargeImageList();

//-------------------------------------------------------------------------
/** \brief Loads a bitmap file into an HIMAGELIST.
* \param[in] pFile	The name of the bitmap file to load.
* \param[in,out] hImageList	The HIMAGELIST to load the bitmap file into.
* \param[in] color	The background color
* \param[in] disabled	Specifies whether icons show enabled or disabled state. 
* \param[in] applyDPIScaling If true, the bitmap loaded from the file is rescaled using the UI scaling factor. 
* \returns TRUE if the specified file was loaded

The icon width/height used when creating the HIMAGELIST must reflect whether DPI scaling is
performed. For example:

\code
HIMAGELIST hImageList = ImageList_Create(MaxSDK::GetUIScaleFactor(16), MaxSDK::GetUIScaleFactor(16), ILC_COLOR24 | ILC_MASK, 14, 0);
LoadMAXFileIcon(images_filename, hImageList, kBackground, FALSE, TRUE);
\endcode
*/
CoreExport BOOL LoadMAXFileIcon(const MCHAR* pFile, HIMAGELIST hImageList, ColorId color, BOOL disabled, BOOL applyDPIScaling = FALSE);


class QIcon;
class QString; 
namespace MaxSDK
{
	//-------------------------------------------------------------------------
	/** \brief Loads a set of Images into a multi-resolution QIcon.
	 * This function looks into the UI_ln folder of the 3dsmax installation (or 
	 * the internal Qt resource system) to find a set of matching image files 
	 * to mark for usage. The actual images get loaded on demand.
	 * 
	 * The naming of those files has to match one of the following schemes:
	  
	  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	  [:/]Folder/BaseName[_on|_off][_size].ext 
	  [:/]Folder/BaseName[_on|_off][_(width)x(height)].ext 
	  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 
	 * So for example: 
	 * 
	 * | iconName | files on disk     | QIcon entry size  | QIcon state |
	 * |----------|-------------------|-------------------|-------------|
	 * | Undo     | Undo_16.png       | 16 x 16px         | Off         |
	 * | Undo     | Undo_off_24.png   | 24 x 24px         | Off         |
	 * | Undo     | Undo_on_32.png    | 32 x 32px         | On          |
	 * | Undo     | Undo_on_100.png   | 100 x 100px       | On          |
	 * | Undo     | Undo_16x32.png    | 16 x 32px         | Off         |
	 * | Undo     | Undo_on_16x32.png | 16 x 32px         | On          |
	 * 
	 * In that case a single call on LoadMaxMultiResIcon( "Undo" ) would 
	 * produce an QIcon containing all the different sizes and states. 
	 *
	 * | iconName  | files on disk     | QIcon entry size  | QIcon state |
	 * |-----------|-------------------|-------------------|-------------|
	 * | Redo.png  | Redo.png          | based on file[^1] | Off         |
	 * | Redo.png  | Redo_on.png       | based on file[^1] | On          |
	 *
	 * [^1]: This forces the QIcon to load the image file to determine its 
	 *		 actual size.
	 *
	 * | iconName  | files on disk     | QIcon entry size  | QIcon state |
	 * |-----------|-------------------|-------------------|-------------|
	 * | Fancy.jpg | Fancy_300x100.jpg | 300 x 100px       | Off         |
	 * | Fancy.jpg | Fancy_150x50.jpg  | 150 x 50px        | Off         |
	 *
	 * The iconName may also contain folders or sub-folders, so these fileNames 
	 * are fine, too (slashes or backslashes can be used): 
	 *
	 *    "SME/Editor.png"
	 *    "SME/Nodes/Collapse.png"
	 *    "SME\\Nodes\\RemoveItem.png" 
	 * 
	 * Based on the current 3dsmax theme (Dark/Light) the icons are retrieved 
	 * from different locations on the file-system, so depending on the setting
	 * "SME/Editor.png" may get: 
	 * 
	 *    UI_ln/Icons/Dark/SME/Editor*.png
	 *    UI_ln/Icons/Light/SME/Editor*.png
	 *
	 * If the iconName starts with ":/", it points to a Qt Resource like 
	 * ":/Icons/Undo", the theming is not automatically applied, so the caller 
	 * is responsible to actually provide the correct resource-bundle. 
	 * Nevertheless the QIcons internal pixmap cache will be wiped on a theme 
	 * change, so the next paint call on the QIcon will reload the acquired 
	 * version.
	 *
	 * If a size of an image inside an QIcon is not available but requested for 
	 * painting, Qt will downscale the next bigger version automatically - but 
	 * will never scale images up. 
	 *
	 * \param[in] iconName	The name of the icon to load. The path, basename 
	 *						and the extension of the file is used to find 
	 *						matching icons. 
	 *						The Extension may be omitted and .png is assumed to 
	 *						be the default extension.
	 *
	 * \returns A QIcon that contains all the images that matches the given 
	 *			iconName. Might be an empty QIcon if no match could be found.
	 * \see QIcon class in the Qt documentation, Qt Resource System
	 */
	CoreExport QIcon LoadMaxMultiResIcon( const QString& iconName );

	//-------------------------------------------------------------------------
	/** \brief Create an HICON from a MaxMultiResIcon.
	 * Loads a QIcon using LoadMaxMultiResIcon, and tries to extract a HICON
	 * from it using the given size, mode and state settings. For a detailed 
	 * description on the iconName syntax, please have a closer look at the 
	 * LoadMaxMultiResIcon function. 
	 * The HICON returned might be smaller than requested, but never larger.
	 *
	 * Convenience method for legacy usage, for example when using one size of 
	 * a multi-resolution-icon in a win32 HMENU. Note that the resulting HICON 
	 * wont refresh itself on a theme change, as the QIcon does. 
	 *
	 * \param[in] iconName  The name of the icon to load. This may be the 
	 *						actual filename, but also can be some folder/basename
	 *						combination, as described in detail in LoadMaxMultiResIcon.
	 * \param[in] width		The (maximum) width of the image to be loaded.
	 * \param[in] height	The (maximum) height of the image to be loaded.
	 * \param[in] enabled	Sets, if an enabled or disabled version of the icon 
	 *						should be retrieved (QIcon::Normal vs QIcon::Disabled)
	 * \param[in] on		Sets, if the On or Off state of the icon should be 
	 *						retrieved (QIcon::On vs QIcon::Off).
	 *
	 * \returns A new HICON for the given parameters, if available, a nullptr
	 *			otherwise. The HICON might be smaller than requested, but never
	 *			larger.
	 *
	 * \see	LoadMaxMultiResIcon, QIcon::Mode, QIcon::State
	 */
	CoreExport HICON CreateHICONFromMaxMultiResIcon( const MSTR& iconName, int width, int height, bool enabled = true, bool on = false ); 

	inline HICON CreateHICONFromMaxMultiResIcon( const MSTR& iconName, int size, bool enabled = true, bool on = false )
	{
		return CreateHICONFromMaxMultiResIcon( iconName, size, size, enabled, on );
	}

	//-------------------------------------------------------------------------
	/** \brief Create an HIMAGELIST from a MaxMultiResIcon.
	* Loads a QIcon using LoadMaxMultiResIcon, and tries to convert it to a 
	* HIMAGELIST using the given size, mode and state settings. For a detailed
	* description on the iconName syntax, please have a closer look at the
	* LoadMaxMultiResIcon function.
	*
	* \param[in] imageIconName  The name of the icon to load. This may be the
	*						actual filename, but also can be some folder/basename
	*						combination, as described in detail in LoadMaxMultiResIcon.
	* \param[in] size		The width and height of the component icons in the image to be loaded.
	* \param[in] count	The number of component icons in the image to be loaded.
	* \param[in] alpha	Sets, if alpha channel in icon should be used when creating the 
	*						bitmap from the QIcon
	* \param[in] enabled	Sets, if an enabled or disabled version of the icon
	*						should be retrieved (QIcon::Normal vs QIcon::Disabled)
	* \param[in] on		Sets, if the On or Off state of the icon should be
	*						retrieved (QIcon::On vs QIcon::Off).
	*
	* \returns A new HIMAGELIST for the given parameters, if available, a nullptr
	*			otherwise. 
	*
	* \see	LoadMaxMultiResIcon, QIcon::Mode, QIcon::State
	*/
	CoreExport HIMAGELIST CreateHIMAGELISTFromMaxMultiResIcon(const MSTR& imageIconName, int size, int count, bool alpha = true, bool enabled = true, bool on = false);
};


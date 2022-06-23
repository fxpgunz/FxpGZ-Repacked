//-----------------------------------------------------------------------------
// Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//-----------------------------------------------------------------------------


#pragma once

#include "CoreExport.h"
#include "BuildWarnings.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800)
#include <QtWidgets/QMainWindow>
#pragma warning(pop)

// forward declarations
class ICustToolbar;

namespace MaxSDK {;

// forward declarations
class QmaxDockingWinHost;
class QmaxMainWindowPrivate;

//-----------------------------------------------------------------------------
// class QmaxMainWindow
//-----------------------------------------------------------------------------
/** \brief This class is used by 3ds Max for its docking UI and should be used
* as replacement for the standard QMainWindow.
*/
class CoreExport QmaxMainWindow : public QMainWindow
{
	Q_OBJECT

	/** \brief Determines whether the docked toolbars and dockwidgets can be 
	* moved around and/or made floating.
	* \see dockingLocked(), setDockingLocked(), dockingLockedChanged() */
	Q_PROPERTY( bool dockingLocked READ dockingLocked WRITE setDockingLocked )

public:

	// common docking flags
	enum QmaxDockFlag
	{
		DontSaveCUIContent = 0x1,
		SystemWindow = 0x2,
		HideInVisiblityMenu = 0x4, // hides toggle visibility menu entry in main window popup
		ShowInExpertMode = 0x8,
		DestroyOnCUIChange = 0x10, // indicates if a dockwidget should be destroyed when a new CUI scheme is loaded, only makes sense for content that can change dynamically in different workspaces
		NoDockFlags = 0
	};

	Q_DECLARE_FLAGS( QmaxDockFlags, QmaxDockFlag )

	enum QmaxDockType
	{
		Window,
		Toolbar,
		Menu
	};

	explicit QmaxMainWindow( QWidget* parent = nullptr, Qt::WindowFlags flags = 0 ); 
	virtual ~QmaxMainWindow() override;

	/** \brief Loads a docking layout from a specified file.
	* \param fileName The full path to a docking layout file that should be loaded.
	* \see saveLayout() */
	void loadLayout( const QString& fileName );

	/** \brief Saves the current docking layout to a specified file.
	* \param fileName The full path to the docking layout file that should be saved.
	* \see loadLayout() */
	void saveLayout( const QString& fileName ) const;
	
	/** \brief Returns the file extension for a 3ds Max docking layout file.
	* \see loadLayout(), saveLayout() */
	static QString layoutFileExtension() { return ".layout"; }

	/** \brief Returns a list with the toolbars attached to this main window.
	* \see getDockWidgets() */
	QList< QToolBar* > getToolBars() const;

	/** \brief Returns a list with the dock widgets attached to this main window.
	* \see getToolBars() */
	QList< QDockWidget* > getDockWidgets() const;

	/** \brief Determines whether the docked toolbars and dockwidgets can be 
	* moved around.
	* 
	* The basic idea of having the docking locked mode is to prevent all docked 
	* toolbars and dockwidgets to be moved or made floating. Already floating 
	* toolbars and dockwidgets can be moved around but not be docked back into 
	* the main window. 
	* 
	* Note that docking locked does not prevent the user from resize the UI. */
	bool dockingLocked() const;

	// ---------------------------------------------------------------------------------
	// max docking helpers 
		
	/** \brief Sets the 3ds Max dock type on a QToolBar or QDockWidget.
	* \param[in] widget	The QToolBar or QDockWidget on which the dock type should be set.
	* \param[in] dockType The dock type of the actual docking content.
	* \see maxDockType() */
	static void setMaxDockType( QWidget* widget, QmaxDockType dockType );

	/** \brief Returns the 3ds Max dock type for a QToolBar or QDockWidget.
	* \param[in] widget	The QToolBar or QDockWidget for which the dock type should be returned.
	* \see setMaxDockType() */
	static QmaxDockType maxDockType( const QWidget* widget );

	/** \brief Sets the 3ds Max specific dock flags on a QToolBar or QDockWidget.
	* \param[in] widget	The QToolBar or QDockWidget on which the dock flags should be set.
	* \param[in] dockFlags The 3ds Max specific dock flags.
	* \see maxDockFlags(), addMaxDockFlags() */
	static void setMaxDockFlags( QWidget* widget, QmaxDockFlags dockFlags );

	/** \brief Returns the 3ds Max specific dock flags for a QToolBar or QDockWidget.
	* \param[in] widget	The QToolBar or QDockWidget for which the dock flags should be returned.
	* \see setMaxDockFlags(), addMaxDockFlags() */
	static QmaxDockFlags maxDockFlags( const QWidget* widget );

	/** \brief Adds the 3ds Max specific dock flags on a QToolBar or QDockWidget.
	* \param[in] widget	The QToolBar or QDockWidget on which the dock flags should be added.
	* \param[in] dockFlags The 3ds Max specific dock flags.
	* \see maxDockFlags(), setMaxDockFlags() */
	static void addMaxDockFlags( QWidget* widget, QmaxDockFlags dockFlags ) { setMaxDockFlags( widget, maxDockFlags( widget ) | dockFlags ); }

	/** \brief Makes a docked toolbar floating at the specified screen position.
	* \param[in] toolBar The QToolBar that should float.
	* \param[in] pos The screen position at which the toolbar should float.
	* \note The toolbar needs to be added once to the main window toolbar layout, 
	* before you can't float it. */
	static void makeToolBarFloating( QToolBar* toolBar, const QPoint& pos = QPoint() );

	/** \brief Returns the toolbar area for a QToolBar.
	* \param[in] toolbar The QToolBar for which the toolbar area should be returned.
	* \see getDockWidgetArea() */
	static Qt::ToolBarArea getToolBarArea( QToolBar* toolbar );

	/** \brief Returns the docking area for a QDockWidget.
	* \param[in] dockwidget The QDockWidget for which the docking area should be returned.
	* \see getToolBarArea() */
	static Qt::DockWidgetArea getDockWidgetArea( QDockWidget* dockwidget );

	/** \brief Retrieves the QmaxDockingWinHost if the given toolbar hosts a legacy win32 control.
	* \param[in] toolbar The QToolBar for which the QmaxDockingWinHost should be retrieved.
	* \see getICustToolbar() */
	static QmaxDockingWinHost* getDockingWinHost( QToolBar* toolbar );

	/** \brief Retrieves the legacy 3ds Max toolbar if the given QToolBar hosts one.
	* \note The function calls internally GetICustToolbar( HWND ), so you need
	* to call ReleaseICustToolbar after usage.
	* \see GetICustToolbar(), ReleaseICustToolbar(), getDockingWinHost() */
	static ICustToolbar* getICustToolbar( QToolBar* toolbar );

	// ---------------------------------------------------------------------------------


	/** \brief This method is for internal use only. Calling this method
	* alone will not put 3ds Max in Expert mode. */
	void expertMode( bool on );

	// QMainWindow overrides
	virtual QMenu* createPopupMenu() override;

public Q_SLOTS:

	/** \brief Changes the dockingLocked state.
	 * \param[in] state The new dockingLocked state.
	 * \see dockingLocked() */
	void setDockingLocked( bool state );

protected:
	virtual bool eventFilter( QObject* obj, QEvent* evt ) override;
	virtual void childEvent( QChildEvent* evt ) override;

	/** \brief Adds the default docking options for a given toolbar or dock widget 
	* to the specified menu. */
	bool addDefaultDockOptions( QWidget* dockWidget, QMenu* menu ) const;

	/** \brief Returns the child toolbar or dock widget which is currently under the mouse. */
	QWidget* dockWidgetUnderMouse() const;

private:
	Q_DECLARE_PRIVATE( QmaxMainWindow )
	Q_DISABLE_COPY( QmaxMainWindow )
	QmaxMainWindowPrivate* d_ptr;
};

};
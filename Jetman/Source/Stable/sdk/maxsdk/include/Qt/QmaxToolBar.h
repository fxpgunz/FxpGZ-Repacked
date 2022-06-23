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
#include "strbasic.h"
#include "QmaxMainWindow.h"

#pragma warning(push)
#pragma warning(disable: 4127 4251 4275 4512 4800)
#include <QtWidgets/QToolBar>
#pragma warning(pop)


namespace MaxSDK {;

// forward declarations
class QmaxToolBarPrivate;

//-----------------------------------------------------------------------------
// class QmaxToolBar
//-----------------------------------------------------------------------------
/** \brief This class is used by 3ds Max for the docking toolbars and should be 
* used as replacement for the standard QToolBar.
*/
class CoreExport QmaxToolBar: public QToolBar
{
	Q_OBJECT

public:

	explicit QmaxToolBar( const QString& objectName, const QString& title, QWidget* parent = nullptr );
	virtual ~QmaxToolBar() override;

	// CUI content load / save
	/** \brief Loads CUI content from a file into the toolbar.
	* \see saveCUIContent() */
	bool loadCUIContent( const QString& fileName, int startup );

	/** \brief Save CUI content from the toolbar into a file.
	* \see loadCUIContent() */
	void saveCUIContent( const QString& fileName );

	/** \brief Sets the 3ds Max dock type on the toolbar.
	* \param[in] dockType The dock type of the actual docking content.
	* \see maxDockType() */
	void setMaxDockType( QmaxMainWindow::QmaxDockType dockType ) { QmaxMainWindow::setMaxDockType( this, dockType ); }

	/** \brief Returns the 3ds Max dock type for the toolbar.
	* \see setMaxDockType() */
	QmaxMainWindow::QmaxDockType maxDockType() const { return QmaxMainWindow::maxDockType( this ); }

	/** \brief Sets the 3ds Max specific dock flags on the toolbar.
	* \param[in] dockFlags The 3ds Max specific dock flags.
	* \see maxDockFlags(), addMaxDockFlags() */
	void setMaxDockFlags( QmaxMainWindow::QmaxDockFlags dockFlags ) { QmaxMainWindow::setMaxDockFlags( this, dockFlags ); }

	/** \brief Adds the 3ds Max specific dock flags on the toolbar.
	* \param[in] dockFlags The 3ds Max specific dock flags.
	* \see maxDockFlags(), setMaxDockFlags()
	*/
	void addMaxDockFlags( QmaxMainWindow::QmaxDockFlags dockFlags ) { QmaxMainWindow::addMaxDockFlags( this, dockFlags ); }

	/** \brief Returns the 3ds Max specific dock flags for the toolbar.
	* \see setMaxDockFlags(), addMaxDockFlags() */
	QmaxMainWindow::QmaxDockFlags maxDockFlags() const { return QmaxMainWindow::maxDockFlags( this ); }

	

	/** \brief Sets the widget for the toolbar.
	* This method is a convenience function for toolbars that host only one 
	* widget like a QWinHost.
	* 3ds Max uses it for putting a QmaxDockingWinHost which hosts a legacy 
	* win32 toolbar into the QToolBar.
	* \param[in] widget	The widget that should be set to the toolbar.
	* \see widget() */
	void setWidget( QWidget* widget );

	/** \brief Returns the widget for the toolbar.
	* The function returns a nullptr no widget has been set.
	* \see setWidget() */
	QWidget* widget() const;

private:
	Q_DECLARE_PRIVATE( QmaxToolBar )
	Q_DISABLE_COPY( QmaxToolBar )
	QmaxToolBarPrivate* d_ptr;
};

};
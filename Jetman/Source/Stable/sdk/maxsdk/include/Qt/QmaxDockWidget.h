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
#include <QtWidgets/QDockWidget>
#pragma warning(pop)


namespace MaxSDK {;

//-----------------------------------------------------------------------------
// class QmaxDockWidget
//-----------------------------------------------------------------------------
/** \brief This class is used by 3ds Max for the docking dock widgets and should 
* be used as replacement for the standard QDockWidget.
*/
class QmaxDockWidgetPrivate;
class CoreExport QmaxDockWidget : public QDockWidget
{
	Q_OBJECT

	/** \brief The orientation of the dock widget.
	* The orientation property for the dock widget works equivalent to the 
	* orientation of the QToolBar. When the QmaxDockWidget is in a QMainWindow
	* docking layout then the orientation will be managed by the system.
	* For the left and right dock area the orientation will be Qt::Vertical. 
	* Which means top-down, the title bar is on top of the dock widget.
	* For the top and bottom dock area the orientation will be Qt::Horizontal.
	* \see orientation(), setOrientation() */
	Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation
	NOTIFY orientationChanged )

public:

	explicit QmaxDockWidget( const QString& objectName, const QString& title, QWidget* parent = nullptr, Qt::WindowFlags flags = 0 );
	explicit QmaxDockWidget( QmaxMainWindow::QmaxDockType dockType, const QString& objectName, const QString& title, QWidget* parent = nullptr, Qt::WindowFlags flags = 0 );
	explicit QmaxDockWidget( QWidget* parent = nullptr, Qt::WindowFlags flags = 0 );
	virtual ~QmaxDockWidget() override;

	// CUI content load / save
	/** \brief Loads CUI content from a file into the dock widget.
	* \see saveCUIContent() */
	bool loadCUIContent( const QString& fileName, int startup );

	/** \brief Save CUI content from the dock widget into a file.
	* \see loadCUIContent() */
	void saveCUIContent( const QString& fileName );

	/** \brief Sets the 3ds Max dock type on the dock widget.
	* \param[in] dockType The dock type of the actual docking content.
	* \see maxDockType() */
	void setMaxDockType( QmaxMainWindow::QmaxDockType dockType ) { QmaxMainWindow::setMaxDockType( this, dockType ); }

	/** \brief Returns the 3ds Max dock type for the widget.
	* \see setMaxDockType() */
	QmaxMainWindow::QmaxDockType maxDockType() const { return QmaxMainWindow::maxDockType( this ); }

	/** \brief Sets the 3ds Max specific dock flags on the dock widget.
	* \param[in] dockFlags The 3ds Max specific dock flags.
	* \see maxDockFlags(), addMaxDockFlags() */
	void setMaxDockFlags( QmaxMainWindow::QmaxDockFlags dockFlags ) { QmaxMainWindow::setMaxDockFlags( this, dockFlags ); }

	/** \brief Adds the 3ds Max specific dock flags on the dock widget.
	* \param[in] dockFlags The 3ds Max specific dock flags.
	* \see maxDockFlags(), setMaxDockFlags()
	*/
	void addMaxDockFlags( QmaxMainWindow::QmaxDockFlags dockFlags ) { QmaxMainWindow::addMaxDockFlags( this, dockFlags ); }

	/** \brief Returns the 3ds Max specific dock flags for the dock widget.
	* \see setMaxDockFlags(), addMaxDockFlags() */
	QmaxMainWindow::QmaxDockFlags maxDockFlags() const { return QmaxMainWindow::maxDockFlags( this ); }	

	/** \brief Returns the orientation of the dock widget.
	* \see setOrientation() */
	Qt::Orientation orientation() const;

	/** \brief Sets the orientation of the dock widget.
	* \see orientation() */
	void setOrientation( Qt::Orientation orientation );

	/** \brief Returns the docking area of the dock widget in the parent main window layout.
	* If dock widget has not been added to a main window, the return value will be Qt::NoDockWidgetArea. */
	Qt::DockWidgetArea dockWidgetArea();

Q_SIGNALS:

	/** \brief Emitted when the dock widget orientation has changed.
	* \param orientation The new dock widget orientation.
	* \see orientation(), setOrientation(). */
	void orientationChanged( Qt::Orientation orientation );


protected:

	virtual bool event( QEvent* evt ) override;
	virtual void paintEvent( QPaintEvent* evt ) override;

private:

	Q_DECLARE_PRIVATE( QmaxDockWidget )
	Q_DISABLE_COPY( QmaxDockWidget )
	QmaxDockWidgetPrivate* d_ptr;
};

};
//**************************************************************************/
// Copyright (c) 1998-2016 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: A sample (doing nothing) utility plugin to demonstrate the
// various usage options of the MaxSDK::LoadMaxMultiResIcon().  This plugin
// uses three different methods to load multi-resolution icons and put them on Qt
// controls in a GUP rollup. 
// 
// AUTHOR: Ulrich Haberl
//***************************************************************************/

#include "qtIconsDemo.h"
#include "maxicon.h"
#include "icolorman.h"
#include "notify.h"

#pragma warning( push )
#pragma warning( disable: 4127 4512 )

#include <QtCore/QtCore>
#include <QtGui/QtGui>

#pragma warning( pop )

#define IconDemo_CLASS_ID	Class_ID(0x70a84836, 0x2a5669c8)


class qtIconsDemo : public UtilityObj 
{
public:
		
	//Constructor/Destructor
	qtIconsDemo() { ; }
	virtual ~qtIconsDemo() { ; }

	virtual void DeleteThis() override { } // this class shall not be deleted by the reference system.
	
	virtual void BeginEditParams( Interface *ip, IUtil *iu ) override;
	virtual void EndEditParams( Interface *ip, IUtil *iu ) override;

	// Singleton access
	static qtIconsDemo* GetInstance() { 
		static qtIconsDemo theIconDemo; // the one and only static instance
		return &theIconDemo; 
	}

	using Theme = IColorManager::AppFrameColorTheme;
	void RegisterDynamicResources( const Theme& theme = Theme::kDarkTheme );
	void UnregisterDynamicResources();
	static void OnThemeChangedNotificationProc( void* param, NotifyInfo* info ); 

private:

	// Keeping the QWidget around in a QPointer allows us to see if the QWidget 
	// has been deleted elsewhere. 

	QPointer<QWidget> m_Widget;

	// The registered path for the compiled icon bundles - see the qtIconsDemo.qrc 
	// file and the custom build tool on the file on how these qtIconsDemo_Dark.rcc 
	// and qtIconsDemo_Light.rcc are getting generated.
	QString m_registeredRCCPath;
};


class qtIconsDemoClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return qtIconsDemo::GetInstance(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return IconDemo_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("qtIconsDemo"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }		// returns owning module handle
};


ClassDesc2* GetIconDemoDesc() 
{ 
	static qtIconsDemoClassDesc IconDemoDesc;
	return &IconDemoDesc; 
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void qtIconsDemo::BeginEditParams( Interface* ip, IUtil* iu )
{
	Q_UNUSED( iu );

	RegisterDynamicResources( GetColorManager()->GetAppFrameColorTheme() ); 

	// This allows the plugin to support automatic reloading of the icons on 
	// theme change:
	RegisterNotification( &qtIconsDemo::OnThemeChangedNotificationProc, this, NOTIFY_PRE_APP_FRAME_THEME_CHANGED );

	auto widget = new QWidget();
	auto layout = new QVBoxLayout( widget );

	//-------------------------------------------------------------------------
	// Shared common 3dsmax icon
	//-------------------------------------------------------------------------
	auto common_icon = MaxSDK::LoadMaxMultiResIcon( "Common/Delete" );

	//-------------------------------------------------------------------------
	// Dynamic loaded resource-bundles - based on Dark/Light theme, another set
	// of resources will get loaded into the Qt Resource system.
	// See the RegisterDynamicResources() function in this file for details.
	//
	// Note the ':' at the beginning of the path. This is how Qt 
	// defines a path into the resource system.
	//-------------------------------------------------------------------------
	auto dynamic_resource_icon_1 = MaxSDK::LoadMaxMultiResIcon( ":/qtIconsDemo/Icons/Array" );
	auto dynamic_resource_icon_2 = MaxSDK::LoadMaxMultiResIcon( ":/qtIconsDemo/Icons/Snapshot" );

	//-------------------------------------------------------------------------
	// File based icons - put your icons into UI_ln/Icons/[Dark|Light]/... and
	// use the base name of that file to load the icons.
	//-------------------------------------------------------------------------

	// This line loads a bunch of files stored on disk from: 
	//   UI_ln/Icons/[Dark|Light]/qtIconsDemo/Cut_24.png
	//   UI_ln/Icons/[Dark|Light]/qtIconsDemo/Cut_30.png
	//   ... etc
	auto file_icon = MaxSDK::LoadMaxMultiResIcon( "qtIconsDemo/Cut" );


	//-------------------------------------------------------------------------
	// Some sample Qt UI elements using the icons
	//-------------------------------------------------------------------------

	auto toolbar = new QToolBar();
	layout->addWidget( toolbar );

	auto common_icon_action = new QAction( common_icon, "Common/Delete", widget );
	common_icon_action->setToolTip( "Common 3dsmax icon loaded from 'Common/Delete'" );
	toolbar->addAction( common_icon_action );

	auto dynamic_resource_icon_1_action = new QAction( dynamic_resource_icon_1, "qtIconsDemo/Array", widget );
	dynamic_resource_icon_1_action->setToolTip( "Icon loaded from '" + m_registeredRCCPath + "'" );
	toolbar->addAction( dynamic_resource_icon_1_action );

	auto dynamic_resource_icon_2_action = new QAction( dynamic_resource_icon_2, "qtIconsDemo/Snapshot", widget );
	dynamic_resource_icon_2_action->setToolTip( "Icon loaded from '" + m_registeredRCCPath + "'" );
	toolbar->addAction( dynamic_resource_icon_2_action );

	auto file_icon_action = new QAction( file_icon, "qtIconsDemo/Cut", widget );
	file_icon_action->setToolTip( "Icon loaded from file 'UI_ln/Icons/[Dark|Light]/qtIconsDemo/Cut_*.png'" );
	toolbar->addAction( file_icon_action );
	
	auto button = new QToolButton(); 
	button->setIcon( common_icon );
	button->setText( "Common Icon" );
	button->setToolTip( "Common 3dsmax icon loaded from 'Common/Delete'" ); 
	button->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	// Make the QToolButton use ALL the existing space: 
	button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
	layout->addWidget( button );

	button = new QToolButton();
	button->setIcon( dynamic_resource_icon_1 );
	button->setText( ":/qtIconsDemo/Icons/Array" );
	button->setToolTip( "Icon loaded from '" + m_registeredRCCPath + "'" );
	button->setCheckable( true );
	button->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	// Make the QToolButton use ALL the existing space: 
	button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
	layout->addWidget( button );

	button = new QToolButton();
	button->setIcon( dynamic_resource_icon_2 );
	button->setText( ":/qtIconsDemo/Icons/Snapshot" );
	button->setToolTip( "Icon loaded from '" + m_registeredRCCPath + "'" );
	button->setCheckable( true );
	button->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	// Make the QToolButton use ALL the existing space: 
	button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
	layout->addWidget( button );

	button = new QToolButton();
	button->setIcon( file_icon );
	button->setText( "qtIconsDemo/Cut" );
	button->setToolTip( "Icon loaded from file 'UI_ln/Icons/[Dark|Light]/qtIconsDemo/Cut_*.png'" );
	button->setCheckable( true );
	button->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	// Make the QToolButton use ALL the existing space: 
	button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
	layout->addWidget( button );
		
	m_Widget = widget; 
	ip->AddRollupPage( *m_Widget.data(), L"Qt Icon Demo" );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void qtIconsDemo::EndEditParams( Interface* ip, IUtil* )
{
	ip->DeleteRollupPage( *m_Widget.data() );

	// No need to listen for this change any more.
	UnRegisterNotification( &qtIconsDemo::OnThemeChangedNotificationProc, this, NOTIFY_PRE_APP_FRAME_THEME_CHANGED );
	// Unregistering our resource bundle
	UnregisterDynamicResources();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void qtIconsDemo::RegisterDynamicResources( const Theme& theme )
{
	// We may have to unregister the previously registered icons.
	UnregisterDynamicResources();

	m_registeredRCCPath = MSTR( GetCOREInterface()->GetDir( APP_UI_LN_DIR ) ) + "qtIconsDemo_" +
		( ( theme == Theme::kDarkTheme ) ? "Dark.rcc" : "Light.rcc" );

	if ( !QResource::registerResource( m_registeredRCCPath ) )
	{
		m_registeredRCCPath.clear();
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void qtIconsDemo::UnregisterDynamicResources()
{
	if ( !m_registeredRCCPath.isEmpty() )
	{
		QResource::unregisterResource( m_registeredRCCPath );
		m_registeredRCCPath.clear();
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void qtIconsDemo::OnThemeChangedNotificationProc( void* param, NotifyInfo* info )
{
	Q_UNUSED( info ); 

	auto iconDemo = static_cast< qtIconsDemo* >( param );
	iconDemo->RegisterDynamicResources( GetColorManager()->GetAppFrameColorTheme() );
}

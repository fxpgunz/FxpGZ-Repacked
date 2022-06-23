//-----------------------------------------------------------------------------
// ---------------------
// File ....: NodeTrak.h
// ---------------------
// Author...: Gus J Grubba
// Date ....: February 1996
// Descr....: NodeTrak Image Filter
//
// History .: Feb, 21 1995 - Started
//            
//-----------------------------------------------------------------------------
        
#ifndef _NODETRAKCLASS_
#define _NODETRAKCLASS_

#define DLLEXPORT __declspec(dllexport)

//-----------------------------------------------------------------------------
//-- Configuration Block ------------------------------------------------------
//

#define NODETRAKVERSION 100

struct NODETRAKDATA_A {
     DWORD	version;
     char	nodename_a[MAX_PATH];
};

struct NODETRAKDATA_W {
	DWORD	version;
	char	nodename_a[MAX_PATH];
	wchar_t	nodename_w[MAX_PATH];

	void	setNodeName(const TCHAR* pName)
	{
		WStr nodeName = WStr::FromMCHAR(pName);
		wcsncpy(nodename_w,nodeName.data(),_countof(nodename_w)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(nodename_a, nodeName.ToCP(codePage), _countof(nodename_a)-1);
	}
	const TCHAR* getNodeName() const 
	{
#ifdef _UNICODE
		return nodename_w;
#else
		return nodename_a;
#endif
	}
};

//-----------------------------------------------------------------------------
//-- Class Definition ---------------------------------------------------------
//

//-- Used with DoHitByNameDialog();

class ImageFilter_NodeTrak;

class NodeDlg : public HitByNameDlgCallback {

		ImageFilter_NodeTrak *flt;

	public:

					NodeDlg(ImageFilter_NodeTrak *f) {flt = f;} 

		const TCHAR		*dialogTitle()				{ return _T("Node Tracker"); }
		const TCHAR		*buttonText() 				{ return _T("Ok"); }
		BOOL		singleSelect()				{ return TRUE; }
		BOOL		useFilter()					{ return FALSE; }
		BOOL		useProc()					{ return TRUE; }
		void		proc(INodeTab &nodeTab);

};

class ImageFilter_NodeTrak : public ImageFilter {
    
		NODETRAKDATA_W			data;
		NodeDlg					*nodeDlg;
		ILoad					*iload;

     public:
     
        //-- Constructors/Destructors
        
                       ImageFilter_NodeTrak( );
                      ~ImageFilter_NodeTrak( ) {}
               
        //-- Filter Info  ---------------------------------

        const TCHAR   *Description         ( ) ;
        const TCHAR   *AuthorName          ( ) { return _T("Gus J Grubba");}
        const TCHAR   *CopyrightMessage    ( ) { return _T("Copyright 1996, Yost Group");}
        UINT           Version             ( ) { return (NODETRAKVERSION);}

        //-- Filter Capabilities --------------------------
        
        DWORD          Capability          ( ) { return(IMGFLT_FILTER	| 
        																IMGFLT_CONTROL); }

        //-- Show DLL's About box -------------------------
        
        void           ShowAbout           ( HWND hWnd );  
        BOOL           ShowControl         ( HWND hWnd );  

        //-- Show Time ------------------------------------
        
        BOOL           Render              ( HWND hWnd );

        //-- Filter Configuration -------------------------
        
        BOOL           LoadConfigure			( void *ptr, DWORD piDataSize );
        BOOL           SaveConfigure			( void *ptr );
        DWORD          EvaluateConfigure		( );
		BOOL           ValidatePiData			( ImageFilterInfo *fi, ILoad *pILoad );
        
        //-- Local Methods --------------------------------
        
		INT_PTR         Control					(HWND ,UINT ,WPARAM ,LPARAM );
		void            SetNodeName				(const TCHAR *name) { data.setNodeName(name);}

};

#endif

//-- EOF: NodeTrak.h ----------------------------------------------------------

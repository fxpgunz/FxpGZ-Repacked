#include "stdafx.h"
//#include "../MatchServer/vld/vld.h"


#include "ZPrerequisites.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "dxerr.h"

#include "main.h"
#include "resource.h"
#include "VersionNo.h"

#include "Mint4R2.h"
#include "ZApplication.h"
#include "MDebug.h"
#include "ZMessages.h"
#include "MMatchNotify.h"
#include "RealSpace2.h"
#include "Mint.h"
#include "ZGameInterface.h"
#include "RFrameWork.h"
#include "ZButton.h"
#include "ZDirectInput.h"
#include "ZActionDef.h"
#include "MRegistry.h"
#include "ZInitialLoading.h"
#include "MDebug.h"
#include "MCrashDump.h"
#include "ZEffectFlashBang.h"
#include "ZMsgBox.h"
#include "ZSecurity.h"
#include "ZStencilLight.h"
#include "ZReplay.h"
#include "ZUtil.h"
#include "ZOptionInterface.h"
#include "HMAC_SHA1.h"

#ifdef USING_VERTEX_SHADER
#include "RShaderMgr.h"
#endif

//#include "mempool.h"
#include "RLenzFlare.h"
#include "ZLocale.h"
#include "MSysInfo.h"

#include "MTraceMemory.h"
#include "ZInput.h"
#include "Mint4Gunz.h"
#include "SecurityTest.h"
#include "CheckReturnCallStack.h"
#include "ZGameInput.h"

RMODEPARAMS	g_ModeParams={800,600,true,D3DFMT_X8R8G8B8};

#define SUPPORT_EXCEPTIONHANDLING


#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#include "ZNHN_USA_Poll.h"
#endif

RRESULT RenderScene(void *pParam);

#define RD_STRING_LENGTH 512
char cstrReleaseDate[512];// = "ReleaseDate : 12/22/2003";

ZApplication	g_App;
MDrawContextR2* g_pDC = NULL;
MFontR2*		g_pDefFont = NULL;
ZDirectInput	g_DInput;
ZInput*			g_pInput = NULL;
Mint4Gunz		g_Mint;

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );

void zexit(int returnCode)
{
	// ���Ӱ���� ����� delete�Ǿ�� �����߻��� ��ü �α׸� �ùٸ��� ���� �� �ִ�.
	// �׳� exit()�ص� ZGameGuard�� �̱������� ������� ������ �Ҹ��ڿ��� ���Ӱ��尡 delete������ ��°������ �׶� ũ���ð� �Ͼ��.
	// exit()�ϱ� ���� ���Ӱ��带 �������� �����ϸ� �׷� ������ �Ͼ�� �ʴ´�.
	// ��ŷ ���� ���� ������ Ŭ���̾�Ʈ ����� exit�������� zexit�� ����.
	exit(returnCode);
}

void CrcFailExitApp() { 
#ifdef _PUBLISH
	PostMessage(g_hWnd, WM_CLOSE, 0, 0); 
#else
	int* crash = NULL;
	*crash = 0;
#endif
}

void _ZChangeGameState(int nIndex)
{
	GunzState state = GunzState(nIndex);

	if (ZApplication::GetGameInterface())
	{
		ZApplication::GetGameInterface()->SetState(state);
	}
}

//list<HANDLE>	g_FontMemHandles;
static bool bFirstLoad = true;

RRESULT OnCreate(void *pParam)
{
	g_App.PreCheckArguments();

	D3DMULTISAMPLE_TYPE type;
	switch (Z_VIDEO_ANTIALIAS)
	{
	case 0:
		type = D3DMULTISAMPLE_NONE; break;
	case 1:
		type = D3DMULTISAMPLE_2_SAMPLES; break;
	case 2:
		type = D3DMULTISAMPLE_4_SAMPLES; break;
	case 3:
		type = D3DMULTISAMPLE_8_SAMPLES; break;
	}

	RSetMultiSampling(type);
	bool resetDevice = false;
	RMODEPARAMS modeParams;

	if (RealSpace2::RGetMultiSampling() > D3DMULTISAMPLE_NONE)
	{
		modeParams = { RGetScreenWidth(), RGetScreenHeight(), Z_VIDEO_FULLSCREEN, RGetPixelFormat() };
		resetDevice = true;
	}

	if (resetDevice)
	{
		RResetDevice(&modeParams);
	}
	string strFileLenzFlare("System/LenzFlare.xml");
	RCreateLenzFlare(strFileLenzFlare.c_str());
	RGetLenzFlare()->Initialize();

	mlog("main : RGetLenzFlare()->Initialize() \n");

	RBspObject::CreateShadeMap("sfx/water_splash.bmp");

	sprintf_s( cstrReleaseDate, "Alpha GunZ - Ver %d", ZGetSVNRevision());
	mlog(cstrReleaseDate); mlog("\n");
	g_DInput.Create(g_hWnd, FALSE, FALSE);
	g_pInput = new ZInput(&g_DInput);

	RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
	RSetRenderFlags(RRENDER_CLEAR_BACKBUFFER);

	ZGetInitialLoading()->Initialize(  1, 0, 0, RGetScreenWidth(), RGetScreenHeight(), 0, 0, 1024, 768 );

	mlog("InitialLoading success.\n");

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*." FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			AddFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	g_pDefFont = new MFontR2;

	if( !g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, DEFAULT_FONT_HEIGHT, 1.0f) )

	{
		mlog("Fail to Create defualt font : MFontR2 / main.cpp.. onCreate\n" );
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
		g_pDefFont	= NULL;
	}


	g_pDC = new MDrawContextR2(RGetDevice());

#ifndef _FASTDEBUG
	if( ZGetInitialLoading()->IsUseEnable() )
	{
		ZGetInitialLoading()->AddBitmap(0, "Interface/default/loading/loading_adult.jpg");
		ZGetInitialLoading()->AddBitmapBar( "Interface/default/loading/loading.bmp" );
		ZGetInitialLoading()->SetText( g_pDefFont, 10, 30, cstrReleaseDate );

		ZGetInitialLoading()->SetPercentage( 0.0f );
		ZGetInitialLoading()->Draw( MODE_FADEIN, 0 , true );
	}
#endif
	g_Mint.Initialize(800, 600, g_pDC, g_pDefFont);
	Mint::GetInstance()->SetHWND(RealSpace2::g_hWnd);

	mlog("interface Initialize success\n");

	ZLoadingProgress appLoading("application");
	if(!g_App.OnCreate(&appLoading))
	{
		ZGetInitialLoading()->Release();
		return R_ERROR_LOADING;
	}

	ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
	ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME);
	ZGetSoundEngine()->SetEffectMute(Z_AUDIO_EFFECT_MUTE);
	ZGetSoundEngine()->SetMusicMute(Z_AUDIO_BGM_MUTE);


	g_Mint.SetWorkspaceSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	g_Mint.GetMainFrame()->SetSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	ZGetOptionInterface()->Resize(g_ModeParams.nWidth, g_ModeParams.nHeight);


    
	// Default Key
	for(int i=0; i<ZACTION_COUNT; i++)
	{
		ZACTIONKEYDESCRIPTION& keyDesc = ZGetConfiguration()->GetKeyboard()->ActionKeys[i];
		g_pInput->RegisterActionKey(i, keyDesc.nVirtualKey);
		if(keyDesc.nVirtualKeyAlt!=-1)
			g_pInput->RegisterActionKey(i, keyDesc.nVirtualKeyAlt);
	}

	g_App.SetInitialState();

//	ParseParameter(g_szCmdLine);

	ZGetFlashBangEffect()->SetDrawCopyScreen(true);

	static const char *szDone = "Done.";
	ZGetInitialLoading()->SetLoadingStr(szDone);
	if( ZGetInitialLoading()->IsUseEnable() )
	{
#ifndef _FASTDEBUG
		ZGetInitialLoading()->SetPercentage( 100.f );
		ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 ,true  );
#endif
		ZGetInitialLoading()->Release();
	}

	mlog("main : OnCreate() done\n");

	SetFocus(g_hWnd);

	bFirstLoad = false;


	return R_OK;
}


bool CheckDll(char* fileName, BYTE* SHA1_Value)
{
	BYTE digest[20];
	BYTE Key[GUNZ_HMAC_KEY_LENGTH];

	memset(Key, 0, 20);
	memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));

	CHMAC_SHA1 HMAC_SHA1 ;
	HMAC_SHA1.HMAC_SHA1_file(fileName, Key, GUNZ_HMAC_KEY_LENGTH, digest) ;

	if(memcmp(digest, SHA1_Value, 20) ==0)
	{
		return true;
	}

	return false;
}



RRESULT OnDestroy(void *pParam)
{
	mlog("Destroy gunz\n");

	g_App.OnDestroy();

	SAFE_DELETE(g_pDefFont);

	g_Mint.Finalize();

	mlog("interface finalize.\n");

	SAFE_DELETE(g_pInput);
	g_DInput.Destroy();

	mlog("game input destroy.\n");

	RGetShaderMgr()->Release();

//	g_App.OnDestroy();

	// mlog("main : g_App.OnDestroy()\n");

	ZGetConfiguration()->Destroy();

	mlog("game gonfiguration destroy.\n");

	delete g_pDC;

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*." FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			RemoveFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	MFontManager::Destroy();
	MBitmapManager::Destroy();
	MBitmapManager::DestroyAniBitmap();

	mlog("Bitmap manager destroy Animation bitmap.\n");


	// �޸�Ǯ ����
	ZBasicInfoItem::Release(); // �Ҵ�Ǿ� �ִ� �޸� ����
//	ZHPInfoItem::Release();

	ZGetStencilLight()->Destroy();
	LightSource::Release();

//	ZStencilLight::GetInstance()->Destroy();

	RBspObject::DestroyShadeMap();
	RDestroyLenzFlare();
	RAnimationFileMgr::GetInstance()->Destroy();
	
	ZStringResManager::ResetInstance();

	mlog("destroy gunz finish.\n");

	return R_OK;
}

RRESULT OnUpdate(void* pParam)
{
	//_ASSERTE( _CrtCheckMemory( ) );

	__BP(100, "main::OnUpdate");
	g_pInput->Update();

	g_App.OnUpdate();


	return R_OK;
}

RRESULT OnUpdateInput(void* param)
{
	///TODO: decouple gameinput
	return R_OK;
}

RRESULT OnRender(void *pParam)
{
	__BP(101, "main::OnRender");
	if( !RIsActive() && RIsFullScreen())
	{
		__EP(101);
		return R_NOTREADY;
	}

	g_App.OnDraw();


#ifdef _SMOOTHLOOP
	Sleep(10);
#endif

#ifndef _PUBLISH

	if(g_pDefFont) {
		static char __buffer[256];

		float fMs = 1000.f/g_fFPS;
		float fScore = 100-(fMs-(1000.f/60.f))*2;

		sprintf(__buffer, "FPS : %3.3f %.3f�� (%.3f ms)",g_fFPS,fScore,fMs);
		g_pDefFont->m_Font.DrawText( MGetWorkspaceWidth()-200,0,__buffer );
//		OutputDebugString(__buffer);
	}

#endif

	__EP(101);

	return R_OK;
}

RRESULT OnInvalidate(void *pParam)
{
	MBitmapR2::m_dwStateBlock=NULL;

	g_App.OnInvalidate();
	
	return R_OK;
}

RRESULT OnRestore(void *pParam)
{
	for(int i=0; i<MBitmapManager::GetCount(); i++){
		MBitmapR2* pBitmap = (MBitmapR2*)MBitmapManager::Get(i);
			pBitmap->OnLostDevice();
	}

	g_App.OnRestore();

	return R_OK;
}

RRESULT OnActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient() && Z_ETC_BOOST)
		ZGetGameClient()->PriorityBoost(true);
	return R_OK;
}

RRESULT OnDeActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient())
		ZGetGameClient()->PriorityBoost(false);
	return R_OK;
}

RRESULT OnError(void *pParam)
{
	mlog("RealSpace::OnError(%d) \n", RGetLastError());

	switch (RGetLastError())
	{
	case RERROR_INVALID_DEVICE:
		{
			D3DADAPTER_IDENTIFIER9 *ai=RGetAdapterID();
			char szLog[512];
			ZTransMsg( szLog, MSG_DONOTSUPPORT_GPCARD, 1, ai->Description);

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret!=IDYES)
				return R_UNKNOWN;
		}
		break;
	case RERROR_CANNOT_CREATE_D3D:
		{
			ShowCursor(TRUE);

			char szLog[512];
			sprintf(szLog, ZMsg( MSG_DIRECTX_NOT_INSTALL));

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret==IDYES)
			{
				ShellExecute(g_hWnd, "open", ZMsg( MSG_DIRECTX_DOWNLOAD_URL), NULL, NULL, SW_SHOWNORMAL); 
			}
		}
		break;

	};

	return R_OK;
}

void SetModeParams()
{
	g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
	g_ModeParams.nWidth = ZGetConfiguration()->GetVideo()->nWidth;
	g_ModeParams.nHeight = ZGetConfiguration()->GetVideo()->nHeight;
	g_ModeParams.PixelFormat = D3DFMT_X8R8G8B8;

}

void ResetAppResource()
{
	GunzState state = ZGetGameInterface()->GetState();
	ZGetStringResManager()->ResetInstance();
	g_App.InitLocale(true);
	g_App.ReInitializeGameInterface();
	g_App.ReInitializeStageOptionInterface();
	ZGetGameInterface()->SetFocusEnable(true);
	ZGetGameInterface()->SetFocus();
	ZGetGameInterface()->Show(true);
	MGetMatchItemDescMgr()->Clear();
	if (!MGetMatchItemDescMgr()->ReadCache())
	{
		if (!MGetMatchItemDescMgr()->ReadXml(FILENAME_ZITEM_DESC,ZGetFileSystem()))
		{
			MLog("Error while Read Item Descriptor %s\n", FILENAME_ZITEM_DESC);
		}
		if (!MGetMatchItemDescMgr()->ReadXml(FILENAME_ZITEM_DESC_LOCALE ,ZGetFileSystem()))
		{
			MLog("Error while Read Item Descriptor %s\n", FILENAME_ZITEM_DESC_LOCALE);
		}

		MGetMatchItemDescMgr()->WriteCache();
	}
	ZGetEmblemInterface()->Destroy();
	ZGetEmblemInterface()->Create();
	ZGetGameInterface()->SetState(state);

	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = false;
}

// ������ �������~~ -.-

int FindStringPos(char* str,char* word)
{
	if(!str || str[0]==0)	return -1;
	if(!word || word[0]==0)	return -1;

	int str_len = (int)strlen(str);
	int word_len = (int)strlen(word);

	char c;
	bool bCheck = false;

	for(int i=0;i<str_len;i++) {
		c = str[i];
		if(c == word[0]) {

			bCheck = true;

			for(int j=1;j<word_len;j++) {
				if(str[i+j]!=word[j]) {
					bCheck = false;
					break;
				}
			}

			if(bCheck) {
				return i;
			}
		}
	}
	return -1;
}

bool FindCrashFunc(char* pName)
{
//	Function Name
//	File Name 
	if(!pName) return false;

	FILE *fp;
	fp = fopen( "mlog.txt", "r" );
	if(fp==NULL)  return false;

	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char* pBuffer = new char [size];
	pBuffer[size] = 0;

	fread(pBuffer,1,size,fp);

	fclose(fp);

	// �츮 ����� ã�´�.
	int posSource = FindStringPos(pBuffer,"ublish");
	if(posSource==-1) return false;

	int posA = FindStringPos(pBuffer+posSource,"Function Name");
//	int posB = FindStringPos(pBuffer,"File Name");	
	// filename �� ���� ��쵵 �־ �̷��� �ٲ��
	int posB = posA + FindStringPos(pBuffer+posSource+posA,"\n");

	if(posA==-1) return false;
	if(posB==-1) return false;

	posA += 16;

//	int memsize = posB-posA-6;
	int memsize = posB-posA;
	memcpy(pName,&pBuffer[posA+posSource],memsize);

	pName[memsize] = 0;

	delete [] pBuffer;

	for(int i=0;i<memsize;i++) {
		if(pName[i]==':') {
			pName[i] = '-';
		}
	}

	return true;
}

void HandleExceptionLog()
{
	#define ERROR_REPORT_FOLDER	"ReportError"

	extern char* logfilename;	// Instance on MDebug.cpp

	// ERROR_REPORT_FOLDER �����ϴ��� �˻��ϰ�, ������ ����
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ERROR_REPORT_FOLDER, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (!CreateDirectory("ReportError", NULL)) {
			MessageBox(g_hWnd, "ReportError ������ ������ �� �����ϴ�.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
			return;
		}
	} else 	{
		FindClose(hFind);
	}


//2007�� 2�� 13�� BAReport ���̻� ��� ���ϰ� ����


	// mlog.txt �� ERROR_REPORT_FOLDER �� ����

	//acesaga_0928_911_moanus_rslog.txt
	//USAGE_EX) BAReport app=acesaga;addr=moon.maiet.net;port=21;id=ftp;passwd=ftp@;gid=10;user=moanus;localfile=rslog.txt;remotefile=remote_rslog.txt;

	if(ZGetCharacterManager()) {
		ZGetCharacterManager()->OutputDebugString_CharacterState();
	}


	ZGameClient* pClient = (ZGameClient*)ZGameClient::GetInstance();

	char* pszCharName = NULL;
	MUID uidChar;
	MMatchObjCache* pObj;
	char szPlayer[128];

	if( pClient ) {

		uidChar = pClient->GetPlayerUID();
		pObj = pClient->FindObjCache(uidChar);
		if (pObj)
			pszCharName = pObj->GetName();

		wsprintf(szPlayer, "%s(%d%d)", pszCharName?pszCharName:"Unknown", uidChar.High, uidChar.Low);
	}
	else { 
		wsprintf(szPlayer, "Unknown(-1.-1)");
	}


	if (pClient) {

		time_t currtime;
		time(&currtime);
		struct tm* pTM = localtime(&currtime);

		char cFuncName[1024];

		if(FindCrashFunc(cFuncName)==false) {
			strcpy(cFuncName,"Unknown Error");
		}

		char szFileName[_MAX_DIR], szDumpFileName[_MAX_DIR];
		wsprintf(szFileName, "%s_%s_%.2d%.2d_%.2d%.2d_%s_%s", cFuncName,
				APPLICATION_NAME, pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min, szPlayer, "mlog.txt");
		wsprintf(szDumpFileName, "%s.dmp", szFileName);

		char szFullFileName[_MAX_DIR], szDumpFullFileName[_MAX_DIR];
		wsprintf(szFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szFileName);
		wsprintf(szDumpFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szDumpFileName);

		if (CopyFile("mlog.txt", szFullFileName, TRUE))
		{
			CopyFile("Gunz.dmp", szDumpFullFileName, TRUE);

			 //BAReport ����
			char szCmd[4048];
			char szRemoteFileName[_MAX_DIR], szRemoteDumpFileName[_MAX_DIR];
			wsprintf(szRemoteFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szFileName);
			wsprintf(szRemoteDumpFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szDumpFileName);

			wsprintf(szCmd, "BAReport app=%s;addr=%s;port=21;id=ftp;passwd=ftp@;user=%s;localfile=%s,%s;remotefile=%s,%s", 
				APPLICATION_NAME, ZGetConfiguration()->GetBAReportAddr(), szPlayer, szFullFileName, szDumpFullFileName, szRemoteFileName, szRemoteDumpFileName);

			WinExec(szCmd, SW_SHOW);

			FILE *file = fopen("bareportpara.txt","w+");
			fprintf(file,szCmd);
			fclose(file);
		}
	}

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
#ifdef LOCALE_JAPAN
		case WM_COPYDATA:
			{
				ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
				if( ((ZGameOnJPAuthInfo*)pAuth)->NewLogin(wParam, lParam) )
				{
					MessageBox(g_hWnd, "Same id accessing from a different PC", NULL, MB_OK);
					zexit(-1);
				}
			}
			break;
#endif

		case WM_SYSCHAR:
			if(ZIsLaunchDevelop() && wParam==VK_RETURN)
			{
#ifndef _PUBLISH
				RFrame_ToggleFullScreen();
#endif
				return 0;
			}
			break;

		case WM_CREATE:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(false, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_DESTROY:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(true, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_SETCURSOR:
		{
			if (ZApplication::GetGameInterface())
				ZApplication::GetGameInterface()->OnResetCursor();
			return TRUE; // prevent Windows from setting cursor to window class cursor
		}break;
		case WM_ENTERIDLE:
			// ��� ��ȭ���ڰ� �ڵ带 ���ϰ� ���� �� �θ𿡰� ������ idle �����޽���
			// (�Ϻ� IME�� ��� ��ȭ���ڰ� �־ �־���)
			// ��� ��ȭ���ڷ� ������Ʈ ������ ���ؼ� ���� ������ �ǿ�Ǳ� ������ ���⼭ ������Ʈ�� �����Ѵ�
			RFrame_UpdateRender();
			break;

			/*
		case  WM_LBUTTONDOWN:
			SetCapture(hWnd);
			return TRUE;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return TRUE;
			*/
		case WM_KEYDOWN:
			{
				bool b = false;
		}break;
	}

	if(Mint::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam)==true)
	{
		if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())	// for language changing
		{
			ZGetGameInterface()->ReserveResetApp(false);
			ResetAppResource();
		}

		return 0;
	}

	// thread safe�ϱ����� ����
	if (message == WM_CHANGE_GAMESTATE)
	{
		_ZChangeGameState(wParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
class mtrl {
public:

};

class node {
public:
	int		m_nIndex[5];
};


class _map{
public:
	mtrl* GetMtrl(node* node,int index) { return GetMtrl(node->m_nIndex[index]); }
	mtrl* GetMtrl(int id) { return m_pIndex[id]; }

	mtrl*	m_pIndex[5];
};

class game {
public:
	_map m_map;	
};

game _game;
game* g_game;
*/


void ClearTrashFiles()
{
}

bool CheckFileList()
{
	MZFileSystem *pfs = ZApplication::GetFileSystem();
	MZFile mzf;

	rapidxml::xml_document<> doc;

	string strFileNameFillist(FILENAME_FILELIST);
	if (!mzf.Open(strFileNameFillist.c_str(), pfs))
		return false;

	char *buffer = new char[mzf.GetLength() + 1];
	mzf.Read(buffer, mzf.GetLength());
	buffer[mzf.GetLength()] = 0;
	if (!doc.parse<0>(buffer)) {
		mlog("Error parsing %s", strFileNameFillist.c_str());
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "FILE") == 0)
		{
			char szContents[256], szCrc32[256];
			strcpy_s(szContents, itor->first_attribute("NAME")->value());
			strcpy_s(szCrc32, itor->first_attribute("CRC32")->value());

			if (_stricmp(szContents, "config.xml") != 0)
			{
				unsigned int crc32_list = pfs->GetCRC32(szContents);
				unsigned int crc32_current;
				sscanf(szCrc32, "%x", &crc32_current);

				if (crc32_current != crc32_list)
					return false;
			}
		}
	}
	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}


enum RBASE_FONT{
	RBASE_FONT_GULIM = 0,
	RBASE_FONT_BATANG = 1,

	RBASE_FONT_END
};

static int g_base_font[RBASE_FONT_END];
static char g_UserDefineFont[256];

bool _GetFileFontName(char* pUserDefineFont)
{
	if(pUserDefineFont==NULL) return false;

	FILE* fp = fopen("_Font", "rt");
	if (fp) {
		fgets(pUserDefineFont,256, fp);
		fclose(fp);
		return true;
	}
	return false;
}


bool CheckFont()
{
	char FontPath[MAX_PATH];
	char FontNames[MAX_PATH+100];

	::GetWindowsDirectory(FontPath, MAX_PATH);

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\gulim.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_GULIM] = 1; }
	else							{ g_base_font[RBASE_FONT_GULIM] = 0; }

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\batang.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_BATANG] = 1; }
	else							{ g_base_font[RBASE_FONT_BATANG] = 0; }

	//	strcpy(FontNames,FontPath);
	//	strcat(FontNames, "\\Fonts\\System.ttc");
	//	if (_access(FontNames,0) != -1)	{ g_font[RBASE_FONT_BATANG] = 1; }
	//	else							{ g_font[RBASE_FONT_BATANG] = 0; }

	if(g_base_font[RBASE_FONT_GULIM]==0 && g_base_font[RBASE_FONT_BATANG]==0) {//�Ѵپ�����..

		if( _access("_Font",0) != -1) { // �̹� ��ϵǾ� �ִٸ�..
			_GetFileFontName( g_UserDefineFont );
		}
		else {

			int hr = IDOK;

			//hr = ::MessageBox(NULL,"������ ��ǻ�Ϳ��� ��� ����ϴ� (����,����) ��Ʈ�� ���� �� �����ϴ�.\n �ٸ� ��Ʈ�� ���� �Ͻðڽ��ϱ�?","�˸�",MB_OKCANCEL);
			//hr = ::MessageBox(NULL,"������ ��ǻ�Ϳ��� ��� ����ϴ� (����,����) ��Ʈ�� ���� �� �����ϴ�.\n ��� ���� �Ͻðڽ��ϱ�?","�˸�",MB_OKCANCEL);

			if(hr==IDOK) {
				/*			
				CFontDialog dlg;
				if(dlg.DoModal()==IDOK) {
				CString facename = dlg.GetFaceName();
				lstrcpy((LPSTR)g_UserDefineFont,(LPSTR)facename.operator const char*());

				hr = ::MessageBox(NULL,"�����Ͻ� ��Ʈ�� ���� �Ͻðڽ��ϱ�?","�˸�",MB_OKCANCEL);

				if(hr==IDOK)
				_SetFileFontName(g_UserDefineFont);
				}
				*/
				return true;
			}
			else {
				return false;
			}
		}
	}
	return true;
}

#include "shlobj.h"

void CheckFileAssociation()
{
#define GUNZ_REPLAY_CLASS_NAME	"GunzReplay"

	// üũ�غ��� ����� �ȵǾ������� ����Ѵ�. ����ڿ��� ������� �ְڴ�.
	char szValue[256];
	if(!MRegistry::Read(HKEY_CLASSES_ROOT,"." GUNZ_REC_FILE_EXT,NULL,szValue))
	{
		MRegistry::Write(HKEY_CLASSES_ROOT,"." GUNZ_REC_FILE_EXT,NULL,GUNZ_REPLAY_CLASS_NAME);

		char szModuleFileName[_MAX_PATH] = {0,};
		GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);

		char szCommand[_MAX_PATH];
		sprintf(szCommand,"\"%s\" \"%%1\"",szModuleFileName);

		MRegistry::Write(HKEY_CLASSES_ROOT,GUNZ_REPLAY_CLASS_NAME"\\shell\\open\\command",NULL,szCommand);

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);
	}
}

void UpgradeMrsFile()
{
	return;
	//char temp_path[ 1024];
	//sprintf( temp_path,"*");

	//FFileList file_list;
	//GetFindFileListWin(temp_path,".mrs",file_list);
	//file_list.UpgradeMrs();
}

HANDLE Mutex = 0;

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
#endif

DWORD g_dwMainThreadID;


//------------------------------------------- nhn usa -------------------------------------------------------------
bool InitReport()
{
#ifdef LOCALE_NHNUSA
	mlog( "Init report start\n" );
	if( !GetNHNUSAReport().InitReport(((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetUserID().c_str(),
		((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
	{
		mlog( "Init nhn report fail.\n" );
		return false;
	}
	GetNHNUSAReport().ReportStartGame();
	mlog( "Init report success.\n" );
#endif

	return true;
}

bool InitPoll()
{
#ifdef LOCALE_NHNUSA
	mlog( "Init poll start\n" );

	((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->ZUpdateGameString();

	if( !GetNHNUSAPoll().ZHanPollInitGameString( ((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
		return false;
#endif

	return true;
}

#include "../sdk/md5/md5.h"

//------------------------------------------- nhn usa end----------------------------------------------------------
int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);

	InitLog(MLOGSTYLE_DEBUGSTRING|MLOGSTYLE_FILE);

	g_fpOnCrcFail = CrcFailExitApp;

#ifdef LOCALE_JAPAN
	ZGameOnJPAuthInfo::m_hLauncher = ::FindWindow( NULL, TITLE_PUBLAGENT );
#endif

	g_dwMainThreadID = GetCurrentThreadId();
	
#ifdef _MTRACEMEMORY
	MInitTraceMemory();
#endif

	//_CrtSetBreakAlloc(994464);

	// Current Directory�� �����.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

#ifndef _GAMEGUARD
	#ifdef _PUBLISH
		// �ߺ� ���� ����
		//Mutex = CreateMutex(NULL, TRUE, "Gunz");
		//if (GetLastError() == ERROR_ALREADY_EXISTS)
		//{
		//	zexit(-1);
		//	return 0;
		//}
	#endif
#endif


	ClearTrashFiles();

	srand( (unsigned)time( NULL ));

	//#ifndef _DEBUG
#ifdef _PUBLISH
		// GunzLock�� ������� Gunz.exe�� �����ϸ� �������� ����Ѵ�. (XProtector ���μ����̹�����ĵ �۾���)
	/*	HANDLE hMutexGunzLock = CreateMutex(NULL, TRUE, "GunzLock");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			WaitForSingleObject(hMutexGunzLock, INFINITE);
			CloseHandle(hMutexGunzLock);
		}*/
#endif

	// �α� ����
	mlog("GUNZ " STRFILEVER " launched. build (" __DATE__" " __TIME__") \n");
	char szDateRun[128]="";
	char szTimeRun[128]="";
	_strdate( szDateRun );
	_strtime( szTimeRun );
	mlog("Log time (%s %s)\n", szDateRun, szTimeRun);

#ifndef _PUBLISH
	mlog("cmdline = %s\n",cmdline);

#endif

#ifndef _LAUNCHER
	UpgradeMrsFile();// mrs1 �̶�� mrs2�� ���׷��̵� �Ѵ�..
#endif

	MSysInfoLog();

	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*)&os);


//	if (CheckVideoAdapterSupported() == false)
//		return 0;

	CheckFileAssociation();

	// Initialize MZFileSystem - MUpdate 
	MRegistry::szApplicationName=APPLICATION_NAME;
	g_App.ShiftBytesOnStart();
	g_App.InitFileSystem();

#ifdef _PUBLISH
//	#ifndef NETMARBLE_VERSION
		MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
//	#endif
#endif

	// config�� string �ε�
	ZGetConfiguration()->Load();

	if (os.dwMajorVersion >= 6 && Z_VIDEO_D3D9EX) {
		g_isDirect3D9ExEnabled = true;
	}


	ZStringResManager::MakeInstance();
	if( !ZApplication::GetInstance()->InitLocale() )
	{
		MLog("Locale Init error !!!\n");
		return false;
	}

	ZGetConfiguration()->Load_StringResDependent();

	// ���⼭ ��ũ�� ������... �հ� ��������~~ -by SungE.
	if( !ZGetConfiguration()->LateStringConvert() )
	{
		MLog( "main.cpp - Late string convert fale.\n" );
		return false;
	}

	DWORD ver_major = 0;
	DWORD ver_minor = 0;
	TCHAR ver_letter = ' ';


#ifdef SUPPORT_EXCEPTIONHANDLING
	char szDumpFileName[256];
	sprintf(szDumpFileName, "Gunz.dmp");
	__try{
#endif

	if (ZApplication::GetInstance()->ParseArguments(cmdline) == false)
	{
		return 0;
	}

//#ifdef _PUBLISH
	// if(!CheckFileList()) {
		// �����ϴ°��� �ϴ� ����
		// int ret=MessageBox(NULL, "������ �ջ�Ǿ����ϴ�.", "�߿�!", MB_OK);
		// return 0;
	//}
//#endif



#ifndef _DEBUG
	//BYTE SHA_fmod[20] = {0x88,0x8f,0x1f,0x7b,0x7e,0x3c,0x43,0x38,0x4f,0x4b,0x80,0xb5,0x77,0xfe,0x09,0x1a,0xc0,0x45,0x38,0x3c};
	//BYTE SHA_dbghelp[20] = {0x8d,0x12,0xc4,0x3a,0x84,0x12,0xc2,0x1,0x58,0xb4,0x46,0x70,0x9,0x5,0xcb,0xd4,0xfa,0xb1,0xe2,0x4b}; 
	//if(!CheckDll("fmod.dll", SHA_fmod) ||
	//	!CheckDll("dbghelp.dll", SHA_dbghelp) )
	//{
	//	MessageBox(g_hWnd,"Dll Hacking detected",  NULL, MB_OK);
	//	return false;
	//}

#endif


#ifndef LOCALE_NHNUSA
	if (ZCheckHackProcess() == true)
	{
//		MessageBox(NULL,
//			ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		mlog(ZMsg(MSG_HACKING_DETECTED));
		mlog("\n");
		mlog("I'm hacker.\n" );
		return 0;
	}
#endif

	if(!InitializeNotify(ZApplication::GetFileSystem())) {
		MLog("Check notify.xml\n");
		return 0;
	}
	else 
	{
		mlog( "InitializeNotify ok.\n" );
	}

	// font �ִ°� �˻�..

	if(CheckFont()==false) {
		MLog("��Ʈ�� ���� ������ ��Ʈ ������ ���\n");
		return 0;
	}

	RSetFunction(RF_CREATE	,	OnCreate);
	RSetFunction(RF_RENDER	,	OnRender);
	RSetFunction(RF_UPDATE	,	OnUpdate);
	RSetFunction(RF_DESTROY ,	OnDestroy);
	RSetFunction(RF_INVALIDATE,	OnInvalidate);
	RSetFunction(RF_RESTORE,	OnRestore);
	RSetFunction(RF_ACTIVATE,	OnActivate);
	RSetFunction(RF_DEACTIVATE,	OnDeActivate);
	RSetFunction(RF_ERROR,		OnError);
	RSetFunction(RF_UPDATEINPUT, OnUpdateInput);
	SetModeParams();

//	while(ShowCursor(FALSE)>0);

	int nRMainReturn = RMain(APPLICATION_NAME, this_inst, prev_inst, cmdline, cmdshow, &g_ModeParams, WndProc, IDI_ICON1);
	if (0 != nRMainReturn)
	{
		mlog("error making window");
		return nRMainReturn;
	}


	if( 0 != RInitD3D(&g_ModeParams) )
	{
		MessageBox(g_hWnd, "fail to initialize DirectX", NULL, MB_OK);
		mlog( "error init RInitD3D\n" );
		return 0;
	}

	const int nRRunReturn = RRun();

	//����� ���Ӱ��尡 Xfire�� �޸𸮾��� ������ �����ϴµ� �̶� ����â�� Ǯ��ũ�� �ڿ� �ߴ� �� �����ϱ� ����
	//�������� ��� �ּ�ȭ/��Ȱ��ȭ ���ѳ��´�. xfire�� �ﰢ���� ���� �ذ��� ����ϱ� �����Ƿ� �̷��� ó��
	ShowWindow(g_hWnd, SW_MINIMIZE);

#ifdef _MTRACEMEMORY
	MShutdownTraceMemory();
#endif

#ifdef _HSHIELD
	_AhnHS_StopService();
	_AhnHS_Uninitialize();		
#endif


#ifdef LOCALE_NHNUSA
	mlog( "Poll Process\n" );
	int nRetPoll = GetNHNUSAPoll().ZHanPollProcess( NULL);
#endif

	ZStringResManager::FreeInstance();

	return nRRunReturn;

//	ShowCursor(TRUE);

#ifdef SUPPORT_EXCEPTIONHANDLING
	}

	//__except(MFilterException(GetExceptionInformation())){
	__except(CrashExceptionDump(GetExceptionInformation(), szDumpFileName, true))
	{
#ifdef LOCALE_NHNUSA
		GetNHNUSAReport().ReportCrashedGame();
#endif

		//HandleExceptionLog();
//		MessageBox(g_hWnd, "����ġ ���� ������ �߻��߽��ϴ�.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
	}
#endif

#ifdef _PUBLISH
	if (Mutex != 0) CloseHandle(Mutex);
#endif

//	CoUninitialize();

	return 0;
}

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam)
{
//	TCHAR szTitle[256];

	switch(lCode)
	{
		//Engine Callback
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("���� ��ġ���� ��ŷ���� �߰ߵǾ� ���α׷��� ������׽��ϴ�.\n%s"), (char*)pParam);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//�Ϻ� API�� �̹� ��ŷ�Ǿ� �ִ� ����
		//�׷��� �����δ� �̸� �����ϰ� �ֱ� ������ �ٸ� ��ŷ�õ� ���α׷��� �������� �ʽ��ϴ�.
		//�� Callback�� ���� ��� ������ �������� �������� �����ǹǷ� ������ ������ �ʿ䰡 �����ϴ�.
	case AHNHS_ACTAPC_DETECT_ALREADYHOOKED:
		{
			PACTAPCPARAM_DETECT_HOOKFUNCTION pHookFunction = (PACTAPCPARAM_DETECT_HOOKFUNCTION)pParam;
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("[HACKSHIELD] Already Hooked\n- szFunctionName : %s\n- szModuleName : %s\n"), 
				pHookFunction->szFunctionName, pHookFunction->szModuleName);
			OutputDebugString(szMsg);
			break;
		}

		//Speed ����
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
		{
//			MessageBox(NULL, _T("���� �� PC���� SpeedHack���� �ǽɵǴ� ������ �����Ǿ����ϴ�."), szTitle, MB_OK);
			mlog("���� �� PC���� SpeedHack���� �ǽɵǴ� ������ �����Ǿ����ϴ�.");
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//����� ���� 
	case AHNHS_ACTAPC_DETECT_KDTRACE:	
	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("���α׷��� ���Ͽ� ����� �õ��� �߻��Ͽ����ϴ�. (Code = %x)\n���α׷��� �����մϴ�."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//�׿� ��ŷ ���� ��� �̻� 
	case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("��ŷ ��� ��ɿ� �̻��� �߻��Ͽ����ϴ�. (Code = %x)\n���α׷��� �����մϴ�."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
	}

	return 1;
}
#endif
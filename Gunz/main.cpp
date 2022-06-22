#include "stdafx.h"

#ifdef _XTRAP
#include "./XTrap/Xtrap_C_Interface.h"						
#include "./XTrap/XTrap4Launcher.h"
#pragma comment (lib, "./XTrap/XTrap4Launcher_mt.lib")
#pragma comment (lib, "./XTrap/XTrap4Client_mt.lib")	
#endif

#include "ZPrerequisites.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "dxerr9.h"

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

#include "RLenzFlare.h"
#include "ZLocale.h"
#include "MSysInfo.h"

#include "MTraceMemory.h"
#include "ZInput.h"
#include "Mint4Gunz.h"
#include "SecurityTest.h"
#include "CheckReturnCallStack.h"



#ifdef _DEBUG
RMODEPARAMS	g_ModeParams = { 640,480,false,D3DFMT_R5G6B5 };
//RMODEPARAMS	g_ModeParams={1024,768,false,RPIXELFORMAT_565};
#else
RMODEPARAMS	g_ModeParams = { 800, 600, true, D3DFMT_R5G6B5 };
#endif


#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#include "ZNHN_USA_Poll.h"
#endif

#ifdef _GAMEGUARD
#include "ZGameguard.h"
#endif

RRESULT RenderScene(void* pParam);

#define RD_STRING_LENGTH 512
char cstrReleaseDate[512];// = "ReleaseDate : 12/22/2003";

ZApplication	g_App;
MDrawContextR2* g_pDC = NULL;
MFontR2* g_pDefFont = NULL;
ZDirectInput	g_DInput;
ZInput* g_pInput = NULL;
Mint4Gunz		g_Mint;

HRESULT GetDirectXVersionViaDxDiag(DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter);

void zexit(int returnCode)
{
#ifdef _GAMEGUARD
	GetZGameguard().Release();
#endif
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

list<HANDLE>	g_FontMemHandles;

RRESULT OnCreate(void* pParam)
{
	g_App.PreCheckArguments();

	string strFileLenzFlare("System/LenzFlare.xml");
#ifndef _DEBUG
	strFileLenzFlare += "";
#endif
	RCreateLenzFlare(strFileLenzFlare.c_str());
	RGetLenzFlare()->Initialize();

	mlog("main : RGetLenzFlare()->Initialize() \n");

	RBspObject::CreateShadeMap("sfx/water_splash.bmp");

	sprintf(cstrReleaseDate, "Date : %s", __DATE__);
	mlog(cstrReleaseDate); mlog("\n");
	g_DInput.Create(g_hWnd, FALSE, FALSE);
	g_pInput = new ZInput(&g_DInput);

	RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
	RSetRenderFlags(RRENDER_CLEAR_BACKBUFFER);

	ZGetInitialLoading()->Initialize(1, 0, 0, RGetScreenWidth(), RGetScreenHeight(), 0, 0, 1024, 768);

#ifdef _WHOLE_SCREEN_LOADING
	ZInitialLoading::drawMode = MODE_FULLSCREEN;
#endif

	mlog("InitialLoading success.\n");

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if ((hFile = _findfirst(FONT_DIR"*." FONT_EXT, &c_file)) != -1L) {
		do {
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			AddFontResource(szFileName);
		} while (_findnext(hFile, &c_file) == 0);
		_findclose(hFile);
	}

	g_pDefFont = new MFontR2;

	if (!g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, DEFAULT_FONT_HEIGHT, 1.0f))

	{
		mlog("Fail to Create defualt font : MFontR2 / main.cpp.. onCreate\n");
		g_pDefFont->Destroy();
		SAFE_DELETE(g_pDefFont);
		g_pDefFont = NULL;
	}


	g_pDC = new MDrawContextR2(RGetDevice());

#ifndef _FASTDEBUG
	if (ZGetInitialLoading()->IsUseEnable())
	{
		if (ZGetLocale()->IsTeenMode())
		{
			ZGetInitialLoading()->AddBitmap(0, "Interface/Default/LOADING/loading_teen.jpg");
		}
		else
		{
			ZGetInitialLoading()->AddBitmap(0, "Interface/Default/LOADING/loading_adult.jpg");
		}
		ZGetInitialLoading()->AddBitmapBar("Interface/Default/LOADING/loading.bmp");
		ZGetInitialLoading()->SetText(g_pDefFont, 10, 30, cstrReleaseDate);


		ZGetInitialLoading()->AddBitmapGrade("Interface/Default/LOADING/loading_grade_fifteen.jpg");


		ZGetInitialLoading()->SetPercentage(0.0f);
		ZGetInitialLoading()->Draw(MODE_FADEIN, 0, true);
	}
#endif

	g_Mint.Initialize(800, 600, g_pDC, g_pDefFont);
	Mint::GetInstance()->SetHWND(RealSpace2::g_hWnd);

	mlog("interface Initialize success\n");

	ZLoadingProgress appLoading("");
	if (!g_App.OnCreate(&appLoading))
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

	for (int i = 0; i < ZACTION_COUNT; i++) 
	{
		ZACTIONKEYDESCRIPTION& keyDesc = ZGetConfiguration()->GetKeyboard()->ActionKeys[i];
		g_pInput->RegisterActionKey(i, keyDesc.nVirtualKey);
		if (keyDesc.nVirtualKeyAlt != -1)
			g_pInput->RegisterActionKey(i, keyDesc.nVirtualKeyAlt);
	}

	g_App.SetInitialState();

	ZGetFlashBangEffect()->SetDrawCopyScreen(true);

	static const char* szDone = "";
	ZGetInitialLoading()->SetLoadingStr(szDone);
	if (ZGetInitialLoading()->IsUseEnable())
	{
#ifndef _FASTDEBUG
		ZGetInitialLoading()->SetPercentage(100.f);
		ZGetInitialLoading()->Draw(MODE_FADEOUT, 0, true);
#endif
		ZGetInitialLoading()->Release();
	}

	ChangeAA(ZGetConfiguration()->GetVideo()->nAntiAlias);
	RMODEPARAMS ModeParams = { RGetScreenWidth(), RGetScreenHeight(), RIsFullScreen(), RGetPixelFormat() };
	RResetDevice(&ModeParams);

#ifdef _WHOLE_SCREEN_LOADING
	ZInitialLoading::drawMode = MODE_BAR;
#endif

	mlog("main : OnCreate() done\n");

	SetFocus(g_hWnd);

	return R_OK;
}


bool CheckDll(char* fileName, BYTE* SHA1_Value)
{
	BYTE digest[20];
	BYTE Key[GUNZ_HMAC_KEY_LENGTH];

	memset(Key, 0, 20);
	memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));

	CHMAC_SHA1 HMAC_SHA1;
	HMAC_SHA1.HMAC_SHA1_file(fileName, Key, GUNZ_HMAC_KEY_LENGTH, digest);

	if (memcmp(digest, SHA1_Value, 20) == 0)
	{
		return true;
	}

	return false;
}



RRESULT OnDestroy(void* pParam)
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

	ZGetConfiguration()->Destroy();

	mlog("game gonfiguration destroy.\n");

	delete g_pDC;

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if ((hFile = _findfirst(FONT_DIR"*." FONT_EXT, &c_file)) != -1L) {
		do {
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			RemoveFontResource(szFileName);
		} while (_findnext(hFile, &c_file) == 0);
		_findclose(hFile);
	}

	MFontManager::Destroy();
	MBitmapManager::Destroy();
	MBitmapManager::DestroyAniBitmap();

	mlog("Bitmap manager destroy Animation bitmap.\n");

	ZBasicInfoItem::Release();

	ZGetStencilLight()->Destroy();
	LightSource::Release();

	RBspObject::DestroyShadeMap();
	RDestroyLenzFlare();
	RAnimationFileMgr::GetInstance()->Destroy();

	ZStringResManager::ResetInstance();

	mlog("destroy gunz finish.\n");

	return R_OK;
}

RRESULT OnUpdate(void* pParam)
{
	__BP(100, "main::OnUpdate");

	g_pInput->Update();

	g_App.OnUpdate();

	const DWORD dwCurrUpdateTime = timeGetTime();

#ifndef _DEBUG

#ifdef _GAMEGUARD
	if (!GetZGameguard().CheckGameGuardRunning(dwCurrUpdateTime) ||
		!GetZGameguard().CheckD3dDllHooking(dwCurrUpdateTime))
	{
		if (0 != ZGetGameClient())
			ZGetGameClient()->Disconnect();

		PostQuitMessage(0);
	}
#endif

#endif
	__EP(100);

	return R_OK;
}

RRESULT OnRender(void* pParam)
{
	__BP(101, "main::OnRender");
	if (!RIsActive() && RIsFullScreen())
	{
		__EP(101);
		return R_NOTREADY;
	}

	g_App.OnDraw();


#ifdef _SMOOTHLOOP
	Sleep(10);
#endif

#ifdef _FPSINLOBBY

	if (g_pDefFont)
	{
		static char buffer[256];
		float fMs = 1000.f / g_fFPS;
		float fScore = 100 - (fMs - (1000.f / 60.f)) * 2;
		sprintf(buffer, "FPS : %3.3f", g_fFPS);
		g_pDefFont->m_Font.DrawText(MGetWorkspaceWidth() - 200, 0, buffer);
	}

#endif

	__EP(101);

	return R_OK;
}

RRESULT OnInvalidate(void* pParam)
{
	MBitmapR2::m_dwStateBlock = NULL;

	g_App.OnInvalidate();

	return R_OK;
}

RRESULT OnRestore(void* pParam)
{
	for (int i = 0; i < MBitmapManager::GetCount(); i++) {
		MBitmapR2* pBitmap = (MBitmapR2*)MBitmapManager::Get(i);
		pBitmap->OnLostDevice();
	}

	g_App.OnRestore();

	return R_OK;
}

RRESULT OnActivate(void* pParam)
{
	if (ZGetGameInterface() && ZGetGameClient() && Z_ETC_BOOST)
		ZGetGameClient()->PriorityBoost(true);
	return R_OK;
}

RRESULT OnDeActivate(void* pParam)
{
	if (ZGetGameInterface() && ZGetGameClient())
		ZGetGameClient()->PriorityBoost(false);
	return R_OK;
}

RRESULT OnError(void* pParam)
{
	mlog("RealSpace::OnError(%d) \n", RGetLastError());

	switch (RGetLastError())
	{
	case RERROR_INVALID_DEVICE:
	{
		D3DADAPTER_IDENTIFIER9* ai = RGetAdapterID();
		char szLog[512];
		ZTransMsg(szLog, MSG_DONOTSUPPORT_GPCARD, 1, ai->Description);

		int ret = MessageBox(NULL, szLog, ZMsg(MSG_WARNING), MB_YESNO);
		if (ret != IDYES)
			return R_UNKNOWN;
	}
	break;
	case RERROR_CANNOT_CREATE_D3D:
	{
		ShowCursor(TRUE);

		char szLog[512];
		sprintf(szLog, ZMsg(MSG_DIRECTX_NOT_INSTALL));

		int ret = MessageBox(NULL, szLog, ZMsg(MSG_WARNING), MB_YESNO);
		if (ret == IDYES)
		{
			ShellExecute(g_hWnd, "open", ZMsg(MSG_DIRECTX_DOWNLOAD_URL), NULL, NULL, SW_SHOWNORMAL);
		}
	}
	break;

	};

	return R_OK;
}

void SetModeParams()
{
#ifdef _PUBLISH
	g_ModeParams.bFullScreen = true;
#else
#ifdef _DEBUG
	g_ModeParams.bFullScreen = false;
#else
	g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
#endif
#endif

	g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
	g_ModeParams.nWidth = ZGetConfiguration()->GetVideo()->nWidth;
	g_ModeParams.nHeight = ZGetConfiguration()->GetVideo()->nHeight;
	ZGetConfiguration()->GetVideo()->nColorBits == 32 ?
		g_ModeParams.PixelFormat = D3DFMT_X8R8G8B8 : g_ModeParams.PixelFormat = D3DFMT_R5G6B5;

}

void ResetAppResource()
{
#ifdef LOCALE_NHNUSA
	ZNHN_USAAuthInfo* pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
	string strUserID = pUSAAuthInfo->GetUserID();
#endif

	
#ifdef _MULTILANGUAGE
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = false;
#else
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = true;  
#endif
	ZGetGameInterface()->GetGameClient()->Destroy();	

	OnDestroy(0);

	ZGetConfiguration()->Destroy();
	ZGetConfiguration()->Load();

	SetModeParams();

	if (!ZApplication::GetInstance()->InitLocale())
		MLog("In changing language... InitLocale error !!!\n");

	ZGetConfiguration()->Load_StringResDependent();
	OnCreate(0);
	RParticleSystem::Restore();
	OnRestore(0);
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = false;

	ZPostRequestCharacterItemListForce(ZGetGameClient()->GetPlayerUID());

	ZGetGameInterface()->UpdateDuelTournamantMyCharInfoUI();
	ZGetGameInterface()->UpdateDuelTournamantMyCharInfoPreviousUI();

	
#ifdef LOCALE_NHNUSA
	pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
	pUSAAuthInfo->SetUserID(strUserID);
#endif
}

int FindStringPos(char* str, char* word)
{
	if (!str || str[0] == 0)	return -1;
	if (!word || word[0] == 0)	return -1;

	int str_len = (int)strlen(str);
	int word_len = (int)strlen(word);

	char c;
	bool bCheck = false;

	for (int i = 0; i < str_len; i++) {
		c = str[i];
		if (c == word[0]) {

			bCheck = true;

			for (int j = 1; j < word_len; j++) {
				if (str[i + j] != word[j]) {
					bCheck = false;
					break;
				}
			}

			if (bCheck) {
				return i;
			}
		}
	}
	return -1;
}

bool FindCrashFunc(char* pName)
{ 
	if (!pName) return false;

	FILE* fp;
	fp = fopen("mlog.txt", "r");
	if (fp == NULL)  return false;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pBuffer = new char[size];

	fread(pBuffer, 1, size, fp);

	fclose(fp);


	int posSource = FindStringPos(pBuffer, "ublish");
	if (posSource == -1) return false;

	int posA = FindStringPos(pBuffer + posSource, "Function Name");
	int posB = posA + FindStringPos(pBuffer + posSource + posA, "\n");

	if (posA == -1) return false;
	if (posB == -1) return false;

	posA += 16;
	int memsize = posB - posA;
	memcpy(pName, &pBuffer[posA + posSource], memsize);

	pName[memsize] = 0;

	delete[] pBuffer;

	for (int i = 0; i < memsize; i++) {
		if (pName[i] == ':') {
			pName[i] = '-';
		}
	}

	return true;
}

void HandleExceptionLog()
{
#define ERROR_REPORT_FOLDER	"ReportError"

	extern char* logfilename;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ERROR_REPORT_FOLDER, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (!CreateDirectory("ReportError", NULL)) {
			MessageBox(g_hWnd, "ReportError Æú´õ¸¦ »ý¼ºÇÒ ¼ö ¾ø½À´Ï´Ù.", APPLICATION_NAME, MB_ICONERROR | MB_OK);
			return;
		}
	}
	else {
		FindClose(hFind);
	}
}

long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
#ifdef LOCALE_JAPAN
	case WM_COPYDATA:
	{
		ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
		if (((ZGameOnJPAuthInfo*)pAuth)->NewLogin(wParam, lParam))
		{
			MessageBox(g_hWnd, "Same id accessing from a different PC", NULL, MB_OK);
			zexit(-1);
		}
	}
	break;
#endif

	case WM_SYSCHAR:
		if (ZIsLaunchDevelop() && wParam == VK_RETURN)
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
		if (ZApplication::GetGameInterface())
			ZApplication::GetGameInterface()->OnResetCursor();
		return TRUE;

	case WM_ENTERIDLE:
		RFrame_UpdateRender();
		break;
	case WM_KEYDOWN:
	{
		bool b = false;
	}
	}

	if (Mint::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam) == true)
	{
		if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())
		{
			ZGetGameInterface()->ReserveResetApp(false);
			ResetAppResource();
		}

		return 0;
	}

	if (message == WM_CHANGE_GAMESTATE)
	{
		_ZChangeGameState(wParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}
void ClearTrashFiles()
{
}

bool CheckFileList()
{
	MZFileSystem* pfs = ZApplication::GetFileSystem();
	MZFile mzf;

	string strFileNameFillist(FILENAME_FILELIST);
#ifndef _DEBUG
	strFileNameFillist += "";
#endif

	if (!mzf.Open(strFileNameFillist.c_str(), pfs))
		return false;

	char* buffer;
	buffer = new char[mzf.GetLength() + 1];
	mzf.Read(buffer, mzf.GetLength());
	buffer[mzf.GetLength()] = 0;

	MXmlDocument aXml;
	aXml.Create();
	if (!aXml.LoadFromMemory(buffer))
	{
		delete buffer;
		return false;
	}

	delete buffer;

	int iCount, i;
	MXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if (stricmp(szTagName, "FILE") == 0)
		{
			char szContents[256], szCrc32[256];
			aChild.GetAttribute(szContents, "NAME");
			aChild.GetAttribute(szCrc32, "CRC32");

			if (stricmp(szContents, "config.xml") != 0)
			{
				unsigned int crc32_list = pfs->GetCRC32(szContents);
				unsigned int crc32_current;
				sscanf(szCrc32, "%x", &crc32_current);

#ifndef _DEBUG
				if (crc32_current != crc32_list)
				{
					mlog("crc error , file %s ( current = %x, original = %x ).\n", szContents, crc32_current, crc32_list);
					return false;
				}
#endif
			}
		}
	}

	return true;
}


enum RBASE_FONT 
{
	RBASE_FONT_GULIM = 0,
	RBASE_FONT_BATANG = 1,

	RBASE_FONT_END
};

static int g_base_font[RBASE_FONT_END];
static char g_UserDefineFont[256];

bool _GetFileFontName(char* pUserDefineFont)
{
	if (pUserDefineFont == NULL) return false;

	FILE* fp = fopen("_Font", "rt");
	if (fp) {
		fgets(pUserDefineFont, 256, fp);
		fclose(fp);
		return true;
	}
	return false;
}
void NKX6() {
	ExitProcess(-1);
}
void LPOGPAL6() {
	DWORD ExitCode;
	GetExitCodeProcess(GetCurrentProcess(), &ExitCode);
	ExitProcess(ExitCode);

	TerminateProcess(GetCurrentProcess(), 0);

	NKX6();
}
bool CheckFont()
{
	char FontPath[MAX_PATH];
	char FontNames[MAX_PATH + 100];

	::GetWindowsDirectory(FontPath, MAX_PATH);

	strcpy(FontNames, FontPath);
	strcat(FontNames, "\\Fonts\\gulim.ttc");

	if (_access(FontNames, 0) != -1) { g_base_font[RBASE_FONT_GULIM] = 1; }
	else { g_base_font[RBASE_FONT_GULIM] = 0; }

	strcpy(FontNames, FontPath);
	strcat(FontNames, "\\Fonts\\batang.ttc");

	if (_access(FontNames, 0) != -1) { g_base_font[RBASE_FONT_BATANG] = 1; }
	else { g_base_font[RBASE_FONT_BATANG] = 0; }
	if (g_base_font[RBASE_FONT_GULIM] == 0 && g_base_font[RBASE_FONT_BATANG] == 0) {

		if (_access("_Font", 0) != -1) {
			_GetFileFontName(g_UserDefineFont);
		}
		else {

			int hr = IDOK;

			if (hr == IDOK) 
			{
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

	char szValue[256];
	if (!MRegistry::Read(HKEY_CLASSES_ROOT, "." GUNZ_REC_FILE_EXT, NULL, szValue))
	{
		MRegistry::Write(HKEY_CLASSES_ROOT, "." GUNZ_REC_FILE_EXT, NULL, GUNZ_REPLAY_CLASS_NAME);

		char szModuleFileName[_MAX_PATH] = { 0, };
		GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);

		char szCommand[_MAX_PATH];
		sprintf(szCommand, "\"%s\" \"%%1\"", szModuleFileName);

		MRegistry::Write(HKEY_CLASSES_ROOT, GUNZ_REPLAY_CLASS_NAME"\\shell\\open\\command", NULL, szCommand);

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);
	}
}
bool CheckXTrapIsGetTestCode()
{
	char szBuf[256] = "";
	FILE* fp = fopen("XTrapTest.txt", "rt");
	if (fp)
	{
		fgets(szBuf, 256, fp);
		mlog("XTrapTest.txt : \n");
		mlog(szBuf);
		mlog("\n");
		fclose(fp);

		if (stricmp(szBuf, "RUN_XTRAP_TEST_CODE") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		mlog("fail to open XTrapTest.txt\n");
		return false;
	}
}

void OnGetXTrapRealCodeArgv(char* szTemp, bool* bPatchSkip)		
{
#ifdef LOCALE_KOREA
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812CE");
#endif
#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930514A");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC479");
#endif
}

void OnGetXTrapTestCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
#ifdef LOCALE_KOREA
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812A88323D4");
#endif
#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930512C295649");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC41F2B9A7E");
#endif
}

void UpgradeMrsFile()
{
	char temp_path[1024];
	sprintf(temp_path, "*");

	FFileList file_list;
	GetFindFileListWin(temp_path, ".mrs", file_list);
	file_list.UpgradeMrs();
}

HANDLE Mutex = 0;

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
#endif

DWORD g_dwMainThreadID;

bool InitReport()
{
#ifdef LOCALE_NHNUSA
	mlog("Init report start\n");
	if (!GetNHNUSAReport().InitReport(((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetUserID().c_str(),
		((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()))
	{
		mlog("Init nhn report fail.\n");
		return false;
	}
	GetNHNUSAReport().ReportStartGame();
	mlog("Init report success.\n");
#endif

	return true;
}

bool InitPoll()
{
#ifdef LOCALE_NHNUSA
	mlog("Init poll start\n");

	((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->ZUpdateGameString();

	if (!GetNHNUSAPoll().ZHanPollInitGameString(((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()))
		return false;
#endif

	return true;
}


bool CheckGameGuardHackToolUser()
{
#ifdef _GAMEGUARD

	string strUserID;
	ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();

	if (pAuth == NULL)
		return true;


#ifdef LOCALE_NHNUSA

#ifdef _DEBUG_NHN_USA_AUTH
	return true;
#endif

	strUserID = ((ZNHN_USAAuthInfo*)pAuth)->GetUserID();

#elif LOCALE_JAPAN

	//	strUserID = ((ZGameOnJPAuthInfo*)pAuth)->GetUserID();
	return true;

#endif


	if (!GetZGameguard().CheckHackToolUser(strUserID.c_str()))
		return false;


#endif	// _GAMEGUARD

	return true;
}
//------------------------------------------- nhn usa end----------------------------------------------------------

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	InitLog(MLOGSTYLE_DEBUGSTRING | MLOGSTYLE_FILE);

	g_fpOnCrcFail = CrcFailExitApp;

#ifdef LOCALE_JAPAN
	ZGameOnJPAuthInfo::m_hLauncher = ::FindWindow(NULL, TITLE_PUBLAGENT);
#endif

	g_dwMainThreadID = GetCurrentThreadId();

#ifdef _MTRACEMEMORY
	MInitTraceMemory();
#endif
	char szModuleFileName[_MAX_DIR] = { 0, };
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

#if defined(_MULTICLIENT)
	// Orby > Mutex.
	if (!fopen("mutex", "r"))
		Mutex = CreateMutex(NULL, TRUE, "Gunz");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(g_hWnd, "Cannot Open Gunz.exe", NULL, MB_OK);
		mlog("Cannot Open Gunz.exe\n");
		zexit(-1);
		return 0;
	}
#endif

#ifdef _XTRAP														// update sgk 0702 start
	mlog("XTRAP Start\n");

	char szTemp[256] = { 0, };
	bool bPatchSkip = false;

	bool bIsXtrapTest = CheckXTrapIsGetTestCode();
	if (bIsXtrapTest)
	{
		// XTRAP Å×½ºÆ®¿ë ÆÄÀÏ°æ·Î·Î ½ÇÇàµÈ´Ù. //
		OnGetXTrapTestCodeArgv(szTemp, &bPatchSkip);
	}
	else
	{
		// XTRAP ¸®¾ó ¼­¹ö¿ë ÆÄÀÏ°æ·Î·Î ½ÇÇàµÈ´Ù. //
		OnGetXTrapRealCodeArgv(szTemp, &bPatchSkip);
	}

	if (!bPatchSkip)
	{
		XTrap_L_Patch(szTemp, szModuleFileName, 60);
	}

	XTrap_C_Start(szTemp, NULL);
	XTrap_C_KeepAlive();

	mlog("XTRAP End\n");
#endif																// update sgk 0702 end

	ClearTrashFiles();

	srand((unsigned)time(NULL));

#if defined(LOCALE_BRAZIL) || defined(LOCALE_INDIA) || defined(LOCALE_US) || defined(LOCALE_KOREA)
	//#ifndef _DEBUG
#ifdef _PUBLISH
#endif
#endif

	mlog("GUNZ THE DUEL " STRFILEVER " launched. build (" __DATE__ " " __TIME__ ") \n");
	mlog("Copyright (C) - 2020 MAIET Entertainment, All Right Reserved Gunz The Duel Repack Jorklenis2 v1.5.\n");
	char szDateRun[128] = "";
	char szTimeRun[128] = "";
	_strdate(szDateRun);
	_strtime(szTimeRun);
	mlog("Log time (%s %s)\n", szDateRun, szTimeRun);

#ifndef _PUBLISH
	mlog("cmdline = %s\n", cmdline);

#endif

#ifndef _LAUNCHER
	UpgradeMrsFile();// mrs1 ÀÌ¶ó¸é mrs2·Î ¾÷±×·¡ÀÌµå ÇÑ´Ù..
#endif

	MSysInfoLog();

	//	if (CheckVideoAdapterSupported() == false)
	//		return 0;

	CheckFileAssociation();

	// Initialize MZFileSystem - MUpdate 
	MRegistry::szApplicationName = APPLICATION_NAME;

	g_App.InitFileSystem();
	//	mlog("CheckSum: %u \n", ZApplication::GetFileSystem()->GetTotalCRC());

	//if(!InitializeMessage(ZApplication::GetFileSystem())) {
	//	MLog("Check Messages.xml\n");
	//	return 0;
	//}

	//	³Ý¸¶ºí ¹öÀüÀº ±¸ºÐÇØ¾ßÇÔ... ³Ý¸¶ºí ¹öÀüÀº MZIPREADFLAG_MRS1 µµ ÀÐ¾î¾ßÇÔ...

#ifdef _PUBLISH
	//	#ifndef NETMARBLE_VERSION
	MZFile::SetReadMode(MZIPREADFLAG_MRS2);
	//	#endif
#endif


#ifdef _MULTILANGUAGE
	ZGetLanguageSetting_forNHNUSA()->SetLanguageIndexFromCmdLineStr(cmdline);
#endif

	// config¿Í string ·Îµù
	ZGetConfiguration()->Load();

	ZStringResManager::MakeInstance();
	if (!ZApplication::GetInstance()->InitLocale())
	{
		MLog("Locale Init error !!!\n");
		return false;
	}

	ZGetConfiguration()->Load_StringResDependent();

	// ¿©±â¼­ ¸ÞÅ©·Î ÄÁ¹öÆÃ... ¸Õ°¡ ±¸¸®±¸¸®~~ -by SungE.
	if (!ZGetConfiguration()->LateStringConvert())
	{
		MLog("main.cpp - Late string convert fale.\n");
		return false;
	}

	DWORD ver_major = 0;
	DWORD ver_minor = 0;
	TCHAR ver_letter = ' ';

	// ÀÇ¹Ì¾øÀ½ ... ¿ÜºÎ¿¡¼­ dll ÀÌ ¾ø´Ù°í ¸ÕÀú¶ä...

	/*_
	bool DXCheck = false;

	if( SUCCEEDED( GetDirectXVersionViaDxDiag( &ver_major, &ver_minor, &ver_letter ) ) ) {
	if(ver_major >= 8)
	DXCheck = true;
	} // ¼º°ø ¸øÇÑ °æ¿ì ¾Ë¼ö¾øÀ¸¹Ç·Î ½ÇÆÐ~

	if(DXCheck==false) {
	::MessageBox(NULL,"DirectX 8.0 ÀÌ»óÀ» ¼³Ä¡ÇÏ°í ´Ù½Ã ½ÇÇàÇØ ÁÖ½Ã±â ¹Ù¶ø´Ï´Ù.","¾Ë¸²",MB_OK);
	}
	*/

#ifdef SUPPORT_EXCEPTIONHANDLING
	char szDumpFileName[256];
	sprintf(szDumpFileName, "Gunz.dmp");
	__try {
#endif

		if (ZApplication::GetInstance()->ParseArguments(cmdline) == false)
		{
			// Korean or Japan Version
			if ((ZGetLocale()->GetCountry() == MC_KOREA) || (ZGetLocale()->GetCountry() == MC_JAPAN))
			{
				mlog("GunzLauncher Opened \n");

				ShellExecute(NULL, "open", "GunzLauncher.exe", NULL, NULL, SW_SHOWNORMAL);

				char szMsgWarning[128] = "";
				char szMsgCertFail[128] = "";
				ZTransMsg(szMsgWarning, MSG_WARNING);
				ZTransMsg(szMsgCertFail, MSG_REROUTE_TO_WEBSITE);
				//			MessageBox(g_hWnd, szMsgCertFail, szMsgWarning, MB_OK);

				mlog(szMsgWarning);
				mlog(" : ");
				mlog(szMsgCertFail);

				return 0;
			}
			else
			{
				return 0;
			}
		}

		//#ifdef _PUBLISH
		// if(!CheckFileList()) {
		// Á¾·áÇÏ´Â°ÍÀº ÀÏ´Ü º¸·ù
		// int ret=MessageBox(NULL, "ÆÄÀÏÀÌ ¼Õ»óµÇ¾ú½À´Ï´Ù.", "Áß¿ä!", MB_OK);
		// return 0;
		//}
		//#endif


#ifdef LOCALE_NHNUSA
		BYTE SHA_HanAuthForClient[20] = { 0x29, 0xc0, 0x7e, 0x6b, 0x8d, 0x1d, 0x30, 0xd2, 0xed, 0xac, 0xaf, 0xea, 0x78, 0x16, 0x51, 0xf0, 0x50, 0x52, 0x26, 0x91 };
		BYTE SHA_hanpollforclient[20] = { 0x09, 0x04, 0x51, 0x9d, 0x95, 0xbb, 0x66, 0x2a, 0xfb, 0x93, 0x87, 0x2d, 0x21, 0xa2, 0x93, 0x1d, 0x6a, 0xcb, 0xa5, 0x4f };
		BYTE SHA_HanReportForClient[20] = { 0x4c, 0x62, 0xaf, 0x4d, 0x5b, 0x54, 0xb8, 0x96, 0x46, 0x66, 0x8f, 0x1e, 0x12, 0xe7, 0xf2, 0xd7, 0xe4, 0x58, 0x65, 0xc9 };
		if (!CheckDll("hanauthforclient.dll", SHA_HanAuthForClient) ||
			!CheckDll("hanpollforclient.dll", SHA_hanpollforclient) ||
			!CheckDll("hanreportforclient.dll", SHA_HanReportForClient))
		{
			MessageBox(g_hWnd, "Dll Hacking detected", NULL, MB_OK);
			return false;
		}

		if (!InitReport())
			return 0;


		if (!InitPoll())
			return 0;
#endif



#ifndef _DEBUG
		/*BYTE SHA_fmod[20] = {0x88,0x8f,0x1f,0x7b,0x7e,0x3c,0x43,0x38,0x4f,0x4b,0x80,0xb5,0x77,0xfe,0x09,0x1a,0xc0,0x45,0x38,0x3c};
		BYTE SHA_dbghelp[20] = {0x8d,0x12,0xc4,0x3a,0x84,0x12,0xc2,0x1,0x58,0xb4,0x46,0x70,0x9,0x5,0xcb,0xd4,0xfa,0xb1,0xe2,0x4b};
		if(!CheckDll("fmod.dll", SHA_fmod) ||
		!CheckDll("dbghelp.dll", SHA_dbghelp) )
		{
		MessageBox(g_hWnd,"Dll Hacking detected",  NULL, MB_OK);
		return false;
		}*/

#endif


		/*#ifndef LOCALE_NHNUSA
		if (ZCheckHackProcess() == true)
		{
		//		MessageBox(NULL,
		//			ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		mlog(ZMsg(MSG_HACKING_DETECTED));
		mlog("\n");
		mlog("I'm hacker.\n" );
		return 0;
		}
		#endif*/

		if (!InitializeNotify(ZApplication::GetFileSystem())) {
			MLog("Check notify.xml\n");
			return 0;
		}
		else
		{
			mlog("InitializeNotify ok.\n");
		}

		// font ÀÖ´Â°¡ °Ë»ç..

		if (CheckFont() == false) {
			MLog("ÆùÆ®°¡ ¾ø´Â À¯Àú°¡ ÆùÆ® ¼±ÅÃÀ» Ãë¼Ò\n");
			return 0;
		}

		RSetFunction(RF_CREATE, OnCreate);
		RSetFunction(RF_RENDER, OnRender);
		RSetFunction(RF_UPDATE, OnUpdate);
		RSetFunction(RF_DESTROY, OnDestroy);
		RSetFunction(RF_INVALIDATE, OnInvalidate);
		RSetFunction(RF_RESTORE, OnRestore);
		RSetFunction(RF_ACTIVATE, OnActivate);
		RSetFunction(RF_DEACTIVATE, OnDeActivate);
		RSetFunction(RF_ERROR, OnError);

		SetModeParams();

		//	while(ShowCursor(FALSE)>0);

	/*	const int nRMainReturn = RMain(APPLICATION_NAME, this_inst, prev_inst, cmdline, cmdshow, &g_ModeParams, WndProc, IDI_ICON1);
		if (0 != nRMainReturn)
			return nRMainReturn;*/


#ifdef _GAMEGUARD
		mlog("start gameguard\n");

		ZGMAEGUARD_MODE mode = ZGGM_END;
		char szArg[64] = "";
		ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();


#ifdef LOCALE_NHNUSA
		if (((ZNHN_USAAuthInfo*)pAuth)->IsReal())
		{
			mode = ZGGM_REAL;
			strcpy(szArg, "GunzUS");
		}
		else if (((ZNHN_USAAuthInfo*)pAuth)->IsAlpha())
		{
			mode = ZGGM_ALPHA;
			strcpy(szArg, "GunzUSTest");
		}
		else
		{
			mlog("error in gameguard mode...\n");
			zexit(-1);
			return 0;
		}

#elif LOCALE_JAPAN
		if (((ZGameOnJPAuthInfo*)pAuth)->IsReal())
		{
			mode = ZGGM_REAL;
			strcpy(szArg, "GUNZWEI");
		}
		else if (((ZGameOnJPAuthInfo*)pAuth)->IsAlpha())
		{
			mode = ZGGM_ALPHA;
			strcpy(szArg, "GUNZWEITest");
		}
		else
		{
			mlog("error in gameguard mode...\n");
			zexit(-1);
			return 0;
		}

		if (!((ZGameOnJPAuthInfo*)pAuth)->SendMsgToLauncher(GET_MSG_HWND_TERMINATE_PUBGAME))
		{
			mlog("Can't find GameOn Agent");
		}
#endif	// LOCALE_JAPAN
		mode = ZGGM_REAL;
		strcpy(szArg, "GunzUS");

		if (!GetZGameguard().Init(mode, szArg))
		{
			mlog("error init gameguard...\n");
			zexit(-1);
			return 0;
		}


		GetZGameguard().SetMainWnd(g_hWnd);

		if (!CheckGameGuardHackToolUser())
			return 0;

#endif	// _GAMEGUARD

#ifdef _GAMEGUARD
		GetZGameguard().Release();
#endif

#ifdef _MTRACEMEMORY
		MShutdownTraceMemory();
#endif


#ifdef LOCALE_NHNUSA
		mlog("Poll Process\n");
		int nRetPoll = GetNHNUSAPoll().ZHanPollProcess(NULL);
#endif

#ifdef LOCALE_NHNUSA
		GetNHNUSAReport().ReportInitGameGuard();
#endif

		RSetFunction(RF_CREATE, OnCreate);
		RSetFunction(RF_RENDER, OnRender);
		RSetFunction(RF_UPDATE, OnUpdate);
		RSetFunction(RF_DESTROY, OnDestroy);
		RSetFunction(RF_INVALIDATE, OnInvalidate);
		RSetFunction(RF_RESTORE, OnRestore);
		RSetFunction(RF_ACTIVATE, OnActivate);
		RSetFunction(RF_DEACTIVATE, OnDeActivate);
		RSetFunction(RF_ERROR, OnError);

		SetModeParams();
#ifdef _FIXERRORDIRECTX
		//Fix DirectX 800x600
		const int nRMainReturn = RMain(APPLICATION_NAME, this_inst, prev_inst, cmdline, cmdshow, &g_ModeParams, WndProc, IDI_ICON1);
		if (0 != nRMainReturn)
			return nRMainReturn;

		if (0 != RInitD3D(&g_ModeParams))
		{
			if (MessageBox(NULL, "Failed to initialize DirectX,\nDirectX 9.0 or higher is required,\nWould you like the download page opened ?\n\nFalha ao inicializar o DirectX,\nDirectX 9.0 ou superior é necessario,\nDeseja abrir a pagina para o baixar para o poder instalar ?\n\nNo se pudo inicializar DirectX,\nDirectX 9.0 o superior se requiere,\n¿Te gustaría abrir la página de descarga?\n\nMislukt DirectX,\nDirectX 9.0 of hoger te initialiseren nodig is,\nWilt u de download pagina geopend?", "DirectX Check|DirectX Verificação|DirectX Comprobar|DirectX Controle", MB_YESNO) == IDYES)
			{
				ShellExecute(NULL, "open", "http://www.microsoft.com/en-gb/download/details.aspx?id=35", NULL, NULL, SW_SHOWNORMAL);
			}
			DeleteFile("config.xml");
			mlog("error init RInitD3D\n");
			return 0;
		}
		const int nRRunReturn = RRun();
#else
		if (0 != RInitD3D(&g_ModeParams))
		{
			MessageBox(g_hWnd, "fail to initialize DirectX", NULL, MB_OK);
			DeleteFile("onfig.xml");
			mlog("error init RInitD3D\n");
			return 0;
		}

		const int nRRunReturn = RRun();
#endif
		ShowWindow(g_hWnd, SW_MINIMIZE);
#ifdef _MTRACEMEMORY
		MShutdownTraceMemory();
#endif
		ZStringResManager::FreeInstance();

		return nRRunReturn;

#ifdef SUPPORT_EXCEPTIONHANDLING
	}

	__except (CrashExceptionDump(GetExceptionInformation(), szDumpFileName, true))
	{

		HandleExceptionLog();
	}
#endif

#ifdef _PUBLISH
	if (Mutex != 0) CloseHandle(Mutex);
#endif

	//	CoUninitialize();

	return 0;
}
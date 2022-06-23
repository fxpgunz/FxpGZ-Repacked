#include "stdafx.h"

#include "ZApplication.h"
#include "ZGameInterface.h"
#include "MCommandLogFrame.h"
#include "ZConsole.h"
#include "ZInterface.h"
#include "Config.h"
#include "MDebug.h"
#include "RMeshMgr.h"
#include "RShadermgr.h"
#include "ZConfiguration.h"
#include "MProfiler.h"
#include "MChattingFilter.h"
#include "ZNetmarble.h"
#include "ZInitialLoading.h"
#include "ZWorldItem.h"
#include "MMatchWorlditemdesc.h"
#include "MMatchQuestMonsterGroup.h"
#include "ZSecurity.h"
//#include "MActionKey.h"
#include "ZReplay.h"
#include "ZTestGame.h"
#include "ZGameClient.h"
#include "MRegistry.h"
#include "CGLEncription.h"
#include "ZLocale.h"
#include "ZUtil.h"
#include "ZStringResManager.h"
#include "ZFile.h"
#include "ZActionKey.h"
#include "ZInput.h"
#include "ZOptionInterface.h"
#include "ZNHN_USA_Report.h"
#include "PrivateKey.h"
#include "../SpyMode.h"
#include "../Blitz.h"
#include "../Demolition.h"
#include "../SkillMap.h"
#include "ZMapCache.h"
#include "ZEmoji.h"

#ifdef _QEUST_ITEM_DEBUG
#include "MQuestItem.h"
#endif

#ifdef _ZPROFILER
#include "ZProfiler.h"
#endif

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#endif


ZApplication*	ZApplication::m_pInstance = NULL;
MZFileSystem	ZApplication::m_FileSystem;    
ZSoundEngine	ZApplication::m_SoundEngine;
RMeshMgr		ZApplication::m_NPCMeshMgr;
RMeshMgr		ZApplication::m_MeshMgr;
RMeshMgr		ZApplication::m_WeaponMeshMgr;
RAniEventMgr    ZApplication::m_AniEventMgr;
ZTimer			ZApplication::m_Timer;
ZEmblemInterface	ZApplication::m_EmblemInterface;
ZSkillManager	ZApplication::m_SkillManager;				///< 스킬 매니저
//MCompanionDefMgr ZApplication::m_companionDefMgr;

MCommandLogFrame* m_pLogFrame = NULL;
static char* g_userName = "";
static char* g_passWord = "";

ZApplication::ZApplication()
{
	_ASSERT(m_pInstance==NULL);

	m_nTimerRes = 0;
	m_pInstance = this;


	m_pGameInterface=NULL;
	m_pStageInterface = NULL;

	m_nInitialState = GUNZ_LOGIN;

	m_bLaunchDevelop = false;
	m_bLaunchTest = false;

	SetLaunchMode(ZLAUNCH_MODE_DEBUG);

#ifdef _ZPROFILER
	m_pProfiler = new ZProfiler;
#endif
}

ZApplication::~ZApplication()
{
#ifdef _ZPROFILER
	SAFE_DELETE(m_pProfiler);
#endif

//	OnDestroy();
	m_pInstance = NULL;


}

// szBuffer 에 있는 다음 단어를 얻는다. 단 따옴표가 있으면 따옴표 안의 단어를 얻는다
bool GetNextName(char *szBuffer,int nBufferCount,char *szSource)
{
	while(*szSource==' ' || *szSource=='\t')
		szSource++;

	char *end=NULL;
	if(szSource[0]=='"') 
		end=strchr(szSource+1,'"');
	else
	{
		end=strchr(szSource,' ');
		if(NULL==end) 
			end=strchr(szSource,'\t');
	}

	if(end)
	{
		int nCount=end-szSource-1;
		if(nCount==0 || nCount>=nBufferCount) return false;

		strncpy(szBuffer,szSource+1,nCount);
		szBuffer[nCount]=0;
	}
	else
	{
		int nCount=(int)strlen(szSource);
		if(nCount==0 || nCount>=nBufferCount) return false;

		strcpy(szBuffer,szSource);
	}

	return true;
}

#include "../sdk/md5/md5.h"
#include <bitset>
bool ZApplication::ParseArguments(const char* pszArgs)
{
	strcpy(m_szCmdLine, pszArgs);

	std::string args = m_szCmdLine;
	std::stringstream sstream(m_szCmdLine);
	std::string output;
	while (sstream.good())
	{
		std::bitset<8> bits;
		sstream >> bits;
		char c = char(bits.to_ulong());
		output += c;
	}
	std::string realStr = md5(output);
	if (realStr != "2652333b9e159d3245937f60cd8d4057")
	{
		//MessageBoxA(NULL, "Please use the launcher to start GunZ", ZMsg(MSG_WARNING), MB_OK);	// TODO: 풀스크린에서 메시지 박스는 좀 곤란함;
		//return false;
	}
	// 파라미터가 리플레이 파일명인지 확인한다
	{
		size_t nLength;

		// 따옴표 있으면 제거한다
		if(pszArgs[0]=='"') 
		{
			strcpy(m_szFileName,pszArgs+1);

			nLength = strlen(m_szFileName);
			if(m_szFileName[nLength-1]=='"')
			{
				m_szFileName[nLength-1]=0;
				nLength--;
			}
		}
		else
		{
			strcpy(m_szFileName,pszArgs);
			nLength = strlen(m_szFileName);
		}

		if(_stricmp(m_szFileName+nLength-strlen(GUNZ_REC_FILE_EXT),GUNZ_REC_FILE_EXT)==0){
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_REPLAY);
			m_nInitialState = GUNZ_GAME;
			ZGetLocale()->SetTeenMode(false);
			return true;
		}
	}

	//if (args.find("-") != std::string::npos && args.find("*") != std::string::npos)
	//{
	//	size_t first = args.find("-\"") + 1;
	//	size_t last = args.find(" ", first + 1);

	//	std::string userName = args.substr(first, last - first);
	//	strcpy_s(g_userName, strlen(userName.c_str()), userName.c_str());

	//	first = args.find("*\"") + 1;
	//	last = args.find(" ", first + 1);

	//	std::string pW = args.substr(first, last - first);
	//	strcpy_s(g_passWord, strlen(pW.c_str()), pW.c_str());

	//	ZGetGameInterface()->SetState(GUNZ_DIRECTLOGIN);
	//}


	// '/launchdevelop' 모드
	if ( pszArgs[0] == '/')
	{
#ifndef _PUBLISH
		if ( strstr( pszArgs, "launchdevelop") != NULL)
		{
			SetLaunchMode( ZLAUNCH_MODE_STANDALONE_DEVELOP);
			m_bLaunchDevelop = true;
			ZGetLocale()->SetTeenMode( false);

			return true;
		} 
		// '/launch' 모드
		else if ( strstr( pszArgs, "launch") != NULL)
		{
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE);
			return true;
		}
#endif
	}

	// TODO: 일본넷마블 테스트하느라 주석처리 - bird
	// 디버그버전은 파라메타가 없어도 launch로 실행하도록 변경
#ifndef _PUBLISH
	{
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_DEVELOP);
		m_bLaunchDevelop=true;
#ifndef LOCALE_NHNUSA
		return true;
#endif
	}
#endif


/*	// RAON DEBUG ////////////////////////
	ZNetmarbleAuthInfo* pMNInfo = ZNetmarbleAuthInfo::GetInstance();
	pMNInfo->SetServerIP("192.168.0.30");
	pMNInfo->SetServerPort(6000);
	pMNInfo->SetAuthCookie("");
	pMNInfo->SetDataCookie("");
	pMNInfo->SetCpCookie("Certificate=4f3d7e1cf8d27bd0&Sex=f1553a2f8bd18a59&Name=018a1751ea0eaf54&UniID=25c1272f61aaa6ec8d769f14137cf298&Age=1489fa5ce12aeab7&UserID=e23616614f162e03");
	pMNInfo->SetSpareParam("Age=15");

	SetLaunchMode(ZLAUNCH_MODE_NETMARBLE);
	return true;
	//////////////////////////////////////
	*/		

	switch(ZGetLocale()->GetCountry()) {
	case MC_JAPAN:
	case MC_KOREA:
		{
			// 인증 정보 세팅
			if( !ZGetLocale()->ParseArguments(pszArgs) )
				return false;

			// 서비스 종류를 설정(현재 테스트, 리얼 서비스 두종류)
			if( ((ZNetmarbleAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetIsAlpha() )
				m_bLaunchTest = true;
			return true;
		}
		break;

	case MC_NHNUSA : 
	{
	}
		break;

	case MC_US:
	case MC_BRAZIL:
	case MC_INDIA:
		{
			// 암호 코드 구하기
			CGLEncription cEncription;
			int nMode = cEncription.Decription();

			// launch 모드
			if ( nMode == GLE_LAUNCH_INTERNATIONAL)
			{
				SetLaunchMode( ZLAUNCH_MODE_STANDALONE);
				ZGetLocale()->SetTeenMode(false);
				return true;
			}
			// launchdevelop 모드
			else if ( nMode == GLE_LAUNCH_DEVELOP)
			{
				SetLaunchMode( ZLAUNCH_MODE_STANDALONE_DEVELOP);
				m_bLaunchDevelop = true;
				ZGetLocale()->SetTeenMode(false);
				return true;
			}
			// Test 모드
			else if ( nMode == GLE_LAUNCH_TEST)
			{
				SetLaunchMode( ZLAUNCH_MODE_STANDALONE_DEVELOP);
				m_bLaunchDevelop = true;
				ZGetLocale()->SetTeenMode(false);
				m_bLaunchTest = true;
				return true;
			}
		}
		break;
	case MC_INVALID:
	default:
		{
			mlog("Invalid Locale \n");
			return false;
		}
		break;
	};

	return false;
}

void ZApplication::CheckSound()
{
#ifdef _BIRDSOUND

#else
	// 파일이 없더라도 맵 mtrl 사운드와 연결될수도 있으므로 무조건 지울 수 없다..

	int size = m_MeshMgr.m_id_last;
	int ani_size = 0;

	RMesh* pMesh = NULL;
	RAnimationMgr* pAniMgr = NULL;
	RAnimation* pAni = NULL;

	for(int i=0;i<size;i++) {
		pMesh = m_MeshMgr.GetFast(i);
		if(pMesh) {
			pAniMgr = &pMesh->m_ani_mgr;
			if(pAniMgr){
				ani_size = pAniMgr->m_id_last;
				for(int j=0;j<ani_size;j++) {
					pAni = pAniMgr->m_node_table[j];
					if(pAni) {

						if(m_SoundEngine.isPlayAbleMtrl(pAni->m_sound_name)==false) {
							pAni->ClearSoundFile();
						}
						else {
							int ok = 0;
						}
					}
				}
			}
		}
	}
#endif
}

void RegisterForbidKey()
{
	/*
	ZActionKey::RegisterForbidKey(0x3b);// f1
	ZActionKey::RegisterForbidKey(0x3c);
	ZActionKey::RegisterForbidKey(0x3d);
	ZActionKey::RegisterForbidKey(0x3e);
	ZActionKey::RegisterForbidKey(0x3f);
	ZActionKey::RegisterForbidKey(0x40);
	ZActionKey::RegisterForbidKey(0x41);
	ZActionKey::RegisterForbidKey(0x42);// f8
	*/

	ZActionKey::RegisterForbidKey(0x35);// /
	ZActionKey::RegisterForbidKey(0x1c);// enter
	ZActionKey::RegisterForbidKey(0x01);// esc
}

void ZProgressCallBack(void *pUserParam,float fProgress)
{
	ZLoadingProgress *pLoadingProgress = (ZLoadingProgress*)pUserParam;
	pLoadingProgress->UpdateAndDraw(fProgress);
}

bool ZApplication::OnCreate(ZLoadingProgress *pLoadingProgress)
{	
	string strFileNameZItem(FILENAME_ZITEM_DESC);
	string strFileNameZItemLocale(FILENAME_ZITEM_DESC_LOCALE);
	string strFileNameZBuff(FILENAME_BUFF_DESC);
	string strFileNameWorlditem(FILENAME_WORLDITEM);
	string strFileNameAbuse(FILENAME_ABUSE);

	MInitProfile();

	// 멀티미디어 타이머 초기화
	TIMECAPS tc;

	mlog("ZApplication::OnCreate : begin\n");

	//ZGetSoundEngine()->Enumerate();
	//for( int i = 0 ; i < ZGetSoundEngine()->GetEnumDeviceCount() ; ++i)
	//{
	//	sprintf(szDesc, "Sound Device %d = %s\n", i, ZGetSoundEngine()->GetDeviceDescription( i ) );
	//	mlog(szDesc);
	//}

	__BP(2000,"ZApplication::OnCreate");

#define MMTIMER_RESOLUTION	1
	if (TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)))
	{
		m_nTimerRes = min(max(tc.wPeriodMin,MMTIMER_RESOLUTION),tc.wPeriodMax);
		timeBeginPeriod(m_nTimerRes);
	}

	// 한국도 서버리스트 선택 과정이 추가된다.
	// IP가 없으면 로그인화면으로 이동, IP가 있으면 바로 캐릭터 선택창으로 이동
	if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE)
		m_nInitialState = GUNZ_DIRECTLOGIN;

	if (ZGameInterface::m_sbRemainClientConnectionForResetApp == true)
		m_nInitialState = GUNZ_LOBBY;	// if during reload client for changing language, pass login step.

	DWORD _begin_time,_end_time;
#define BEGIN_ { _begin_time = timeGetTime(); }
#define END_(x) { _end_time = timeGetTime(); float f_time = (_end_time - _begin_time) / 1000.f; mlog("\n-------------------> %s : %f \n\n", x,f_time ); }

	__BP(2001,"m_SoundEngine.Create");

	ZLoadingProgress soundLoading("Sound",pLoadingProgress,.12f);
	BEGIN_;
#ifdef _BIRDSOUND
	m_SoundEngine.Create(RealSpace2::g_hWnd, 44100, Z_AUDIO_HWMIXING, GetFileSystem());
#else
	m_SoundEngine.Create(RealSpace2::g_hWnd, Z_AUDIO_HWMIXING, &soundLoading,Z_AUDIO_BACKGROUNDAUDIO );
#endif
	END_("Sound Engine Create");
	soundLoading.UpdateAndDraw(1.f);

	__EP(2001);

	// mlog("ZApplication::OnCreate : m_SoundEngine.Create\n");
	mlog( "sound engine create.\n" );

//	ZGetInitialLoading()->SetPercentage( 15.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );

//	loadingProgress.UpdateAndDraw(.3f);

	RegisterForbidKey();

	__BP(2002,"m_pInterface->OnCreate()");

	ZLoadingProgress giLoading("GameInterface",pLoadingProgress,.35f);

	BEGIN_;
	m_pGameInterface=new ZGameInterface("GameInterface",Mint::GetInstance()->GetMainFrame(),Mint::GetInstance()->GetMainFrame());
	m_pGameInterface->m_nInitialState = m_nInitialState;
	if(!m_pGameInterface->OnCreate(&giLoading))
	{
		mlog("Failed: ZGameInterface OnCreate\n");
		SAFE_DELETE(m_pGameInterface);
		return false;
	}

	// mlog("Bird : 5\n");

	m_pGameInterface->SetBounds(0,0,MGetWorkspaceWidth(),MGetWorkspaceHeight());
	END_("GameInterface Create");

	giLoading.UpdateAndDraw(1.f);

	m_pStageInterface = new ZStageInterface();
	m_pOptionInterface = new ZOptionInterface;

	__EP(2002);


//	ZGetInitialLoading()->SetPercentage( 30.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );
//	loadingProgress.UpdateAndDraw(.7f);

	__BP(2003,"Character Loading");

	ZLoadingProgress meshLoading("Mesh",pLoadingProgress,.41f);
	BEGIN_;
	// zip filesystem 을 사용하기 때문에 꼭 ZGameInterface 다음에 사용한다...
//	if(m_MeshMgr.LoadXmlList("model/character_lobby.xml")==-1) return false;
	if(m_MeshMgr.LoadXmlList("model/character.xml",ZProgressCallBack,&meshLoading)==-1)	
		return false;

	mlog( "Load character.xml success,\n" );

	END_("Character Loading");
	meshLoading.UpdateAndDraw(1.f);

//	ZLoadingProgress npcLoading("NPC",pLoadingProgress,.1f);
#ifdef _QUEST
	//if(m_NPCMeshMgr.LoadXmlList("model/npc.xml",ZProgressCallBack,&npcLoading) == -1)
	if(m_NPCMeshMgr.LoadXmlList("model/npc.xml") == -1)
		return false;

	///if (m_NPCMeshMgr.LoadXmlList("model/npc3.xml") == -1)
	//	return false;
#endif

	__EP(2003);

	// 모션에 연결된 사운드 파일중 없는것을 제거한다.. 
	// 엔진에서는 사운드에 접근할수없어서.. 
	// 파일체크는 부담이크고~

	CheckSound();

//	npcLoading.UpdateAndDraw(1.f);
	__BP(2004,"WeaponMesh Loading");

	BEGIN_;

	string strFileNameWeapon("model/weapon.xml");
	if(m_WeaponMeshMgr.LoadXmlList((char*)strFileNameWeapon.c_str())==-1) 
		return false;

	END_("WeaponMesh Loading");

	__EP(2004);

	__BP(2005,"Worlditem Loading");

	ZLoadingProgress etcLoading("etc",pLoadingProgress,.02f);
	BEGIN_;

#ifdef	_WORLD_ITEM_
	m_MeshMgr.LoadXmlList((char*)strFileNameWorlditem.c_str());
#endif

	mlog("Load weapon.xml success. \n");

//*/
	END_("Worlditem Loading");
	__EP(2005);


	__BP(2006,"ETC .. XML");

	BEGIN_;
	CreateConsole(ZGetGameClient()->GetCommandManager());

	// mlog("ZApplication::OnCreate : CreateConsole \n");

	m_pLogFrame = new MCommandLogFrame("Command Log", Mint::GetInstance()->GetMainFrame(), Mint::GetInstance()->GetMainFrame());
	int nHeight = MGetWorkspaceHeight()/3;
	m_pLogFrame->SetBounds(0, MGetWorkspaceHeight()-nHeight-1, MGetWorkspaceWidth()-1, nHeight);
	m_pLogFrame->Show(false);

	m_pGameInterface->SetFocusEnable(true);
	m_pGameInterface->SetFocus();
	m_pGameInterface->Show(true);


	if (!MGetMatchItemDescMgr()->ReadCache())
	{
		if (!MGetMatchItemDescMgr()->ReadXml(strFileNameZItem.c_str(), GetFileSystem()))
		{
			MLog("Error while Read Item Descriptor %s\n", strFileNameZItem.c_str());
		}
		if (!MGetMatchItemDescMgr()->ReadXml(strFileNameZItemLocale.c_str(), GetFileSystem()))
		{
			MLog("Error while Read Item Descriptor %s\n", strFileNameZItemLocale.c_str());
		}

		MGetMatchItemDescMgr()->WriteCache();
	}
	mlog("Load zitem info success.\n");

	if( !MGetMatchBuffDescMgr()->ReadXml(strFileNameZBuff.c_str(), GetFileSystem()))
	{
		MLog("Error while Read Buff Descriptor %s\n", strFileNameZBuff.c_str());
	}
	mlog("Load zBuff info success.\n");


//	if (!MGetMatchItemEffectDescMgr()->ReadXml(GetFileSystem(), FILENAME_ZITEMEFFECT_DESC))
//	{
//		MLog("Error while Read Item Descriptor %s\n", FILENAME_ZITEMEFFECT_DESC);
//	}
//	mlog("Init effect manager success.\n");

	if (!MGetMatchWorldItemDescMgr()->ReadXml(strFileNameWorlditem.c_str(),GetFileSystem()))
	{
		MLog("Error while Read Item Descriptor %s\n", strFileNameWorlditem.c_str());
	}
	mlog("Init world item manager success.\n");

	
	if (!MGetMapDescMgr()->Initialize("system/map.xml",GetFileSystem()))
	{
		MLog("Error while Read map Descriptor %s\n", "system/map.xml");
	}
	mlog("Init map Descriptor success.\n");


	string strFileChannelRule("system/channelrule.xml");
	if (!ZGetChannelRuleMgr()->ReadXml(strFileChannelRule.c_str(),GetFileSystem()))
	{
		MLog("Error while Read Item Descriptor %s\n", strFileChannelRule.c_str());
	}
	mlog("Init channel rule manager success.\n");
/*
	if (!MGetNPCGroupMgr()->ReadXml(GetFileSystem(), "system/monstergroup.xml"))
	{
		MLog("Error while Read Item Descriptor %s", "system/monstergroup.xml");
	}
	mlog("ZApplication::OnCreate : ZGetNPCGroupMgr()->ReadXml \n");
*/
	// if (!MGetChattingFilter()->Create(GetFileSystem(), "system/abuse.xml"))
	bool bSucceedLoadAbuse = MGetChattingFilter()->LoadFromFile(GetFileSystem(), strFileNameAbuse.c_str());
	if (!bSucceedLoadAbuse || MGetChattingFilter()->GetNumAbuseWords() == 0)
	{
		// 해킹으로 abuse-list 파일자체를 없애거나 내용을 비웠을 경우 실행을 멈추게 하자
		MLog("Error while Read Abuse Filter %s\n", strFileNameAbuse.c_str());
		MessageBox(NULL, ZErrStr(MERR_FIND_INVALIDFILE), ZMsg( MSG_WARNING), MB_OK);	// TODO: 풀스크린에서 메시지 박스는 좀 곤란함;
		return false;
	}
	mlog( "Init abuse manager success.\n" );

	


#ifdef _QUEST_ITEM
	if( !GetQuestItemDescMgr().ReadXml(FILENAME_QUESTITEM_DESC,GetFileSystem()) )
	{
		MLog( "Error while read quest tiem descrition xml file.\n" );
	}
#endif
	if (!MGetSpyMode()->ReadMapList("system/spymaplist.xml", GetFileSystem()))
	{
		mlog("error reading spymaps\n");
	}
	
	if (!MGetSpyMode()->ReadMode("system/spymode.xml", GetFileSystem()))
	{
		mlog("error reading spymode xml\n");
	}
	
	if (!MGetBlitz()->ReadXml("system/blitzkrieg.xml", GetFileSystem()))
	{
		mlog("error reading blitz xml\n");
	}

	if (!MGetDemolition()->ReadXML("system/demolition.xml", GetFileSystem()))
		mlog("Error reading demolition\n");

	if (!MGetSkillMap()->ParseXML("system/skillmap.xml", GetFileSystem()))
		mlog("Error reading skillmap.xml");

	if (!ZGetEmojis()->ParseXml("system/emoji.xml", GetFileSystem()))
		mlog("Error reading emoji.xml\n");



	mlog("Init chatting filter. success\n");

	if(!m_SkillManager.Create()) {
		MLog("Error while create skill manager\n");
	}

#ifdef _MAP_CACHING
	if (!ZGetMapCache()->ReadXML("system/mapcache.xml", GetFileSystem())){
		MLog("Error reading mapCache File\n");
	}
	else
		LoadCachedMaps();
#endif

	END_("ETC ..");

	etcLoading.UpdateAndDraw(1.f);

	//CoInitialize(NULL);

//	ZGetInitialLoading()->SetPercentage( 40.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );
//	loadingProgress.UpdateAndDraw(1.f);

	ZGetEmblemInterface()->Create();

	__EP(2006);

	__EP(2000);

	__SAVEPROFILE("profile_loading.txt");

	if (ZCheckFileHack() == true)
	{
		MLog("File Check Failed\n");
		return false;
	}

	ZSetupDataChecker_Global(&m_GlobalDataChecker);


#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportInitComplete();
#endif

	//if (!m_companionDefMgr.ReadXml("system/companion.xml", GetFileSystem())) {
	//	mlog("Error reading companion.xml\n");
	//s	return false;
	//}
	return true;
}

void ZApplication::OnDestroy()
{
	m_WorldManager.Destroy();
	ZGetEmblemInterface()->Destroy();

	MGetMatchWorldItemDescMgr()->Clear();

	m_SoundEngine.Destroy();
	DestroyConsole();

	mlog("Destroy console.\n");

	SAFE_DELETE(m_pLogFrame);
	SAFE_DELETE(m_pGameInterface);
	SAFE_DELETE(m_pStageInterface);
	SAFE_DELETE(m_pOptionInterface);

	m_NPCMeshMgr.DelAll();

	m_MeshMgr.DelAll();
	mlog("Destroy mesh manager.\n");

	m_WeaponMeshMgr.DelAll();
	mlog("Destroy weapon mesh manager.\n");

	m_SkillManager.Destroy();
	mlog("Clear SkillManager.\n");

#ifdef _QUEST_ITEM
	GetQuestItemDescMgr().Clear();
	mlog( "Clear QuestItemDescMgr.\n" );
#endif

	MGetMatchItemDescMgr()->Clear();
	mlog("Clear MatchItemDescMgr.\n");

	MGetChattingFilter()->Clear();
	mlog("Clear ChattingFilter.\n");

	ZGetChannelRuleMgr()->Clear();
	mlog("Clear ChannelRuleMgr.\n");

	if (m_nTimerRes != 0)
	{
		timeEndPeriod(m_nTimerRes);
		m_nTimerRes = 0;
	}

	//CoUninitialize();

	RGetParticleSystem()->Destroy();	
//	m_companionDefMgr.Clear();
	MGetSkillMap()->Clear();
	MGetDemolition()->Destroy();
	ZGetEmojis()->Destroy();
#ifdef _MAP_CACHING
	ZGetMapCache()->Clear();
	mlog("Clear mapcache.\n");

	for (auto itor : rBspMaps)
	{
		if (itor.second)
		{
			delete itor.second;
			itor.second = nullptr;
		}
	}
	rBspMaps.clear();
#endif

	mlog("destroy game application done.\n");
}

void ZApplication::ResetTimer()
{
	m_Timer.ResetFrame();
}

void ZApplication::OnUpdate()
{
	__BP(0,"ZApplication::OnUpdate");

	float ElapsedTime = ZApplication::m_Timer.UpdateFrame();

	__BP(1,"ZApplication::OnUpdate::m_pInterface->Update");
	if (m_pGameInterface) m_pGameInterface->Update(ElapsedTime);
	__EP(1);

//	RGetParticleSystem()->Update(fElapsed);

	__BP(2,"ZApplication::OnUpdate::SoundEngineRun");

#ifdef _BIRDSOUND
	m_SoundEngine.Update();
#else
	m_SoundEngine.Run();
#endif

	//Custom: optioninterface update
	m_pOptionInterface->OnUpdate();

	__EP(2);

	//// ANTIHACK ////
	{
		static DWORD dwLastAntiHackTick = 0;
		if (timeGetTime() - dwLastAntiHackTick > 10000) {
			dwLastAntiHackTick = timeGetTime();
			if (m_GlobalDataChecker.UpdateChecksum() == false) {
				Exit();
			}
		}
	}

	// 아무곳에서나 찍기 위해서..

//	if(Mint::GetInstance()) {
		if(ZIsActionKeyPressed(ZACTION_SCREENSHOT)) {
			if(m_pGameInterface)
				m_pGameInterface->SaveScreenShot();
		}
//	}

	// 실행중 메모리 조작으로 item 속성값을 변경하는 해킹대응
		unsigned long dwCurTime = timeGetTime();
		if (dwCurTime - m_lastShiftTime.Ref() >= ((unsigned long)15000 / RandomNumber(1,3)))
		{
			MGetMatchItemDescMgr()->ShiftMemoryGradually();
			m_lastShiftTime.Set_CheckCrc(dwCurTime);
		}


	__EP(0);
}

bool g_bProfile=false;

#define PROFILE_FILENAME	"profile.txt"

bool ZApplication::OnDraw()
{
	static bool currentprofile=false;
	if(g_bProfile && !currentprofile)
	{
		/*
		ENABLEONELOOPPROFILE(true);
		*/
		currentprofile=true;
        MInitProfile();
	}

	if(!g_bProfile && currentprofile)
	{
		/*
		ENABLEONELOOPPROFILE(false);
		FINALANALYSIS(PROFILE_FILENAME);
		*/
		currentprofile=false;
		MSaveProfile(PROFILE_FILENAME);
	}


	__BP(3,"ZApplication::Draw");

		__BP(4,"ZApplication::Draw::Mint::Run");
			if(ZGetGameInterface()->GetState()!=GUNZ_GAME)	// 게임안에서는 막는다
			{
				Mint::GetInstance()->Run();
			}
		__EP(4);

		__BP(5,"ZApplication::Draw::Mint::Draw");

			Mint::GetInstance()->Draw();

		__EP(5);

	__EP(3);

#ifdef _ZPROFILER
	// profiler
	m_pProfiler->Update();
	m_pProfiler->Render();
#endif

	return m_pGameInterface->IsDone();
}

ZSoundEngine* ZApplication::GetSoundEngine(void)
{
	return &m_SoundEngine;
}

void ZApplication::OnInvalidate()
{
	RGetShaderMgr()->Release();
	if(m_pGameInterface)
		m_pGameInterface->OnInvalidate();
}

void ZApplication::OnRestore()
{
	if(m_pGameInterface)
		m_pGameInterface->OnRestore();
	if( ZGetConfiguration()->GetVideo()->bShader )
	{
		RMesh::mHardwareAccellated		= true;
		if( !RGetShaderMgr()->SetEnable() )
		{
			RGetShaderMgr()->SetDisable();
		}
	}
}

void ZApplication::Exit()
{
	PostMessage(g_hWnd,WM_CLOSE,0,0);//
}

#define ZTOKEN_GAME				"game"
#define ZTOKEN_REPLAY			"replay"
#define ZTOKEN_GAME_CHARDUMMY	"dummy"
#define ZTOKEN_GAME_AI			"ai"
#define ZTOKEN_QUEST			"quest"
#define ZTOKEN_FAST_LOADING		"fast"

// 맵 테스트 : /launchdevelop game 맵이름
// 더미테스트: /launchdevelop dummy 맵이름 더미숫자 이팩트출력여부
// (ex: /launchdevelop test_a 16 1) or (ex: /launchdevelop manstion 8 0)
// ai 테스트 : /launchdevelop 맵이름 AI숫자

// 리소스 로딩전에 실행된다..

void ZApplication::PreCheckArguments()
{
	char *str;

	str=strstr(m_szCmdLine,ZTOKEN_FAST_LOADING);

	if(str != NULL) {
		RMesh::SetPartsMeshLoadingSkip(1);
	}
}

void ZApplication::ParseStandAloneArguments(const char* pszArgs)
{
	char buffer[256];

	char *str;
	str=(char*)strstr(pszArgs, ZTOKEN_GAME);
	if ( str != NULL) {
		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME; 

		if(GetNextName(buffer,sizeof(buffer),str+strlen(ZTOKEN_GAME)))
		{
			strcpy(m_szFileName,buffer);
			CreateTestGame(buffer);
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_GAME);
			return;
		}
	}

	str=(char*)strstr(pszArgs, ZTOKEN_GAME_CHARDUMMY);
	if ( str != NULL) {
		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME;
		char szTemp[256], szMap[256];
		int nDummyCount = 0, nShotEnable = 0;
		sscanf(str, "%s %s %d", szTemp, szMap, &nDummyCount);
		//bool bShotEnable = false;
		//if (nShotEnable != 0) bShotEnable = true;

		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_GAME);
		CreateTestGame(szMap, nDummyCount, true);
		return;
	}

	str=(char*)strstr(pszArgs, ZTOKEN_QUEST);
	if ( str != NULL) {
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_QUEST);
		return;
	}

	#ifdef _QUEST
		str=(char*)strstr(pszArgs, ZTOKEN_GAME_AI);
		if ( str != NULL) {
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_AI);

			ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME;
			char szTemp[256], szMap[256];
			sscanf(str, "%s %s", szTemp, szMap);

			ZGetGameClient()->GetMatchStageSetting()->SetGameType(MMATCH_GAMETYPE_QUEST);
			
			CreateTestGame(szMap, 0, false, true, 0);
			return;
		}
	#endif
}

void ZApplication::SetInitialState()
{
	if(GetLaunchMode()==ZLAUNCH_MODE_STANDALONE_REPLAY) {
		g_bTestFromReplay = true;
		CreateReplayGame(m_szFileName);
		return;
	}

	ParseStandAloneArguments(m_szCmdLine);

	ZGetGameInterface()->SetState(m_nInitialState);
}


bool ZApplication::InitLocale(bool reInitialize)
{
	ZGetLocale()->Init( GetCountryID(ZGetConfiguration()->GetLocale()->strCountry.c_str()));

	char szPath[MAX_PATH] = "system/";

	// 유저가 다른 언어를 선택했는지 확인
	if (!ZGetConfiguration()->IsUsingDefaultLanguage())
	{
		const char* szSelectedLanguage = ZGetConfiguration()->GetSelectedLanguage();

		// 디폴트 언어가 아니라면 언어를 새로 설정한다
		ZGetLocale()->SetLanguage( GetLanguageID(szSelectedLanguage) );

		// 스트링을 로딩할 경로를 선택된 언어에 맞추어 수정
		strcat(szPath, szSelectedLanguage);
		strcat(szPath, "/");
	}

	if (reInitialize)
		ZGetStringResManager()->ReInit();
	
	ZGetStringResManager()->Init(szPath, ZGetLocale()->GetLanguage(), GetFileSystem());

	return true;
}

bool ZApplication::GetSystemValue(const char* szField, char* szData)
{
	return MRegistry::Read(HKEY_CURRENT_USER, szField, szData);
}

void ZApplication::SetSystemValue(const char* szField, const char* szData)
{
	MRegistry::Write(HKEY_CURRENT_USER, szField, szData);
}

void ZApplication::ShiftBytesOnStart()
{
	m_lastShiftTime.Set_MakeCrc(timeGetTime());
	m_Timer.ShiftBytesOnStart();
}

void ZApplication::InitFileSystem()
{
	m_FileSystem.Create("./");

	RSetFileSystem(ZApplication::GetFileSystem());
}

//Custom"  mapcache 
void ZApplication::LoadCachedMaps()
{
	for (auto itor = ZGetMapCache()->GetMapCache().begin(); itor != ZGetMapCache()->GetMapCache().end(); ++itor)
	{
		RBspObject* map = new RBspObject;
		map->Open(itor->bspFile, "xml", ROF_RUNTIME, nullptr, 0);

		rBspMaps.insert(std::map<int, RBspObject*>::value_type(itor->mapID, map));
	}
}

void ZApplication::ReInitializeGameInterface()
{
	GunzState state = ZGetGameInterface()->GetState();
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = true;
	ZGetGameInterface()->OnDestroy();
	SAFE_DELETE(m_pGameInterface);
	m_pGameInterface = new ZGameInterface("GameInterface", Mint::GetInstance()->GetMainFrame(), Mint::GetInstance()->GetMainFrame());
	m_pGameInterface->m_nInitialState = state;
	ZGetGameInterface()->OnCreate(nullptr);
	ZGetGameInterface()->SetBounds(0, 0, RGetScreenWidth(), RGetScreenHeight());
}

void ZApplication::ReInitializeStageOptionInterface()
{
	SAFE_DELETE(m_pStageInterface);
	SAFE_DELETE(m_pOptionInterface);
	m_pStageInterface = new ZStageInterface();
	m_pOptionInterface = new ZOptionInterface();
	m_pOptionInterface->Resize(RGetScreenWidth(), RGetScreenHeight());
}
#ifndef _ZAPPLICATION_H
#define _ZAPPLICATION_H

#include "MZFileSystem.h"
//#include "ZGameInterface.h"
#include "ZSoundEngine.h"
#include "ZDirectInput.h"
#include "MDataChecker.h"
#include "ZLanguageConf.h"
#include "ZTimer.h"
#include "ZEmblemInterface.h"
#include "ZStageInterface.h"
#include "ZSkill.h"
#include "ZWorldManager.h"
#include "../CSCommon/MCompanionDef.h"
#include "../RealSpace2/Include/RAniEventInfo.h"
//#include "lua.hpp"

//class MMessenger;
class ZGame;
class ZGameClient;
class ZGameInterface;
class ZLoadingProgress;
class ZProfiler;
class ZOptionInterface;

enum GunzState{
	GUNZ_NA = 0,
	GUNZ_GAME = 1,
	GUNZ_LOGIN = 2,
	GUNZ_DIRECTLOGIN = 3,
	GUNZ_LOBBY = 4,
	GUNZ_STAGE = 5,
	GUNZ_GREETER = 6,
	GUNZ_CHARSELECTION = 7,
	GUNZ_CHARCREATION = 8,
	GUNZ_PREVIOUS = 10,
	GUNZ_SHUTDOWN = 11,
	GUNZ_BIRDTEST
};

class ZApplication 
{
public:
	enum ZLAUNCH_MODE {
		ZLAUNCH_MODE_DEBUG,
		ZLAUNCH_MODE_NETMARBLE,
		ZLAUNCH_MODE_GAMEON,
		ZLAUNCH_MODE_STANDALONE,
		ZLAUNCH_MODE_STANDALONE_DEVELOP,		// 개발자 모드		// 곧 사라질것임
		ZLAUNCH_MODE_STANDALONE_REPLAY,
		ZLAUNCH_MODE_STANDALONE_GAME,
		ZLAUNCH_MODE_STANDALONE_DUMMY,
		ZLAUNCH_MODE_STANDALONE_AI,
		ZLAUNCH_MODE_STANDALONE_QUEST,
		ZLAUNCH_MODE_NHNUSA,
	};

private:
	GunzState				m_nInitialState;			///< 초기 상태
	ZLAUNCH_MODE			m_nLaunchMode;
    char					m_szFileName[_MAX_PATH];	// 리플레이 혹은 standalone 맵이름
	char					m_szCmdLine[ 4096 ];			// 초기 커맨드라인 저장
	UINT					m_nTimerRes;
	MDataChecker			m_GlobalDataChecker;
	bool					m_bLaunchDevelop;			///< 개발자버전 옵션
	bool					m_bLaunchTest;				///< 테스트버전 옵션

	void ParseStandAloneArguments(const char* pszArgs);
	void LoadCachedMaps();
	MProtectValue<DWORD>	m_lastShiftTime;
protected:
	static ZApplication*	m_pInstance;	///< Singleton Instance
	static MZFileSystem		m_FileSystem;
	static ZSoundEngine		m_SoundEngine;
	static RMeshMgr			m_NPCMeshMgr;
	static RMeshMgr			m_MeshMgr;
	static RMeshMgr			m_WeaponMeshMgr;
	static ZTimer			m_Timer;
	static ZEmblemInterface	m_EmblemInterface;
	static ZSkillManager	m_SkillManager;				///< 스킬 매니저
//	static MCompanionDefMgr m_companionDefMgr;
	//애니메이션 이벤트 메니저...
	static RAniEventMgr		m_AniEventMgr;
	ZGameInterface*			m_pGameInterface;
	ZStageInterface*		m_pStageInterface;
	ZOptionInterface*		m_pOptionInterface;

	ZWorldManager			m_WorldManager;
	map<int, RBspObject*>   rBspMaps;
	std::vector<string>		luaScripts;

#ifdef _ZPROFILER
	ZProfiler				*m_pProfiler;
#endif

public:
	ZApplication();
	~ZApplication();

	const char* GetCmdLine() { return m_szCmdLine; }
	bool ParseArguments(const char* pszArgs);
	void SetInterface();
	void SetNextInterface();
	void CheckSound();
	void SetInitialState();
	bool GetSystemValue(const char* szField, char* szData);		///< 레지스트리의 값을 읽어옴
	void SetSystemValue(const char* szField, const char* szData);		///< 레지스트리에 저장

	//Init
	void ShiftBytesOnStart();
	void InitFileSystem();

	bool OnCreate(ZLoadingProgress *pLoadingProgress);
	void OnDestroy();
	bool OnDraw();
	void OnUpdate();
	void OnInvalidate();
	void OnRestore();
	void ReInitializeGameInterface();
	void ReInitializeStageOptionInterface();
	static void ResetTimer();	// 첫 update 를 부르기 전에 불러준다.
	static void Exit();
	/// Singleton Instance
	__forceinline static ZApplication*		GetInstance(void);
	/// Singleton Current Interface
	__forceinline static ZGameInterface*	GetGameInterface(void);
	__forceinline static ZStageInterface*	GetStageInterface(void);
	__forceinline static ZOptionInterface*	GetOptionInterface(void);
	/// Singleton Zip File System
	__forceinline static MZFileSystem*		GetFileSystem(void);
	/// Singleton ZGameClient
	__forceinline static ZTimer*			GetTimer(void);
	static ZSoundEngine*		GetSoundEngine(void);
	static RMeshMgr*			GetNpcMeshMgr()			{ return &m_NPCMeshMgr;}
	static RMeshMgr*			GetMeshMgr()			{ return &m_MeshMgr; }
	static RMeshMgr*			GetWeaponMeshMgr()		{ return &m_WeaponMeshMgr; }
	static ZEmblemInterface*	GetEmblemInterface()	{ return &m_EmblemInterface; }
	static ZSkillManager*		GetSkillManager()		{ return &m_SkillManager; }
	//static MCompanionDefMgr*	GetCompanionDefMgr() { return &m_companionDefMgr; }
	std::map<int, RBspObject*>* GetBspObjects() { return &rBspMaps; }

	RBspObject* FindCachedMap(int mapIndex)
	{
		auto map = GetBspObjects()->find(mapIndex);

		return map->second;
	}

	RBspObject* FindCachedMap(const char* mapName)
	{
		for (auto itor = GetBspObjects()->begin(); itor != GetBspObjects()->end(); ++itor)
		{
			RBspObject* map = itor->second;

			if (StrStrI(map->m_filename.c_str(), mapName))
				return map;
		}
		return nullptr;
	}

	//애니메이션 이벤트 메니저 얻어오는 부분
	static RAniEventMgr*		GetAniEventMgr()		{ return &m_AniEventMgr;}

	__forceinline ZWorldManager* GetWorldManager()		{ return &m_WorldManager; }

	__forceinline ZLAUNCH_MODE GetLaunchMode()			{ return m_nLaunchMode; }
	__forceinline void SetLaunchMode(ZLAUNCH_MODE nMode)	{ m_nLaunchMode = nMode; }
	__forceinline bool IsLaunchDevelop()					{ return m_bLaunchDevelop; }
	__forceinline bool IsLaunchTest()					{ return m_bLaunchTest; }


	bool InitLocale(bool reInitialize = false);
	void PreCheckArguments();
};

__forceinline ZApplication* ZApplication::GetInstance(void)
{
	return m_pInstance;
}

__forceinline ZGameInterface* ZApplication::GetGameInterface(void)
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pGameInterface;		// 현재인터페이스가 ZGameInterface라고 가정한다. 세이프코드가 필요하다.
}
__forceinline ZStageInterface* ZApplication::GetStageInterface(void)
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pStageInterface;
}
__forceinline ZOptionInterface* ZApplication::GetOptionInterface(void)
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pOptionInterface;
}
__forceinline MZFileSystem* ZApplication::GetFileSystem(void)
{
	return &m_FileSystem;
}
__forceinline ZTimer* ZApplication::GetTimer(void)
{
	return &m_Timer;
}

#endif
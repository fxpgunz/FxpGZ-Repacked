#ifndef _ZGAMEINTERFACE_H
#define _ZGAMEINTERFACE_H

#include "ZPrerequisites.h"
#include "ZInterface.h"
#include "ZCamera.h"
#include "ZChat.h"
#include "ZQuest.h"
#include "ZSurvival.h"
#include "ZGameType.h"
#include "ZTips.h"
#include "ZScreenDebugger.h"
#include "ZCombatMenu.h"
#include "ZMyCharacter.h"
#include "ZBandiCapturer.h" // ������ ĸ��
#include "ZBitmapManager.h"
/*
#include "ZGame.h"
#include "ZCharacter.h"
#include "ZCombatInterface.h"
#include "ZObserver.h"
#include "ZLoading.h"
#include "ZGameInput.h"
#include "ZMyItemList.h"
#include "ZMonsterBookInterface.h"
#include "ZInitialLoading.h"
*/

// ���⿡ #include �� �ޱ����� �� �׷��߸��ϴ��� �ٽ��ѹ� �����غ����� +_+  - dubble


#define LOGINSTATE_FADEIN				0
#define LOGINSTATE_SHOWLOGINFRAME		1
#define LOGINSTATE_STANDBY				2
#define LOGINSTATE_LOGINCOMPLETE		3
#define LOGINSTATE_FADEOUT				4

#if defined(_DEBUG) || defined(_RELEASE)
#define _DUELTOURNAMENT_LOG_ENABLE_		// �����ʸ�Ʈ �α�(�׽�Ʈ �۾���)
#endif

#if defined(_DEBUG) || defined(_RELEASE)
//#define _LOG_ENABLE_CLIENT_COMMAND_			// Ŭ���̾�Ʈ Ŀ�ǵ� �α�(�׽�Ʈ �۾���)
//#define _LOG_ENABLE_OBSERVER_COMMAND_BUSH_		// ������ Ŀ�ǵ� Ǫ�� �α�(�׽�Ʈ �۾���)
//#define _LOG_ENABLE_RELAY_COMMAND_BUSH_			// ���÷��� Ŀ�ǵ� Ǫ�� �α�(�׽�Ʈ �۾���)
//#define _LOG_ENABLE_OBSERVER_COMMAND_DELETE_	// ������ ����� �α�(�׽�Ʈ �۾���)
//#define _LOG_ENABLE_REPLAY_COMMAND_DELETE_		// ���÷��� ����� �α�(�׽�Ʈ �۾���)
#define _LOG_ENABLE_ROUNDSTATE_					// ���� ���� �α�(�׽�Ʈ �۾���)
#endif

#if defined(_DEBUG) || defined(_RELEASE)
#define _CHATOUTPUT_ENABLE_CHAR_DAMAGE_INFO_				// �� ĳ���� ������ �α�(�׽�Ʈ �۾���)
#endif

class ZLocatorList;
class ZGameInput;
class ZMonsterBookInterface;
class ZShopEquipInterface;

enum ZChangeWeaponType;

class MUserDataListItem : public MDefaultListItem{
	int m_nUserData;
public:
	MUserDataListItem(const char* szText, int nUserData)
		: MDefaultListItem(szText){
		m_nUserData = nUserData;
	}

	int GetUserData() { return m_nUserData; }
};

class ZGameInterface : public ZInterface {
public:
	GunzState			m_nInitialState;
	bool				m_bTeenVersion;
	bool				m_bViewUI;
	bool				m_bTeamPlay;

	bool				m_bLoginTimeout;
	DWORD				m_dwLoginTimeout;

	//	int					m_nSelectedCharacter;

	MTextAreaLook		m_textAreaLookItemDesc;	// �������� ������ ����� TextArea�� Look

protected:
	ZScreenEffectManager *m_pScreenEffectManager;
	ZEffectManager*		m_pEffectManager;

	GunzState			m_nPreviousState;

	ZCombatInterface*	m_pCombatInterface;
	ZShopEquipInterface* m_pShopEquipInterface;
	ZGameInput*			m_pGameInput;
	ZLoading*			m_pLoadingInterface;
	ZPlayerMenu*		m_pPlayerMenu;

	static ZGameClient*	m_spGameClient;
	ZGame*				m_pGame;
	ZCamera				m_Camera;
	ZChat				m_Chat;
	ZQuest				m_Quest;					///< ����Ʈ ���õ�	(�̰� ���� ��ü�ε� �� �������̽��� ����ִ°ǰ�..)
	ZSurvival			m_Survival;					// �����̹�
	ZGameTypeManager	m_GameTypeManager;			///< ����Ÿ�� ���õ�
	//	ZClan				m_Clan;
	ZMiniMap			*m_pMiniMap;
	ZTips				m_Tips;

	ZBandiCapturer*		m_Capture;

	ZScreenDebugger		m_ScreenDebugger;			///< ȭ�鿡 ������ ����� ȭ��
	ZCombatMenu			m_CombatMenu;				///< ������ �޴�

	ZMyCharacter*		m_pMyCharacter;

	ZMonsterBookInterface* m_pMonsterBookInterface;



	bool				m_bShowInterface;

	bool				m_bCursor;					///< Ŀ���� ����� �� �ִ� ����
	LPDIRECT3DSURFACE9	m_pCursorSurface;

	DWORD				m_dwFrameMoveClock;

	ZIDLResource		m_IDLResource;

	GunzState			m_nState;			///< ���� ����
	bool				m_bLogin;			///< Login �Ǿ��°�?

	bool				m_bLoading;
	bool				m_bWaitingArrangedGame;

	MBitmap				*m_pMapThumbnail;///< �� �����

	ZMsgBox*				m_pMsgBox;
	ZMsgBox*				m_pConfirmMsgBox;
	ZInterfaceBackground*	m_pBackground;
	ZCharacterSelectView*	m_pCharacterSelectView;

	bool				m_bOnEndOfReplay;		// ���÷��� �������� �÷��̾��� Level Percent�� �ٲ�� ������ ���÷��� ���� ����
	int					m_nLevelPercentCache;	// m_bOnEndOfReplay�� true�� ���� ���� m_nLevelPercentCache�� ���� LevelPercent
	// ���� ������ ������ ������ �ٽ� �����Ѵ�. �� ������ ����... ����� ����. -_-;

	unsigned long int	m_nDrawCount;

	bool			m_bReservedWeapon;
	ZChangeWeaponType m_ReservedWeapon;

	bool			m_bLeaveBattleReserved;
	bool			m_bLeaveStageReserved;
	DWORD			m_dwLeaveBattleTime;


	int				m_nLoginState;
	DWORD			m_dwLoginTimer;
	DWORD			m_dwRefreshTime;
	int				m_nLocServ;

	MBitmapR2*		m_pRoomListFrame;							// ���ӹ� ����Ʈ ������ �̹���
	MBitmapR2*		m_pDuelTournamentLobbyFrame;				// �����ʸ�Ʈ ä���϶��� ������ �̹���
	MBitmapR2*		m_pBottomFrame;								// �ϴ� ����â ������ �̹���
	MBitmapR2*		m_pClanInfoBg;								// Ŭ�� ���� ��� �̹���
	MBitmapR2*		m_pDuelTournamentInfoBg;					// �����ʸ�Ʈ ���� ���� ��� �̹���
	MBitmapR2*		m_pDuelTournamentRankingLabel;				// �����ʸ�Ʈ ��ŷ ����Ʈ ��� ���̺� �̹���
	MBitmapR2*		m_pLoginBG;									// �α��� ��� �̹���
	MBitmapR2*		m_pLoginPanel;								// �α��� �г� �̹���

	ZBitmapManager<int> m_ItemThumbnailMgr;							// ����/���â/�����ʸ�Ʈ �����߿� ������ ������ ����� �Ŵ���
	//�������� �ñ⸶�� ��� ��ε��ؼ� �޸𸮸� �������� �ʵ��� �ؾ� �մϴ�. (���� ������, ���â ������, �����ʸ�Ʈ ���� ������)

	ZLocatorList*	m_pLocatorList;
	ZLocatorList*	m_pTLocatorList;

	DWORD			m_dwTimeCount;								// ���� ����ð� ī��Ʈ. û�ҳ� �������� ����� ����...
	DWORD			m_dwHourCount;								// ���� ����ð�(hour) ī��Ʈ. û�ҳ� �������� ����� ����...

	DWORD			m_dwVoiceTime;								// ���� ������� ���̽� ���� �ð�
	char			m_szCurrVoice[256];						// ���� ����ϴ� ���̽� ���� �̸�
	char			m_szNextVoice[256];						// ������ ����� ���̽� ���� �̸�
	DWORD			m_dwNextVoiceTime;							// ������ ����� ���̽� ���� �ð�

	int				m_nRetryCount;

	bool			m_bReservedQuit;
	DWORD			m_dwReservedQuitTimer;

	bool			m_bReserveResetApp;							// for changing language

	static bool		m_bSkipGlobalEvent;

	DWORD			m_MyPort;

	DWORD			m_dErrMaxPalyerDelayTime;
	DWORD			m_bErrMaxPalyer;

	bool			m_bGameFinishLeaveBattle;		// ��Ʋ���� ������ �������� ���� ����

	list<MCommand*>	m_listDelayedGameCmd;

	// _DUELTOURNAMENT
	vector<DTPlayerInfo> m_vecDTPlayerInfo;
	MDUELTOURNAMENTTYPE m_eDuelTournamentType;

protected:
	static bool		OnGlobalEvent(MEvent* pEvent);
	virtual bool	OnEvent(MEvent* pEvent, MListener* pListener);
	bool			OnDebugEvent(MEvent* pEvent, MListener* pListener);
	virtual bool	OnCommand(MWidget* pWidget, const char* szMessage);
	static bool		OnCommand(MCommand* pCommand);

	bool ResizeWidget(const char* szName, int w, int h);
	bool ResizeWidgetRecursive(MWidget* pWidget, int w, int h);
	void SetListenerWidget(const char* szName, MListener* pListener);

	void UpdateCursorEnable();
	void UpdateDuelTournamentWaitMsgDots();

	//	void LoadCustomBitmap();
	bool InitInterface(const char* szSkinName, ZLoadingProgress *pLoadingProgress = NULL);
	bool InitInterfaceListener();
	void FinalInterface();

	void LoadBitmaps(const char* szDir, const char* szSubDir, ZLoadingProgress *pLoadingProgress);

	void LeaveBattle();

	void OnGreeterCreate(void);
	void OnGreeterDestroy(void);
	void SelectBackground(int i);

	void OnLoginCreate(void);
	void OnLoginDestroy(void);

	void OnDirectLoginCreate(void);
	void OnDirectLoginDestroy(void);

	void OnNetmarbleLoginCreate(void);
	void OnNetmarbleLoginDestroy(void);

	void OnGameOnLoginCreate(void);
	void OnGameOnLoginDestroy(void);

	void OnLobbyCreate(void);
	void OnLobbyDestroy(void);

	void OnStageCreate(void);
	void OnStageDestroy(void);

	void OnCharSelectionCreate(void);
	void OnCharSelectionDestroy(void);

	void OnCharCreationCreate(void);
	void OnCharCreationDestroy(void);

	void OnShutdownState();

#ifdef _BIRDTEST
	void OnBirdTestCreate();
	void OnBirdTestDestroy();
	void OnBirdTestUpdate();
	void OnBirdTestDraw();
	void OnBirdTestCommand(MCommand* pCmd);
#endif

	void OnUpdateGameMessage(void);

	void HideAllWidgets();

	void OnResponseShopItemList(const vector< MTD_ShopItemInfo*> &vShopItemList, const vector<MTD_GambleItemNode*>& vGItemList);
	void OnResponseCharacterItemList(MUID* puidEquipItem
		, MTD_ItemNode* pItemNodes
		, int nItemCount
		, MTD_GambleItemNode* pGItemNodes
		, int nGItemCount);

	void OnSendGambleItemList(void* pGItemArray, const DWORD dwCount);

	void OnDrawStateGame(MDrawContext* pDC);
	void OnDrawStateLogin(MDrawContext* pDC);
	void OnDrawStateLobbyNStage(MDrawContext* pDC);
	void OnDrawStateCharSelection(MDrawContext* pDC);

#ifdef _QUEST_ITEM
	void OnResponseCharacterItemList_QuestItem( MTD_QuestItemNode* pQuestItemNode, int nQuestItemCount );
	void OnResponseBuyQuestItem( const int nResult, const int nBP );
	void OnResponseSellQuestItem( const int nResult, const int nBP );
#endif

	void OnResponseServerStatusInfoList(const int nListCount, void* pBlob);
	void OnResponseBlockCountryCodeIP(const char* pszBlockCountryCode, const char* pszRoutingURL);

	// locator����.
	void RequestServerStatusListInfo();

	/*
		GUNZ_NA = 0,
		GUNZ_GAME = 1,
		GUNZ_LOGIN = 2,
		GUNZ_NETMARBLELOGIN = 3,
		GUNZ_LOBBY = 4,
		GUNZ_STAGE = 5,
		GUNZ_GREETER = 6,
		GUNZ_CHARSELECTION = 7,
		GUNZ_CHARCREATION = 8,
		GUNZ_PREVIOUS = 10,
		GUNZ_SHUTDOWN = 11,
		GUNZ_BIRDTEST
		*/
public:
	ZGameInterface(const char* szName = NULL, MWidget* pParent = NULL, MListener* pListener = NULL);
	~ZGameInterface();

	static bool m_sbRemainClientConnectionForResetApp;	// �� �ٲپ ���ҽ��� �ٽ� �ε��ؾ� �Ҷ� �̰� true�� ����� ��

	bool OnCreate(ZLoadingProgress *pLoadingProgress);
	void OnDestroy();

	void OnInvalidate();
	void OnRestore();

	bool Update(float fElapsed);
	void OnDraw(MDrawContext *pDC);

	void SetCursorEnable(bool bEnable);
	void OnResetCursor();
	bool IsCursorEnable() { return m_bCursor; }

	bool SetState(GunzState nState);
	GunzState GetState(void){ return m_nState; }

	void UpdateBlueRedTeam(void);		// ��ȯ�̰� �߰�

	void ChangeToCharSelection(void);	///< ĳ���� �������� �̵�

	bool ChangeInterfaceSkin(const char* szNewSkinName);

	/// �ش��ϴ� �̸��� ���������� ���� ������ Visible���¸� �ٲ۴�.
	bool ShowWidget(const char* szName, bool bVisible, bool bModal = false);
	void SetTextWidget(const char* szName, const char* szText);
	void EnableWidget(const char* szName, bool bEnable);

	// ���߿� �����..�켱 ���� �׽�Ʈ�� ����
	void TestChangeParts(int mode);
	void TestChangePartsAll();
	void TestChangeWeapon(RVisualMesh* pVMesh = NULL);
	void TestToggleCharacter();

	void ChangeParts(int mode);
	void ChangeWeapon(ZChangeWeaponType nType);

	void Reload();

	void RespawnMyCharacter();	// ȥ���׽�Ʈ�Ҷ� Ŭ���ϸ� �ǻ�Ƴ���.

	void ReserveLeaveStage();	// ������������ ������ �����ð� �帥�� ������
	void ReserveLeaveBattle();	// �������� ������ �����ð� �帥�� ������
	void FinishGame(void);
	bool IsLeaveBattleReserved() { return m_bLeaveBattleReserved; }

	void ReserveResetApp(bool b)	{ m_bReserveResetApp = b; }
	bool IsReservedResetApp()		{ return m_bReserveResetApp; }

	void SaveScreenShot();

	void ShowMessage(const char* szText, MListener* pCustomListenter = NULL, int nMessageID = 0);
	void ShowConfirmMessage(const char* szText, MListener* pCustomListenter = NULL);
	void ShowMessage(int nMessageID);
	void ShowErrorMessage(int nErrorID);
	void ShowErrorMessage(const char* szErrorMsg, int nErrorID);

	void ShowInterface(bool bShowInterface);
	bool IsShowInterface() { return m_bShowInterface; }

	void SetTeenVersion(bool bt) { m_bTeenVersion = bt; }
	bool GetTeenVersion() { return m_bTeenVersion; }

	void OnCharSelect(void);

	bool OnGameCreate(void);
	void OnGameDestroy(void);
	void OnGameUpdate(float fElapsed);

#ifdef _CW_VOTE
	void OnArrangedTeamGameUI(bool bFinding, bool isvote=false);
#else
	void OnArrangedTeamGameUI(bool bFinding);
#endif // _CW_VOTE
	void OnDuelTournamentGameUI(bool bWaiting);

	void InitLobbyUIByChannelType();

	void InitLadderUI(bool bLadderEnable);
	void InitClanLobbyUI(bool bClanBattleEnable);
	void InitPlayerWarsUI(bool bPlayerWarsEnable);
	void InitDuelTournamentLobbyUI(bool bEnableDuelTournamentUI);
	void InitChannelFrame(MCHANNEL_TYPE nChannelType);

	//	bool InitLocatorList( MZFileSystem* pFileSystem, const char* pszLocatorList );

	// �������� UI ����
	void SetMapThumbnail(const char* szMapName);
	void ClearMapThumbnail();
	void SerializeStageInterface();

	void EnableLobbyInterface(bool bEnable);
	void EnableStageInterface(bool bEnable);
	void ShowPrivateStageJoinFrame(const char* szStageName);

	void SetRoomNoLight(int d);


	// ������ ���
	void ShowEquipmentDialog(bool bShow = true);
	void ShowShopDialog(bool bShow = true);

	// ĳ���� ����
	void ChangeSelectedChar(int nNum);


	// ���÷���
	void ShowReplayDialog(bool bShow);
	void ViewReplay(void);


	void ShowMenu(bool bEnable);
	void Show112Dialog(bool bShow);
	bool IsMenuVisible();

	bool OpenMiniMap();
	bool IsMiniMapEnable();

	void RequestQuickJoin();

	void EnableCharSelectionInterface(bool bEnable);

public:


	// Ŭ�������̳� �����������ʴ븦 ������ �ִ� �����ΰ� ?
	bool IsReadyToPropose();

	// ���÷���
	void OnReplay();

	// XTrap
	void OnRequestXTrapSeedKey(unsigned char *pComBuf);			// add sgk 0402

	void OnDisconnectMsg(const DWORD dwMsgID);
	void ShowDisconnectMsg(DWORD errStrID, DWORD delayTime);

	void OnAnnounceDeleteClan(const string& strAnnounce);

	// ����Ʈ ������ ������ ��Ʈ�� ���(���µ� ������ ������ �ѵ��� ���... -_-;)
	MBitmap* GetQuestItemIcon(int nItemID, bool bSmallIcon);

	// ZActionKey �Է��� GlobalEvent ����ȭ
	static bool CheckSkipGlobalEvent() { return m_bSkipGlobalEvent; }
	void SetSkipGlobalEvent(bool bSkip) { m_bSkipGlobalEvent = bSkip; }

	// ���̽� ���� ���
	void OnVoiceSound();
	void PlayVoiceSound(char* pszSoundName, DWORD time = 0);

	void SetAgentPing(DWORD nIP, DWORD nTimeStamp);

	void OnRequestGameguardAuth(const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3);

	void SetErrMaxPlayerDelayTime(DWORD dDelayTime) { m_dErrMaxPalyerDelayTime = dDelayTime; }
	DWORD GetErrMaxPlayerDelayTime() { return m_dErrMaxPalyerDelayTime; }
	void SetErrMaxPlayer(bool bErrMaxPalyer) { m_bErrMaxPalyer = bErrMaxPalyer; }
	bool IsErrMaxPlayer() { return m_bErrMaxPalyer == 0 ? false : true; }

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

	// _DUELTOURNAMENT
	void SetDuelTournamentCharacterList(MDUELTOURNAMENTTYPE nType, const vector<DTPlayerInfo>& vecDTPlayerInfo);
	const vector<DTPlayerInfo>& GetVectorDTPlayerInfo()	{ return m_vecDTPlayerInfo; }
	void SetDuelTournamantType(MDUELTOURNAMENTTYPE eType)		{ m_eDuelTournamentType = eType; }
	MDUELTOURNAMENTTYPE GetDuelTournamentType()		{ return m_eDuelTournamentType; }

	void UpdateDuelTournamantMyCharInfoUI();
	void UpdateDuelTournamantMyCharInfoPreviousUI();
	void UpdatePlayerWarsMyCharInfo();

	// �����̸�
	bool GetIsGameFinishLeaveBattle()			{ return m_bGameFinishLeaveBattle; }

	// �ٱ����� �������� �������̽��� (__forceinline �� dll-injection �� ���ظ� ���� ��)
	__forceinline ZGameClient* GetGameClient(void)			{ return m_spGameClient; }
	__forceinline ZGame* GetGame(void)						{ return m_pGame; }
	__forceinline ZCombatInterface* GetCombatInterface(void) { return m_pCombatInterface; }
	__forceinline ZShopEquipInterface* GetShopEquipInterface(void) { return m_pShopEquipInterface; }
	__forceinline ZCamera* GetCamera()						{ return &m_Camera; }
	__forceinline ZCharacter*	GetMyCharacter()			{ return (ZCharacter*)m_pMyCharacter; }
	__forceinline ZBaseQuest* GetQuest();					// ����Ʈ/�����̹� �� ���� ��忡 �´� �� ��ȯ
	__forceinline ZQuest* GetQuestExactly()					{ return &m_Quest; }		// ����Ʈ ��ü�� ��ȯ
	__forceinline ZSurvival* GetSurvivalExactly()			{ return &m_Survival; }		// �����̹� ��ü�� ��ȯ
	__forceinline ZChat* GetChat()							{ return &m_Chat; }
	__forceinline ZGameTypeManager* GetGameTypeManager()		{ return &m_GameTypeManager; }


	ZScreenEffectManager* GetScreenEffectManager() { return m_pScreenEffectManager; }
	ZEffectManager* GetEffectManager()			{ return m_pEffectManager; }
	void SetGameClient(ZGameClient* pGameClient){ m_spGameClient = pGameClient; }

	ZCharacterSelectView* GetCharacterSelectView() { return m_pCharacterSelectView; }
	ZIDLResource* GetIDLResource(void)			{ return &m_IDLResource; }
	ZPlayerMenu* GetPlayerMenu()				{ return m_pPlayerMenu; }
	ZMiniMap*	GetMiniMap()					{ return m_pMiniMap; }

	ZTips* GetTips()							{ return &m_Tips; }
	ZBandiCapturer*	GetBandiCapturer()			{ return m_Capture; }					///< ������ ĸ��...by kammir 2008.10.02
	ZScreenDebugger* GetScreenDebugger()		{ return &m_ScreenDebugger; }
	ZCombatMenu*	 GetCombatMenu()			{ return &m_CombatMenu; }
	ZMonsterBookInterface* GetMonsterBookInterface()	{ return m_pMonsterBookInterface; }

	ZBitmapManager<int>* GetItemThumbnailMgr()		{ return &m_ItemThumbnailMgr; }
};

__forceinline ZBaseQuest* ZGameInterface::GetQuest()
{
	if (m_pGame && m_pGame->GetMatch())
	{
		MMATCH_GAMETYPE gameType = ZGetGame()->GetMatch()->GetMatchType();

		if (m_GameTypeManager.IsQuestOnly(gameType))
			return static_cast<ZBaseQuest*>(&m_Quest);
		else if (m_GameTypeManager.IsSurvivalOnly(gameType))
			return static_cast<ZBaseQuest*>(&m_Survival);
	}

	//ASSERT(0);
	return static_cast<ZBaseQuest*>(&m_Quest);
}


#define BEGIN_WIDGETLIST(_ITEM, _IDLRESPTR, _CLASS, _INSTANCE)								\
{																							\
	MWidgetList WidgetList;																	\
	(_IDLRESPTR)->FindWidgets(WidgetList, _ITEM);											\
	for (MWidgetList::iterator itor = WidgetList.begin(); itor != WidgetList.end(); ++itor) \
		{																							\
	if ((*itor) != NULL)																	\
		{																							\
	_CLASS _INSTANCE = ((_CLASS)(*itor));

#define END_WIDGETLIST()		}}}


#define DEFAULT_INTERFACE_SKIN "Default"


#define WM_CHANGE_GAMESTATE		(WM_USER + 25)
void ZChangeGameState(GunzState state);		/// �����忡 �����ϱ� ���ؼ��� ����


//void ZLoadBitmap(const char* szDir, const char* szFilter, bool bAddDirToAliasName = false);

inline void GetCountryFlagIconFileName(char* out_sz, int nCountry)
{
	switch (nCountry)
	{
	default:
		sprintf(out_sz, "None.png");
		break;
	case PANAMA:
		sprintf(out_sz, "Panama.png");
		break;
	case JAPON:
		sprintf(out_sz, "Japon.png");
		break;
	case BRASIL:
		sprintf(out_sz, "Brasil.png");
		break;
	case ARGENTINA:
		sprintf(out_sz, "Argentina.png");
		break;
	case BOLIVIA:
		sprintf(out_sz, "Bolivia.png");
		break;
	case KOREA:
		sprintf(out_sz, "Korea.png");
		break;
	case COLOMBIA:
		sprintf(out_sz, "Colombia.png");
		break;
	case COSTA_RICA:
		sprintf(out_sz, "CostaRica.png");
		break;
	case CUBA:
		sprintf(out_sz, "Cuba.png");
		break;
	case ECUADOR:
		sprintf(out_sz, "Ecuador.png");
		break;
	case MEXICO:
		sprintf(out_sz, "Mexico.png");
		break;
	case PARAGUAY:
		sprintf(out_sz, "Paraguay.png");
		break;
	case URUGUAY:
		sprintf(out_sz, "Uruguay.png");
		break;
	case VENEZUELA:
		sprintf(out_sz, "Venezuela.png");
		break;
	case SPAIN:
		sprintf(out_sz, "Spain.png");
		break;
	case CANADA:
		sprintf(out_sz, "Canada.png");
		break;
	case PERU:
		sprintf(out_sz, "Peru.png");
		break;
	case USA:
		sprintf(out_sz, "USA.png");
		break;
	case CHILE:
		sprintf(out_sz, "Chile.png");
		break;
	case ISRAEL:
		sprintf(out_sz, "Israel.png");
		break;
	};
}
inline void GetDuelTournamentGradeIconFileName(char* out_sz, int grade)
{
	sprintf(out_sz, "dt_grade%d.png", grade);
}

char* GetItemSlotName(const char* szName, int nItem);
bool SetWidgetToolTipText(char* szWidget, const char* szToolTipText, MAlignmentMode mam = MAM_LEFT | MAM_TOP);

#endif
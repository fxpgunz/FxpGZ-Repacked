#ifndef _ZCOMBATINTERFACE_H
#define _ZCOMBATINTERFACE_H

#include "ZInterface.h"
#include "MPicture.h"
#include "MEdit.h"
#include "MListBox.h"
#include "MLabel.h"
#include "MAnimation.h"
#include "ZObserver.h"
#include "ZCombatChat.h"
#include "ZCrossHair.h"
#include "ZMiniMap.h"
#include "ZVoteInterface.h"

_USING_NAMESPACE_REALSPACE2

class ZCharacter;
class ZScreenEffect;
class ZWeaponScreenEffect;
class ZMiniMap;
class ZCombatQuestScreen;

struct ZResultBoardItem {
	char szName[64];
	char szClan[CLAN_NAME_LENGTH];
	int nClanID;
	int nTeam;
	int nScore;
	int nKills;
	int nDeaths;
	int	nAllKill;
	int	nExcellent;
	int	nFantastic;
	int	nHeadShot;
	int	nUnbelievable;
	bool bMyChar;
	bool bGameRoomUser;

	ZResultBoardItem() { }
	ZResultBoardItem(const char *_szName, const char *_szClan, int _nTeam, int _nScore, int _nKills, int _nDeaths, bool _bMyChar = false, bool _bGameRoomUser = false) {
		strcpy(szName, _szName);
		strcpy(szClan, _szClan);
		nTeam = _nTeam;
		nScore = _nScore;
		nKills = _nKills;
		nDeaths = _nDeaths;
		// ÇÊ¿äÇÏ¸é ÀÌ°Íµéµµ ¸¸µéÀÚ
		nAllKill = 0;
		nExcellent = 0;
		nFantastic = 0;
		nHeadShot = 0;
		nUnbelievable = 0;
		bMyChar = _bMyChar;
		bGameRoomUser = _bGameRoomUser;
	}
};

class ZResultBoardList : public list<ZResultBoardItem*>
{
public:
	void Destroy() {
		while (!empty())
		{
			delete *begin();
			erase(begin());
		}
	}
};

struct DuelTournamentPlayer
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	MUID uidPlayer;
	int m_nTP;
	int nVictory;
	int nMatchLevel;
	int nNumber;

	float fMaxHP;
	float fMaxAP;
	float fHP;
	float fAP;
};

class ZCombatInterface : public ZInterface
{
private:
	float				m_fElapsed;
protected:
	ZWeaponScreenEffect*		m_pWeaponScreenEffect;
	//	ZScoreBoard*		m_pScoreBoard;

	// °á°úÈ­¸é¿¡ ÇÊ¿äÇÑ°Í
	ZScreenEffect*		m_pResultPanel;
	ZScreenEffect*		m_pResultPanel_Team;
	ZResultBoardList	m_ResultItems;
	ZScreenEffect*		m_pResultLeft;
	ZScreenEffect*		m_pResultRight;

	int					m_nClanIDRed;			///< Å¬·£ÀüÀÏ¶§
	int					m_nClanIDBlue;			///< µÎ Å¬·£ ID
	char				m_szRedClanName[32];
	char				m_szBlueClanName[32];	///< µÎ Å¬·£ÀÇ ÀÌ¸§

	ZCombatQuestScreen*	m_pQuestScreen;

	ZBandiCapturer*		m_Capture;					///< µ¿¿µ»ó Ä¸ÃÄ...by kammir 2008.10.02
	bool				m_bShowUI;

	ZObserver			m_Observer;			///< ¿ÉÁ®¹ö ¸ðµå
	ZCrossHair			m_CrossHair;		///< Å©·Î½º Çì¾î
	ZVoteInterface		m_VoteInterface;

	ZIDLResource*		m_pIDLResource;

	MLabel*				m_pTargetLabel;
	MBitmap*			m_ppIcons[ZCI_END];		/// ÄªÂù ¾ÆÀÌÄÜµé
	MBitmapR2*			m_pResultBgImg;

	bool				m_bMenuVisible;

	bool				m_bPickTarget;
	char				m_szTargetName[256];		// crosshair target ÀÌ¸§

	MMatchItemDesc*		m_pLastItemDesc;

	int					m_nBulletSpare;
	int					m_nBulletCurrMagazine;
	int					m_nMagazine;
#ifdef _GUNGAME
	int                 m_nGunLevel;
#endif

	int					m_nBulletImageIndex;
	int					m_nMagazineImageIndex;

	char				m_szItemName[256];

	bool				m_bReserveFinish;
	unsigned long int	m_nReserveFinishTime;

	bool				m_bDrawLeaveBattle;
	int					m_nDrawLeaveBattleSeconds;

	bool				m_bOnFinish;
	bool				m_bShowResult;
	bool				m_bIsShowUI;					// ¸ðµç UI °¨Ãß±â... by kammir 20081020 (À¯ÀúÀÇ ¼±ÅÃ»çÇ×)
	bool				m_bSkipUIDrawByRule;			// °ÔÀÓ·êÀÌ ÇÊ¿äÇÏ´Ù¸é UI µå·Î¿ì¸¦ ²ø ¼ö ÀÖµµ·Ï

	bool				m_bDrawScoreBoard;
	//	bool				m_bKickPlayerListVisible;		// °ÔÀÓÈ­¸é¿¡ ÇÃ·¹ÀÌ¾î ¸®½ºÆ® º¸¿©ÁØ´Ù

	float				m_fOrgMusicVolume;

	bool				m_bNetworkAlive;
	DWORD				m_dLastTimeTick;
	DWORD				m_dAbuseHandicapTick;

	MBitmapR2*			m_pInfectedOverlay;
	MBitmapR2*			m_pInfectedWidescreenOverlay;

	void SetItemImageIndex(int nIndex);

	void SetItemName(const char* szName);
	void UpdateCombo(ZCharacter* pCharacter);

	void OnFinish();

	void GameCheckPickCharacter();

	// È­¸é¿¡ ±×¸®´Â°Í°ú °ü·ÃµÈ Æã¼Çµé
	void IconRelative(MDrawContext* pDC, float x, float y, int nIcon);

	void DrawFriendName(MDrawContext* pDC);			// °°ÀºÆí ÀÌ¸§
	void DrawEnemyName(MDrawContext* pDC);			// Àû ÀÌ¸§
	void DrawAllClanName(MDrawContext* pDC);
	void DrawAllPlayerName(MDrawContext* pDC);		// ¸ðµç ÆÀ ÀÌ¸§ Ç¥½Ã (Free Spectator)
	void DrawSpyName(MDrawContext* pDC);
	// void DrawHPAPBar(MDrawContext* pDC);
#ifdef	_EVENTCHANNEL
	void Alive(MDrawContext* pDC);
#endif
	void DrawScoreBoard(MDrawContext* pDC);			// Á¡¼ö È­¸é (tabÅ°)
	void DrawDuelTournamentScoreBoard(MDrawContext* pDC);						// µà¾ó Åä³Ê¸ÕÆ® ´ëÁøÇ¥ È­¸é (tabÅ°)
	void DrawPlayTime(MDrawContext* pDC, float xPos, float yPos);	// ÇÃ·¹ÀÌ ½Ã°£
	void DrawResultBoard(MDrawContext* pDC);		// °ÔÀÓ °á°úÈ­¸é
	void DrawSoloSpawnTimeMessage(MDrawContext* pDC);	// ½ò·Î ½ºÆùÀü Å¸ÀÌ¸Ó ¸Þ½ÃÁö
#ifdef _MESSAGENBATTLE
	void DrawSoloSpawnTimeMessageBattle(MDrawContext* pDC);	// ½ò·Î ½ºÆùÀü Å¸ÀÌ¸Ó ¸Þ½ÃÁö
#endif
	void DrawLeaveBattleTimeMessage(MDrawContext* pDC);	// °ÔÀÓ¿¡¼­ ³ª°¥¶§ ±â´Ù¸®´Â ½Ã°£Ç¥½Ã
	//	void DrawVoteMessage(MDrawContext* pDC);		// ÅõÇ¥°¡ ÁøÇàÁßÀÏ¶§ ¸Þ½ÃÁö
	//	void DrawKickPlayerList(MDrawContext* pDC);		// kick ÇÒ ÇÃ·¹ÀÌ¾î ¼±ÅÃÇÏ´Â È­¸é
	void GetResultInfo(void);

	void DrawTDMScore(MDrawContext* pDC);

	void DrawNPCName(MDrawContext* pDC);	// µð¹ö±×¿ë

	void UpdateNetworkAlive(MDrawContext* pDC);

public:
	ZCombatChat			m_Chat;
	ZCombatChat			m_AdminMsg;
	DWORD				m_nReservedOutTime;				// Finish ÈÄ¿¡ ¹ÛÀ¸·Î ³ª°¡´Â ½Ã°£À» ¼³Á¤


	ZCombatInterface(const char* szName = NULL, MWidget* pParent = NULL, MListener* pListener = NULL);
	virtual ~ZCombatInterface();

	void OnInvalidate();
	void OnRestore();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void OnDraw(MDrawContext* pDC);	// ±×¸®´Â ¼ø¼­¶§¹®¿¡ Á÷Á¢ ±×¸°´Ù
	virtual void OnDrawCustom(MDrawContext* pDC);
	virtual void DrawAfterWidgets(MDrawContext* pDC);	//MWidget¿¡¼­ ÀçÁ¤ÀÇÇÑ ÇÔ¼ö, UI À§Á¬À» ¸ðµÎ ±×¸° ÈÄ Á÷Á¢ DC¿¡ Ãß°¡·Î ±×¸®±â À§ÇÑ ÇÔ¼ö
	void		 DrawPont(MDrawContext* pDC);
	void		 DrawMyNamePont(MDrawContext* pDC);
	void	     DrawClanMembersTextInfo(MDrawContext* pDC);
	void		 DrawMyWeaponPont(MDrawContext* pDC);
	void		 DrawScore(MDrawContext* pDC);
	void		 DrawBuffStatus(MDrawContext* pDC);
	void		 DrawFinish();
	int DrawVictory(MDrawContext* pDC, int x, int y, int nWinCount, bool bGetWidth = false);

	virtual bool IsDone();

	void OnAddCharacter(ZCharacter *pChar);
	void UpdateCTFMsg(const char* szMsg);

	void Resize(int w, int h);

	void OutputChatMsg(const char* szMsg);
	void OutputChatMsg(MCOLOR color, const char* szMsg);

	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

	static MFont *GetGameFont();
	MPOINT GetCrosshairPoint() { return MPOINT(MGetWorkspaceWidth() / 2, MGetWorkspaceHeight() / 2); }

	ZBandiCapturer*	GetBandiCapturer()			{ return m_Capture; }					///< µ¿¿µ»ó Ä¸ÃÄ...by kammir 2008.10.02

	void ShowMenu(bool bVisible = true);
	void ShowInfo(bool bVisible = true);
	void EnableInputChat(bool bInput = true, bool bTeamChat = false);

	void SetDrawLeaveBattle(bool bShow, int nSeconds);

	void ShowChatOutput(bool bShow);
	bool IsChat() { return m_Chat.IsChat(); }
	bool IsTeamChat() { return m_Chat.IsTeamChat(); }
	bool IsMenuVisible() { return m_bMenuVisible; }


#ifdef _GUNGAME
	void GunMode(ZCharacter* pCharacter, int GunLevel);
	int AddGunLevel(){ return m_nGunLevel++; };
#endif

	void Update(float fElapsed);
	void SetPickTarget(bool bPick, ZCharacter* pCharacter = NULL);

	//	void ShowScoreBoard(bool bVisible = true);
	//	bool IsScoreBoardVisible() { return m_pScoreBoard->IsVisible(); }

	void Finish();
	bool IsFinish();

	ZCharacter* GetTargetCharacter();
	MUID		GetTargetUID();

	int GetPlayTime();

	void SetObserverMode(bool bEnable);
	bool GetObserverMode() { return m_Observer.IsVisible(); }
	ZObserver* GetObserver() { return &m_Observer; }
	ZCrossHair* GetCrossHair() { return &m_CrossHair; }

	ZVoteInterface* GetVoteInterface()	{ return &m_VoteInterface; }

	void ShowCrossHair(bool bVisible) { m_CrossHair.Show(bVisible); }
	void OnGadget(MMatchWeaponType nWeaponType);
	void OnGadgetOff();

	void SetSkipUIDraw(bool b) { m_bSkipUIDrawByRule = b; }
	bool IsSkupUIDraw() { return m_bSkipUIDrawByRule; }

	bool IsShowResult(void)  { return m_bShowResult; }
	bool IsShowUI(void)  { return m_bIsShowUI; }
	void SetIsShowUI(bool bIsShowUI)  { m_bIsShowUI = bIsShowUI; }
	bool IsShowScoreBoard()   { return m_bDrawScoreBoard; }
	//	void SetKickPlayerListVisible(bool bShow = true) { m_bKickPlayerListVisible = bShow; }
	//	bool IsKickPlayerListVisible() { return m_bKickPlayerListVisible; }

	bool IsNetworkalive()	{ return m_bNetworkAlive; }

	const char* GetRedClanName() const { return m_szRedClanName; }
	const char* GetBlueClanName() const { return m_szBlueClanName; }
	ZWeaponScreenEffect* GetWeaponScreenEffect() { return m_pWeaponScreenEffect; }
public:
	void OnSpyCreate();
	void OnSpyDestroy();

	void CreateSpyGameInterface();
	void CreateSpyWaitInterface();

	void OnSpyUpdate(float fElapsed);
	void OnSpyDraw(MDrawContext* pDC);

	void SetSpyTip(const char* msg);
	void SetSpyEventMsg(const char* imgName);
	void SetSpyNotice(const char* imgName);

	void SetDefaultSpyTip(MMatchTeam team);
	const char* GetSuitableSpyItemTip(int itemid);

	bool m_bSpyLocationOpened;

protected:
	MBitmapR2* m_pSpyIcon;

protected:
	void SetSpyTimeLimitValue(int m, int s, int ms);
	void SetSpyTimeLimitValue(DWORD dwTime);

public:
	void SetSpyTimer(DWORD dwTimeLimit);
protected:
	DWORD m_dwSpyTimer;

public:
	void PlaySpyNotice(const char* imgName);
protected:
	bool m_bSpyNoticePlaying;
	DWORD m_dwSpyNoticePlayStartedTime;
};

void TextRelative(MDrawContext* pDC, float x, float y, const char *szText, bool bCenter = false);
#endif
#include "stdafx.h"

#include "ZGameClient.h"
#include "ZCombatInterface.h"
#include "ZGameInterface.h"
#include "ZInterfaceItem.h"
#include "ZInterfaceListener.h"
#include "ZApplication.h"
#include "ZCharacter.h"
#include "ZCharacterManager.h"
#include "RealSpace2.h"
#include "MComboBox.h"
#include "RTypes.h"
#include "ZScreenEffectManager.h"
#include "ZActionDef.h"
#include "ZEffectFlashBang.h"
#include "ZConfiguration.h"
#include "ZPost.h"
#include "ZWeaponScreenEffect.h"
#include "MemPool.h"
#include "ZMyInfo.h"
#include "ZCharacterView.h"
#include "ZItemSlotView.h"
#include <algorithm>
#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")

#include "ZApplication.h"
#include "ZCombatQuestScreen.h"
#include "ZBmNumLabel.h"
#include "ZModule_QuestStatus.h"
#include "ZLocale.h"
#include "MagicBox/ZRuleDropGunGame.h"

#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"
#include "ZInput.h"
#include "ZGlobalAdminAnnounceInterface.h"
#include "ZNHN_USA_Report.h"

#include "../SpyModeSystem/MMatchSpyMode.h"
#include "../SpyModeSystem/MMatchSpyMap.h"

using namespace std;


// CONSTANTS
#define BACKGROUND_COLOR1					0xff202020
#define BACKGROUND_COLOR2					0xff000000
#define BACKGROUND_COLOR_MYCHAR_DEATH_MATCH	MINT_ARGB(255*40/100,140,180,255)
#define BACKGROUND_COLOR_MYCHAR_RED_TEAM	MINT_ARGB(255*40/100,255,50,50)
#define BACKGROUND_COLOR_MYCHAR_BLUE_TEAM	MINT_ARGB(255*40/100,50,50,255)

#define BACKGROUND_COLOR_COMMANDER			MINT_ARGB(255*40/100,255,88,255)

#define TEXT_COLOR_TITLE			0xFFAAAAAA
#define TEXT_COLOR_DEATH_MATCH		0xfffff696
#define TEXT_COLOR_DEATH_MATCH_DEAD	0xff807b4b
#define TEXT_COLOR_BLUE_TEAM		0xff8080ff
#define TEXT_COLOR_BLUE_TEAM_DEAD	0xff606080
#define TEXT_COLOR_RED_TEAM			0xffff8080
#define TEXT_COLOR_RED_TEAM_DEAD	0xff806060
#define TEXT_COLOR_SPECTATOR		0xff808080
#define TEXT_COLOR_CLAN_NAME		0xffffffff

// Custom: CTF
bool g_bShowCTFMsg = false;
DWORD g_dwCTFMsgShowTime = 0;
static char g_szCTFMsg[128] = { 0, };

struct ZScoreBoardItem : public CMemPoolSm<ZScoreBoardItem>{
	MUID uidUID;
	char szLevel[16];
	char szName[64];
	char szClan[CLAN_NAME_LENGTH];
	int nDuelQueueIdx;
	int	nClanID;
	int nTeam;
	bool bDeath;
	int nExp;
	int nKills;
	int nDeaths;
	int nPing;
	int nDTLastWeekGrade;
	int nCountry;
	bool bMyChar;
	bool bCommander;
	bool bGameRoomUser;
	float HP, AP, MaxHP, MaxAP;

	MCOLOR SpColor;
	bool  bSpColor;

	ZScoreBoardItem(const MUID& _uidUID, char* _szLevel, char *_szName, char *_szClan, int _nTeam, bool _bDeath, int _nExp, int _nKills, int _nDeaths, int _nPing, int nCountry, int _nDTLastWeekGrade, bool _bMyChar, bool _bGameRoomUser, float hp, float ap, float maxhp, float maxap, bool _bCommander = false)
	{
		uidUID = _uidUID;
		strcpy(szLevel, _szLevel);
		strcpy(szName, _szName);
		strcpy(szClan, _szClan);
		nTeam = _nTeam;
		nExp = nExp;
		bDeath = _bDeath;
		nKills = _nKills;
		nDeaths = _nDeaths;
		nPing = _nPing;
		bMyChar = _bMyChar;
		bCommander = _bCommander;
		bSpColor = false;
		SpColor = MCOLOR(0, 0, 0);
		bGameRoomUser = _bGameRoomUser;
		nDTLastWeekGrade = _nDTLastWeekGrade;
		nCountry = nCountry;
		HP = hp;
		AP = ap;
		MaxHP = maxhp;
		MaxAP = maxap;
	}
	ZScoreBoardItem() {
		bSpColor = false;
		SpColor = MCOLOR(0, 0, 0);
		HP = 0;
		AP = 0;
		MaxHP = 0;
		MaxAP = 0;
	}

	void SetColor(MCOLOR c) {
		SpColor = c;
		bSpColor = true;
	}

	MCOLOR GetColor() {
		return SpColor;
	}
};

ZCombatInterface::ZCombatInterface(const char* szName, MWidget* pParent, MListener* pListener)
	: ZInterface(szName, pParent, pListener)
{
	m_fElapsed = 0;

	m_nBulletSpare = 0;
	m_nBulletCurrMagazine = 0;
#ifdef _GUNGAME
	m_nGunLevel = 1;
#endif
	m_nMagazine = 0;
	memset(m_szItemName, 0, sizeof(m_szItemName));

	//	m_pScoreBoard = NULL;
	m_pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	m_bMenuVisible = false;

	m_bPickTarget = false;
	m_pLastItemDesc = NULL;

	m_bReserveFinish = false;

	m_pTargetLabel = NULL;
	m_szTargetName[0] = 0;

	m_nBulletImageIndex = 0;
	m_nMagazineImageIndex = 0;

	m_nReserveFinishTime = 0;

	m_pWeaponScreenEffect = NULL;

	m_pResultPanel = NULL;
	m_pResultPanel_Team = NULL;
	m_pResultLeft = NULL;
	m_pResultRight = NULL;

	m_pQuestScreen = NULL;

	//	m_bKickPlayerListVisible = false;

	m_nClanIDRed = 0;
	m_nClanIDBlue = 0;
	m_szRedClanName[0] = 0;
	m_szBlueClanName[0] = 0;

	m_bNetworkAlive = true;		// 인터넷 연결되어있음
	m_dLastTimeTick = 0;
	m_dAbuseHandicapTick = 0;

	m_bSkipUIDrawByRule = false;

	m_pInfectedOverlay = NULL;
	m_pInfectedWidescreenOverlay = NULL;

	m_bSpyLocationOpened = false;
	m_pSpyIcon = NULL;

	m_dwSpyTimer = 0;

	m_bSpyNoticePlaying = false;
	m_dwSpyNoticePlayStartedTime = 0;
}

ZCombatInterface::~ZCombatInterface()
{
	OnDestroy();
}

bool ZCombatInterface::OnCreate()
{
	ZGetGame()->m_pMyCharacter->EnableAccumulationDamage(false);

	m_Observer.Create(ZApplication::GetGameInterface()->GetCamera(),
		ZApplication::GetGameInterface()->GetIDLResource());


	m_pTargetLabel = new MLabel("", this, this);
	m_pTargetLabel->SetTextColor(0xffff0000);
	m_pTargetLabel->SetSize(100, 30);

	ShowInfo(true);

	m_pResultBgImg = NULL;
	m_bDrawScoreBoard = false;

	EnableInputChat(false);

	m_Chat.Create("CombatChatOutput", true);
	m_Chat.ShowOutput(ZGetConfiguration()->GetViewGameChat());
	m_Chat.m_pChattingOutput->ReleaseFocus();

	m_AdminMsg.Create("CombatChatOutputAdmin", false);
	MFont* pFont = MFontManager::Get("FONTb11b");
	m_AdminMsg.SetFont(pFont);
	m_AdminMsg.m_pChattingOutput->ReleaseFocus();

	if (ZGetMyInfo()->IsAdminGrade()) {
		MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
		if (pCache && pCache->GetUGrade() == MMUG_EVENTMASTER && pCache->CheckFlag(MTD_PlayerFlags_AdminHide)) {
			ShowChatOutput(false);
		}
	}

	m_ppIcons[0] = MBitmapManager::Get("medal_A.tga");
	m_ppIcons[1] = MBitmapManager::Get("medal_U.tga");
	m_ppIcons[2] = MBitmapManager::Get("medal_E.tga");
	m_ppIcons[3] = MBitmapManager::Get("medal_F.tga");
	m_ppIcons[4] = MBitmapManager::Get("medal_H.tga");

	m_CrossHair.Create();
	m_CrossHair.ChangeFromOption();

	m_pWeaponScreenEffect = new ZWeaponScreenEffect;
	m_pWeaponScreenEffect->Create();

	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGame()->GetMatch()->GetMatchType()))
	{
		m_pQuestScreen = new ZCombatQuestScreen();
	}

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatTDMInfo");
	if (pWidget)
	{
		if (ZGetGameTypeManager()->IsTeamExtremeGame(ZGetGame()->GetMatch()->GetMatchType()))
		{
			int nMargin[BMNUM_NUMOFCHARSET] = { 13, 9, 13, 13, 13, 13, 13, 13, 13, 13, 8, 10, 8 };

			ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Blue");
			if (pBmNumLabel)
			{
				pBmNumLabel->SetAlignmentMode(MAM_HCENTER);
				pBmNumLabel->SetCharMargin(nMargin);
				pBmNumLabel->SetNumber(0);
			}

			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Red");
			if (pBmNumLabel)
			{
				pBmNumLabel->SetAlignmentMode(MAM_HCENTER);
				pBmNumLabel->SetIndexOffset(16);
				pBmNumLabel->SetCharMargin(nMargin);
				pBmNumLabel->SetNumber(0);
			}

			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Max");
			if (pBmNumLabel)
			{
				pBmNumLabel->SetAlignmentMode(MAM_HCENTER);
				pBmNumLabel->SetIndexOffset(32);
				int nMargin2[BMNUM_NUMOFCHARSET] = { 18, 12, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18 };
				pBmNumLabel->SetCharMargin(nMargin2);
				pBmNumLabel->SetNumber(0);
			}

			pWidget->Show(true);


			MWidget *pPicture = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_RedWin");
			if (pPicture)
				pPicture->Show(true);
			pPicture = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_BlueWin");
			if (pPicture)
				pPicture->Show(true);
		}
		else
		{
			pWidget->Show(false);

			MWidget *pPicture = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_RedWin");
			if (pPicture)
				pPicture->Show(false);
			pPicture = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_BlueWin");
			if (pPicture)
				pPicture->Show(false);
		}
	}
	// Custom: Infected
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED)
	{
		SAFE_DELETE(m_pInfectedOverlay);
		SAFE_DELETE(m_pInfectedWidescreenOverlay);
		m_pInfectedOverlay = new MBitmapR2;
		m_pInfectedWidescreenOverlay = new MBitmapR2;
		m_pInfectedOverlay->Create("zombie_overlay.dds", RGetDevice(), "interface/Default/Combat/zombie_overlay.dds");
		m_pInfectedWidescreenOverlay->Create("zombie_overlay_ws.dds", RGetDevice(), "interface/Default/Combat/zombie_overlay_ws.dds");
	}
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
	{
		// 듀얼토너먼트 남은시간 표시
		MWidget *pPicture = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_TimeBack");
		if (pPicture)
			pPicture->Show(true);

		ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("DT_RemainTime");

		if (pBmNumLabel)
		{
			pBmNumLabel->SetAlignmentMode(MAM_HCENTER);
			pBmNumLabel->SetIndexOffset(32);
			int nMargin[BMNUM_NUMOFCHARSET] = { 18, 12, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18 };
			pBmNumLabel->SetCharMargin(nMargin);
			pBmNumLabel->SetNumber(0);
		}

		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDTInfo");
		if (pWidget)
			pWidget->Show(true);

		// 대전자 정보 레이블 align (레이블align은 버그가 있어서 xml에서 지정할수 없다-_-;)
		MWidget* pFrame = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_CharacterInfo");
		if (pFrame)
		{
			int numChild = pFrame->GetChildCount();
			MWidget* pChild = NULL;
			for (int i = 0; i<numChild; ++i)
			{
				pChild = pFrame->GetChild(i);
				if (pChild &&
					strcmp(pChild->GetClassName(), MINT_LABEL) == 0 &&
					strstr(pChild->m_szIDLName, "CombatDT_PlayerInfo_"))
				{
					if (strstr(pChild->m_szIDLName, "Left"))
						((MLabel*)pChild)->SetAlignment(MAM_RIGHT | MAM_VCENTER);
					else if (strstr(pChild->m_szIDLName, "Right"))
						((MLabel*)pChild)->SetAlignment(MAM_LEFT | MAM_VCENTER);
					else
						((MLabel*)pChild)->SetAlignment(MAM_HCENTER | MAM_VCENTER);
				}
			}
		}
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_MatchLevel");
		if (pLabel)
			pLabel->SetAlignment(MAM_HCENTER | MAM_VCENTER);

		GetWidgetCharViewLeft()->SetEnableRotateZoom(false, false);
		GetWidgetCharViewRight()->SetEnableRotateZoom(false, false);
		GetWidgetCharViewResult()->SetEnableRotateZoom(false, false);

		// 누적데미지 적용(듀얼토너먼트일시 OK승이 힘들때 누적데미지로 판정승 처리용)
		ZGetGame()->m_pMyCharacter->EnableAccumulationDamage(true);
	}

	// 게임 나가기 버튼의 확인 메시지를 게임룰에 따라 변경한다
	MButton* pExitConfirmButton = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageExit");
	if (pExitConfirmButton) {
		char szConfirmMsg[256];
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
		{
			ZTransMsg(szConfirmMsg, MSG_GAME_DUELTOURNAMENT_MATCH_EXITSTAGE_CONFIRM);
			pExitConfirmButton->SetAlterableConfirmMessage(szConfirmMsg);
		}
		else
		{
			pExitConfirmButton->RestoreIDLConfirmMessage();
		}
	}

	if (ZGetGame())
	{
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
		{
			m_pSpyIcon = new MBitmapR2;
			m_pSpyIcon->Create("icon_spy.tga", RGetDevice(), "interface/default/icon_spy.tga");

			OnSpyCreate();
		}
	}

#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->Set3DSoundUpdate(true);
#endif

	m_bOnFinish = false;
	m_bShowResult = false;
	m_bIsShowUI = true;

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Option");
	if (pWidget)
		pWidget->Show(false);

	return true;
}


void ZCombatInterface::OnDestroy()
{
	if (m_nClanIDBlue) {
		ZGetEmblemInterface()->DeleteClanInfo(m_nClanIDBlue);
		m_nClanIDBlue = 0;
	}
	if (m_nClanIDRed) {
		ZGetEmblemInterface()->DeleteClanInfo(m_nClanIDRed);
		m_nClanIDRed = 0;
	}
	// Infected Mode
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED)
	{
		SAFE_DELETE(m_pInfectedOverlay);
		SAFE_DELETE(m_pInfectedWidescreenOverlay);
	}
	if (IsGameRuleCTF(ZGetGame()->GetMatch()->GetMatchType()))
	{
		ZGetScreenEffectManager()->DestroyQuestRes();
	}
	if (m_pQuestScreen){ delete m_pQuestScreen; m_pQuestScreen = NULL; }

	m_Observer.Destroy();

	m_ResultItems.Destroy();
	SAFE_DELETE(m_pResultPanel);
	SAFE_DELETE(m_pResultPanel_Team);
	SAFE_DELETE(m_pResultLeft);
	SAFE_DELETE(m_pResultRight);

	EnableInputChat(false);

	m_Chat.Destroy();
	m_AdminMsg.Destroy();

	m_CrossHair.Destroy();

	if (m_pTargetLabel)
	{
		delete m_pTargetLabel;
		m_pTargetLabel = NULL;
	}
	ShowInfo(false);
	if (m_pSpyIcon)
	{
		delete m_pSpyIcon;
		m_pSpyIcon = NULL;
	}

	OnSpyDestroy();

#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->Set3DSoundUpdate(false);
#endif

	MPicture *pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("GameResult_Background");
	if (pPicture)
		pPicture->SetBitmap(NULL);

	if (m_pResultBgImg != NULL)
	{
		delete m_pResultBgImg;
		m_pResultBgImg = NULL;
	}


	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatTDMInfo");
	if (pWidget)
		pWidget->Show(false);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDTInfo");
	if (pWidget)
		pWidget->Show(false);

	ZCharacterView* pCharView = GetWidgetCharViewLeft();
	if (pCharView)
		pCharView->SetCharacter(MUID(0, 0));

	pCharView = GetWidgetCharViewRight();
	if (pCharView)
		pCharView->SetCharacter(MUID(0, 0));

	pCharView = GetWidgetCharViewResult();
	if (pCharView)
		pCharView->SetCharacter(MUID(0, 0));

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_CharacterInfo");
	if (pWidget)
		pWidget->Show(false);


	m_pWeaponScreenEffect->Destroy();
	//SAFE_DELETE(m_pWeaponScreenEffect);

	ZScoreBoardItem::Release();
}

void TextRelative(MDrawContext* pDC, float x, float y, const char *szText, bool bCenter)
{
	int screenx = x*MGetWorkspaceWidth();
	if (bCenter)
	{
		MFont *pFont = pDC->GetFont();
		screenx -= pFont->GetWidth(szText) / 2;
	}

	pDC->Text(screenx, y*MGetWorkspaceHeight(), szText);
}

void BitmapRelative(MDrawContext* pDC, float x, float y, float w, float h, MBitmap* pBitmap, bool bCenter = false)
{
	pDC->SetBitmap(pBitmap);

	int screenx = x*MGetWorkspaceWidth();
	if (bCenter)
	{
		MFont *pFont = pDC->GetFont();
		screenx -= w / 2;
	}

	pDC->Draw(screenx, y*MGetWorkspaceHeight(), w, h);
}

void MatchOrderRelative(MDrawContext* pDC, float x, float y, float fHalfGrid, int nMatchCount, int nCouple, bool bBlink)
{
	// UI가 나오면 그림으로 대체할꺼임.... 그래서 매직넘버 사용
	float screenx = x*MGetWorkspaceWidth();
	float screeny = y*MGetWorkspaceHeight();

	// 대진표 박스 그려주기
	float fRectX = screenx - 2;
	float fRectY = screeny - 4;
	float fRectWidth = 0.138f*MGetWorkspaceWidth();
	float fRectHeight = 0.04f*MGetWorkspaceHeight();
	pDC->Rectangle(fRectX, fRectY, fRectWidth, fRectHeight);

	if (bBlink)
		return;
	// 대진표 선들 그려주기
	float fLineWidth = 0.032f*MGetWorkspaceWidth();
	float fLineHeight = 0.038f*MGetWorkspaceHeight();

	MDUELTOURNAMENTTYPE eDTType = ZApplication::GetGameInterface()->GetDuelTournamentType();
	switch (eDTType)
	{
	case MDUELTOURNAMENTTYPE_FINAL:				//< 결승전부터
	{
		fLineWidth = 0.145f*MGetWorkspaceWidth();
		if (nCouple != 1)
			pDC->HLine(fRectX + fRectWidth, fRectY + fRectHeight / 2, fLineWidth);
		return;
	}
	break;
	case MDUELTOURNAMENTTYPE_SEMIFINAL:			//< 4강전부터
	{
		fLineWidth = 0.082f*MGetWorkspaceWidth();
		fLineHeight = 0.049f*MGetWorkspaceHeight();
	}
	break;
	case MDUELTOURNAMENTTYPE_QUATERFINAL:		//< 8강전부터
	{
		fLineWidth = 0.032f*MGetWorkspaceWidth();
		fLineHeight = 0.038f*MGetWorkspaceHeight();
	}
	break;
	}

	fLineHeight *= nMatchCount + 0.9f;
	float fHalf = fHalfGrid*MGetWorkspaceHeight();
	if (nCouple)
	{
		fLineHeight = -fLineHeight; // 대진표 세로선을 위로 그릴지 아래로 그릴지 정해준다.
		fHalf = -fHalf;
	}
	// 박스 우측 가로선
	pDC->HLine(fRectX + fRectWidth, fRectY + fRectHeight / 2, fLineWidth);
	// 박스 우측 아래 세로선
	pDC->VLine(fRectX + fRectWidth + fLineWidth, fRectY + fRectHeight / 2, fLineHeight);
	// 박스 한쌍 가운대 선
	if (nMatchCount != eDTType + 1) // Final 가운데 선은 안그린다.
		pDC->HLine(fRectX + fRectWidth + fLineWidth, fRectY + fRectHeight / 2 + fHalf, fLineWidth);
}

void ZCombatInterface::DrawNPCName(MDrawContext* pDC)
{
	for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
		itor != ZGetObjectManager()->end(); ++itor)
	{
		rvector pos, screen_pos;
		ZObject* pObject = (*itor).second;
		if (!pObject->IsVisible()) continue;
		if (pObject->IsDie()) continue;
		if (!pObject->IsNPC()) continue;

		ZActor *pActor = (ZActor*)pObject;
		//		if(!pActor->CheckFlag(AF_MY_CONTROL)) continue;


		pos = pObject->GetPosition();
		RVisualMesh* pVMesh = pObject->m_pVMesh;
		RealSpace2::rboundingbox box;

		if (pVMesh == NULL) continue;

		box.vmax = pos + rvector(50.f, 50.f, 190.f);
		box.vmin = pos + rvector(-50.f, -50.f, 0.f);

		if (isInViewFrustum(&box, RGetViewFrustum()))
		{
			screen_pos = RGetTransformCoord(pObject->GetPosition() + rvector(0, 0, 100.f));

			MFont *pFont = NULL;
			pFont = pActor->CheckFlag(AF_MY_CONTROL) ? MFontManager::Get("FONTa12_O1Blr") : MFontManager::Get("FONTa12_O1Red");
			pDC->SetColor(MCOLOR(0xFF00FF00));
			pDC->SetBitmap(NULL);
			pDC->SetFont(pFont);

			int x = screen_pos.x - pDC->GetFont()->GetWidth(pActor->m_szOwner) / 2;
			pDC->Text(x, screen_pos.y - 12, pActor->m_szOwner);

			// 마지막 정보를 받은시간이 이상하면 찍어준다
			float fElapsedTime = ZGetGame()->GetTime() - pActor->m_fLastBasicInfo;
			if (!pActor->CheckFlag(AF_MY_CONTROL) && fElapsedTime>.2f) {
				int y = screen_pos.y;
				y += pFont->GetHeight();
				char temp[256];
				sprintf(temp, "%2.2f", fElapsedTime);
				x = screen_pos.x - pDC->GetFont()->GetWidth(temp) / 2;
				pDC->Text(x, y - 12, temp);
			}
		}
	}
}

void ZCombatInterface::DrawTDMScore(MDrawContext* pDC)
{
	int nBlueKills = ZGetGame()->GetMatch()->GetTeamKills(MMT_BLUE);
	int nRedKills = ZGetGame()->GetMatch()->GetTeamKills(MMT_RED);
	int nTargetKills = ZGetGameClient()->GetMatchStageSetting()->GetRoundMax();


	ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Blue");
	if (pBmNumLabel)
		pBmNumLabel->SetNumber(nBlueKills);

	pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Red");
	if (pBmNumLabel)
		pBmNumLabel->SetNumber(nRedKills);

	pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Max");
	pBmNumLabel->SetNumber(nTargetKills);


	MWidget* pRed = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_RedWin");
	MWidget* pBlue = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_BlueWin");
	if (pRed && pBlue)
	{
		int nTime[] = { 1, 1400, 1400, 900, 900, 200 };
		int nDiff = min(abs(nBlueKills - nRedKills), 5);
		int nCurrTime = timeGetTime() % nTime[nDiff];

		if (nDiff == 0)
		{
			pRed->Show(false);
			pBlue->Show(false);

			return;
		}
		if ((nDiff == 1) || (nDiff == 2) || (nDiff >= 5))
		{
			if (nCurrTime > 100)
			{
				pRed->Show(false);
				pBlue->Show(false);

				return;
			}
		}
		else if ((nDiff == 3) || (nDiff == 4))
		{
			if (((nCurrTime > 100) && (nCurrTime < 200)) || (nCurrTime > 300))
			{
				pRed->Show(false);
				pBlue->Show(false);

				return;
			}
		}


		if (nRedKills > nBlueKills)
		{
			pRed->Show(true);
			pBlue->Show(false);
		}
		else if (nRedKills < nBlueKills)
		{
			pRed->Show(false);
			pBlue->Show(true);
		}
		else
		{
			pRed->Show(false);
			pBlue->Show(false);
		}
	}
}

void ZCombatInterface::UpdateNetworkAlive(MDrawContext* pDC)
{
	DWORD dw;
	BOOL bIsNetworkAlive = IsNetworkAlive(&dw);
	if (!bIsNetworkAlive)
	{
		m_dAbuseHandicapTick = timeGetTime() + 2000;
		m_bNetworkAlive = false;
	}
	if (bIsNetworkAlive && timeGetTime() > m_dAbuseHandicapTick)
	{
		m_bNetworkAlive = true;
	}
	return;
}

void ZCombatInterface::OnDraw(MDrawContext* pDC)
{
#ifdef LOCALE_KOREA
	if (timeGetTime() - m_dLastTimeTick > 500)
	{
		UpdateNetworkAlive(pDC);
		m_dLastTimeTick = timeGetTime();
	}
#endif // LOCALE_KOREA

	if (m_bShowResult)	// 결과 화면
		return;

	bool bDrawAllPlayerName = false;

	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED &&
		ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED)

		if (ZGetGame()->m_pMyCharacter->IsAdminHide()
			&& MEvent::GetAltState() && ZGetCamera()->GetLookMode() != ZCAMERA_MINIMAP)
			bDrawAllPlayerName = true;
	//Name ESP
	if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isESP == 1)
		bDrawAllPlayerName = true;

	//BAR HP/AP
	if (ZGetGame()->m_pMyCharacter->GetStatus().Ref().isBAR == 1)
		pDC->SetColor(MCOLOR(0x900000FF)); //default full (blue)
	float nHP = ZGetGame()->m_pMyCharacter->GetHP() / ZGetGame()->m_pMyCharacter->GetMaxHP();
	if ((nHP < 1) && (nHP >= 0.5))
		pDC->SetColor(MCOLOR(0x9049E4FC)); // light blue
	if ((nHP < 0.5) && (nHP >= 0.25))
		pDC->SetColor(MCOLOR(0x90FFFF00)); // yelow
	if (nHP < 0.25)
		pDC->SetColor(MCOLOR(0x90FF0000)); //red;

	if (ZGetCamera()->GetLookMode() == ZCAMERA_FREELOOK || bDrawAllPlayerName)
		DrawAllPlayerName(pDC);
	else
	{
		if (!ZGetGameInterface()->IsMiniMapEnable())
		{
			if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
				DrawSpyName(pDC);
			else
				DrawFriendName(pDC);

			DrawEnemyName(pDC);
		}
		if (IsShowUI() && !m_bSkipUIDrawByRule)
		{
			if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_INFECTED
				&& (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY || ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH))
			{
				static int nFadeOpacity = 150;
				static bool bUp = true;
				static int dwLastModTime = 0;
				if ((!m_Observer.IsVisible() && ZGetGame()->m_pMyCharacter->m_bInfected) || (m_Observer.IsVisible() && m_Observer.GetTargetCharacter()->m_bInfected))
				{
					// Resolution change fix
					MBitmap* pBitmapSelection = (RGetIsWidthScreen() || RGetIsLongScreen()) ? m_pInfectedWidescreenOverlay : m_pInfectedOverlay;

					if (pBitmapSelection)
					{
						DWORD elapsed = timeGetTime() - dwLastModTime;

						pDC->SetEffect(MDE_ADD); // the magic trick to ignore black for alpha
						pDC->SetOpacity(nFadeOpacity); // 75 percent of 0xFF (255)
						pDC->SetBitmap(pBitmapSelection);
						pDC->Draw(0, 0, RGetScreenWidth(), RGetScreenHeight());
						pDC->SetOpacity(255); // Reset opacity
						pDC->SetBitmap(NULL); // Reset bitmap
						pDC->SetEffect(MDE_NORMAL);

						if (bUp)
						{
							if (nFadeOpacity == 255)
								bUp = false;

							if (elapsed > 300)
							{
								dwLastModTime = timeGetTime();
								nFadeOpacity++;
							}
						}
						else
						{
							if (nFadeOpacity == 150)
								bUp = true;

							if (elapsed > 300)
							{
								dwLastModTime = timeGetTime();
								nFadeOpacity--;
							}
						}
					}
				}
			}
		}
		//DrawCharacterIcons(pDC);
		//DrawKickPlayerList(pDC);
		GetVoteInterface()->DrawVoteTargetlist(pDC);
		GetVoteInterface()->DrawVoteMessage(pDC);

		// 퀘스트시 보스게이지, 콤보 에니메이션, K.O 이미지
		ZGetScreenEffectManager()->Draw();
#ifdef _PAUSE
		if (ZGetGame()->m_likeBoss)
		{
			char paused[128];

			MFont* pFont = GetGameFont();
			pFont = MFontManager::Get("FONTa10_O2Wht");
			pDC->SetFont(pFont);
			pDC->SetColor(MCOLOR(0xFFFFFFFF));

			sprintf(paused, "Game Paused // Juego Pausado // Jogo Pausado");
			TextRelative(pDC, 400.f / 800.f, 300.f / 600.f, paused);
		}
#endif
		if (IsShowUI())				// 모든 UI 감추기... by kam 20081020
		{
			// 채팅 인풋 창을 그린다.
			m_Chat.OnDraw(pDC);

			if (!m_bSkipUIDrawByRule)
			{
				m_pWeaponScreenEffect->Draw(pDC);

				// EXTRA Work
				ZGetScreenEffectManager()->DrawMyHPPanal(pDC);		// 나의 상태창(HP게이지바)			
				ZGameClient* pGameClient = ZGetGameClient();

				if (pGameClient->GetExtra() && ZIsActionKeyPressed(ZACTION_SCORE) == false)
				{
					ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
					if (pCharacter && !pCharacter->IsDie())
					{
						MFont* pFont = GetGameFont();
						pDC->SetFont(pFont);
						pDC->SetColor(MCOLOR(0xFFFFFFFF));
						pDC->SetFont(MFontManager::Get("FONTa6_O2Wht"));
						char szMsg[220];
						{
							//JorkCode: EXP 
							sprintf(szMsg, "%d%%", ZGetMyInfo()->GetLevelPercent());
							pDC->SetColor(MCOLOR(0xFFFFFFFF));
							TextRelative(pDC, 245.f / 800.f, 71.f / 600.f, szMsg);

							sprintf(szMsg, "HP %i", (int)pCharacter->GetHP(), (int)pCharacter->GetMaxHP());
						//	pDC->SetColor(MCOLOR(0xFFFFFFFF));
						//	TextRelative(pDC, 240.f / 800.f, 28.f / 600.f, szMsg);
							TextRelative(pDC, 100.f / 800.f, 23.f / 600.f, szMsg);

							sprintf(szMsg, "AP %i", (int)pCharacter->GetAP(), (int)pCharacter->GetMaxAP());
						//	pDC->SetColor(MCOLOR(0xFFFFFFFF));
							//TextRelative(pDC, 240.f / 800.f, 48.f / 600.f, szMsg);


							TextRelative(pDC, 100.f/800.f, 50.f/600.f, szMsg);
							/*sprintf(szMsg, "FPS : %i", (int)g_fFPS);
							pDC->SetColor(MCOLOR(0xFFFFFFFF));
							TextRelative(pDC, 10.f / 800.f, 90.f / 600.f, szMsg);*/

							sprintf(szMsg, "Total Damage: Taken: %d, Given: %d", pCharacter->GetStatus().Ref().nTakenDamage, pCharacter->GetStatus().Ref().nGivenDamage);
							pDC->SetColor(MCOLOR(0xFFFFFFFF));
							TextRelative(pDC, 10.f / 800.f, 105.f / 600.f, szMsg);
							sprintf(szMsg, "Damage: Taken: %d, Given: %d", pCharacter->GetStatus().Ref().nRoundTakenDamage, pCharacter->GetStatus().Ref().nRoundGivenDamage);
							pDC->SetColor(MCOLOR(0xFFFFFFFF));
							TextRelative(pDC, 10.f / 800.f, 120.f / 600.f, szMsg);
							// Add By Jorklenis2
							/*sprintf(szMsg, "Shots: Fired: %d, Hitted: %d, Missed: %d, Percentage: %d", pCharacter->GetStatus().Ref().nshotsfired, pCharacter->GetStatus().Ref().nshotshit, pCharacter->GetStatus().Ref().nshotsmissed);
							TextRelative(pDC, 10.f / 800.f, 135.f / 600.f, szMsg);*/
#ifdef	_EVENTCHANNEL
							Alive(pDC); //Count Player Room Event
#endif
							if (ZGetMyInfo()->IsAdminGrade() && ZGetGameClient()->ShowLocalDamage)
							{
								sprintf(szMsg, "Local Damage Sent: %d", pCharacter->GetStatus().Ref().LocalSentDamage);
								pDC->SetColor(MCOLOR(0xFFFFFFFF));
								TextRelative(pDC, 10.f / 800.f, 135.f / 600.f, szMsg);
							}

						}
						//Banderas
#ifdef _COUNTRYFLAG
						/*GEOID myGEO = GetUserGeoID(GEOCLASS_NATION);
						ZPostSendCountryCode(pCharacter->GetProperty()->GetName(), myGEO);*/
#endif
					}
					if (pGameClient->IsLadderGame())
					{
						ZCharacter* pCharacter = NULL;
						ZCharacter* pMyCharacter = ZGetGame()->m_pMyCharacter;
						ZCharacterManager* pCharacterManager = ZGetCharacterManager();
						for (ZCharacterManager::iterator i = pCharacterManager->begin(); i != pCharacterManager->end(); ++i)
						{
							ZCharacter* z = i->second;

							if (z->GetUID() != pGameClient->GetPlayerUID() && z->GetTeamID() == pMyCharacter->GetTeamID())
							{
								pCharacter = z;
								break;
							}
						}

						if (pCharacter && !pCharacter->IsDie() && !ZGetGameClient()->IsDuelTournamentGame())
						{
							MFont* pFont = MFontManager::Get("FONTa6_O2Wht");
							pDC->SetFont(pFont);
							pDC->SetColor(MCOLOR(0xFFFFFFFF));

							char szMsg[128] = { 0, };

							sprintf(szMsg, "HP : %i / %i", (int)pCharacter->GetHP(), (int)pCharacter->GetMaxHP());
							TextRelative(pDC, 100.f / 500.f, 23.f / 600.f, szMsg);

							sprintf(szMsg, "AP : %i / %i", (int)pCharacter->GetAP(), (int)pCharacter->GetMaxAP());
							TextRelative(pDC, 100.f / 500.f, 50.f / 600.f, szMsg);
						}
					}
				}
				ZGetScreenEffectManager()->DrawMyWeaponImage();		// 나의 무기 타입 이미지
				ZGetScreenEffectManager()->DrawMyBuffImage();		// 나의 버프 타입 이미지

				// 퀘스트모드일때는 퀘스트 화면을 보여준다.
				if (m_pQuestScreen) m_pQuestScreen->OnDraw(pDC);
				{
					DrawSoloSpawnTimeMessage(pDC);
				}
#ifdef _MESSAGENBATTLE
				if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DROPMAGIC)
				{
				   DrawSoloSpawnTimeMessageBattle(pDC);
				}
#endif
				DrawPont(pDC);			// 폰트... 			
				DrawMyNamePont(pDC);	// 내 캐릭터명(레벨포함)			
				DrawMyWeaponPont(pDC);	// 내 무기명, 탄알수

#ifdef _GUNGAME
				if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_GUNMODE)
				{
					char buffer[256];
					sprintf(buffer, "[Level %d / %d ]", m_nGunLevel, ZGetGame()->GetMatch()->GetRoundCount());
					TextRelative(pDC, 660.f / 800.f, 480.f / 600.f, buffer);
				}
#endif
				if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DROPMAGIC)
				{
					ZRuleDropGunGame* pRule = (ZRuleDropGunGame*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
					pRule->OnDrawDropGunGame(pDC);
				}
				// 크로스헤어
				if (ZGetGameInterface()->GetCamera()->GetLookMode() == ZCAMERA_DEFAULT)
					m_CrossHair.Draw(pDC);

				//Camara Counter
				if ((ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_CLASSIC_TEAM))
					ZGetGameInterface()->GetCamera()->m_fDist = 5.0f;

				if ((ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_CLASSIC_SOLO))
					ZGetGameInterface()->GetCamera()->m_fDist = 100.0f;
			}
			if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_INFECTED)
			{
				ZBmNumLabel* pBmNumLabel = NULL;
				MPOINT point;

				pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("TDM_Score_Max");
				if (pBmNumLabel)
				{
					point = pBmNumLabel->GetPosition();

					if (RGetIsWidthScreen() || RGetIsLongScreen())
					{
						point.x += 28;
						point.y += 50;
					}
					else
					{
						point.x += 14;
						point.y += 50;
					}

					if (g_bShowCTFMsg && GetTickCount() - g_dwCTFMsgShowTime < 5000)
					{
						point = pBmNumLabel->GetPosition();

						if (RGetIsWidthScreen() || RGetIsLongScreen())
						{
							point.x -= 90;
							point.y += 145;
						}
						else
						{
							point.x -= 100;
							point.y += 145;
						}

						pDC->SetFont(MFontManager::Get("FONTa10_O2Wht"));
						pDC->SetColor(MCOLOR(0xFF00FFDC));
						pDC->Text(point, g_szCTFMsg);
					}
				}
			}
			DrawBuffStatus(pDC);

			// 스코어
			DrawScore(pDC);

#ifdef _DUELTOURNAMENT
			if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
			{
				((ZRuleDuelTournament*)ZGetGame()->GetMatch()->GetRule())->OnDraw(pDC);
			}
#endif
		}

		//그리는 순서 때문에
		if (ZGetGame()) {
			ZGetGame()->m_HelpScreen.DrawHelpScreen();
		}

		// 동영상 캡쳐...2008.10.20
		if (ZGetGameInterface()->GetBandiCapturer() != NULL)
			ZGetGameInterface()->GetBandiCapturer()->DrawCapture(pDC);

		if (ZGetGame() && ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
			OnSpyDraw(pDC);
	}

}
void HpApBarRelative(MDrawContext* pDC, float x, float y, float min, float max, MCOLOR Colour, int cy)
{
	int width = 50;
	pDC->SetColor( MCOLOR( 0x30000000));
	pDC->FillRectangleW(x, y, width, cy);
	int nValue = (width * (min / max));
	pDC->SetColor( MCOLOR(Colour));
	pDC->FillRectangleW( x, y+1, -width+nValue, cy);

#ifndef _EXTRESOLUTION
	if (RGetIsWidthScreen())
		x = (x * 800 + 80) / 960.f;
#endif
	/*int screenx = x*MGetWorkspaceWidth();
	pDC->SetColor(MCOLOR(0x30000000));
	pDC->FillRectangleW(screenx, y*MGetWorkspaceHeight(), 0.08*MGetWorkspaceWidth(), cy);
	float nValue = (0.08 * (min / max));
	pDC->SetColor(Colour);
	pDC->FillRectangleW(screenx, y*MGetWorkspaceHeight() + 1, nValue*MGetWorkspaceWidth(), cy);*/

}
void ZCombatInterface::DrawClanMembersTextInfo(MDrawContext* pDC)
{
	if (ZGetConfiguration()->GetExtra()->bHPAPScoreboard)
	{
		ZCharacter* mCharacter = GetTargetCharacter();
		if (mCharacter == NULL || mCharacter->IsObserverTarget() || ZGetGameClient()->IsLadderGame() == false && ZGetGameClient()->IsPlayerWars() == false)
			return;
		MFont *pFont = GetGameFont();
		pDC->SetFont(pFont);
		float Width = 260.f, HeightMod = 0.f;
		int Counter = 0;
		for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
		{
			ZCharacter* pCharacter = (*itor).second;
			if (mCharacter->GetUID() == pCharacter->GetUID()) continue;
			if (pCharacter && !pCharacter->IsDie() && mCharacter->IsTeam(pCharacter))
			{
				pDC->SetColor(MCOLOR(0xFFFFFFFF));
				TextRelative(pDC, Width / 800.f, (HeightMod + 6.f) / 600.f, pCharacter->GetCharInfo()->szName);
				HpApBarRelative(pDC, Width / 800.f, (HeightMod + 18.f) / 600.f, pCharacter->GetHP(), pCharacter->GetMaxHP(), MCOLOR(0xFFFF0000), 7);
				HpApBarRelative(pDC, Width / 800.f, (HeightMod + 24.f) / 600.f, pCharacter->GetAP(), pCharacter->GetMaxAP(), MCOLOR(0xFF0087FF), 3);
				Width += 65.f;
				Counter++;
				if (Counter == 4)
				{
					Width = 260.f;
					HeightMod += 35.f;
					Counter = 0;
				}
			}
		}
	}
}
void ZCombatInterface::DrawMyNamePont(MDrawContext* pDC)
{
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
		return;

	ZCharacter* pCharacter = GetTargetCharacter();
	if (pCharacter == NULL) return;
	if (m_Observer.IsVisible())
		return;

	MFont *pFont = GetGameFont();

	pDC->SetFont(pFont);
	pDC->SetColor(MCOLOR(0xFFFFFFFF));

	char buffer[256];

	sprintf(buffer, "%d  %s", pCharacter->GetProperty()->nLevel, pCharacter->GetProperty()->GetName());

	if ((ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUEL) || (!pCharacter->IsObserverTarget()))
	{
		float fCenterVert = 0.018f - (float)pFont->GetHeight() / (float)RGetScreenHeight() / 2;
		TextRelative(pDC, 100.f / 800.f, fCenterVert, buffer);
	}
}
void ZCombatInterface::DrawMyWeaponPont(MDrawContext* pDC)
{
	ZCharacter* pCharacter = GetTargetCharacter();
	if (pCharacter == NULL) return;
	if (m_Observer.IsVisible()) return;

	MFont *pFont = GetGameFont();

	pDC->SetFont(pFont);
	pDC->SetColor(MCOLOR(0xFFFFFFFF));

	char buffer[256];

	// 무기 이름
	TextRelative(pDC, 660.f / 800.f, 510.f / 600.f, m_szItemName);

	// 탄알수
	MMatchCharItemParts nParts = pCharacter->GetItems()->GetSelectedWeaponParts();
	if (nParts != MMCIP_MELEE && nParts < MMCIP_END)
	{
		// melee일때는 탄알수 표시를 하지 않는다.
		sprintf(buffer, "%d / %d", m_nBulletCurrMagazine, m_nBulletSpare);
		TextRelative(pDC, 720.f / 800.f, 585.f / 600.f, buffer);
	}
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DROPMAGIC)
	{
		sprintf(buffer, "[ Kills: %d / %d ]", pCharacter->GetKils(), ZGetGame()->GetMatch()->GetRoundCount());
		TextRelative(pDC, 660.f / 800.f, 490.f / 600.f, buffer);
	}
}

void ZCombatInterface::DrawPont(MDrawContext* pDC)
{
	ZCharacter* pCharacter = GetTargetCharacter();
	if (pCharacter == NULL) return;

	if (m_Observer.IsVisible())
		return;

	// 듀얼 모드일때(옵져버 모드 아님)
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
	{
		char	charName[3][32];
		charName[0][0] = charName[1][0] = charName[2][0] = 0;
		float fRx = (float)MGetWorkspaceWidth() / 800.0f;
		float fRy = (float)MGetWorkspaceHeight() / 600.0f;

		MFont *pFont = MFontManager::Get("FONTa10_O2Wht");
		if (pFont == NULL)
			//_ASSERT(0);
			pDC->SetFont(pFont);
		pDC->SetColor(MCOLOR(0xFFFFFFFF));

		bool bIsChallengerDie = false;
		int nMyChar = -1;

		ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		if (pDuel)
		{
			for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
			{
				ZCharacter* pCharacter = (*itor).second;

				// Player
				if (pCharacter->GetUID() == pDuel->QInfo.m_uidChampion)
				{
					if (ZGetMyUID() == pDuel->QInfo.m_uidChampion)
					{
						// Draw victory
						ZGetCombatInterface()->DrawVictory(pDC, 210, 86, pDuel->QInfo.m_nVictory);
					}
					else
					{
						sprintf(charName[0], "%s%d  %s", ZMsg(MSG_CHARINFO_LEVELMARKER), pCharacter->GetProperty()->nLevel, pCharacter->GetUserName());

						if ((ZGetMyUID() == pDuel->QInfo.m_uidChampion) || (ZGetMyUID() == pDuel->QInfo.m_uidChallenger))
						{
							// Draw victory
							int nTextWidth = pFont->GetWidth(charName[0]);
							int nWidth = ZGetCombatInterface()->DrawVictory(pDC, 162, 300, pDuel->QInfo.m_nVictory, true);
							ZGetCombatInterface()->DrawVictory(pDC, 43 + nTextWidth + nWidth, 157, pDuel->QInfo.m_nVictory);
						}
					}
				}

				else if (pCharacter->GetUID() == pDuel->QInfo.m_uidChallenger)
				{
					if (ZGetMyUID() != pDuel->QInfo.m_uidChallenger)
						sprintf(charName[0], "%s%d  %s", ZMsg(MSG_CHARINFO_LEVELMARKER), pCharacter->GetProperty()->nLevel, pCharacter->GetUserName());

					bIsChallengerDie = pCharacter->IsDie();
				}

				// Waiting 1
				else if (pCharacter->GetUID() == pDuel->QInfo.m_WaitQueue[0])
					sprintf(charName[1], "%s%d  %s", ZMsg(MSG_CHARINFO_LEVELMARKER), pCharacter->GetProperty()->nLevel, pCharacter->GetUserName());

				// Waiting 2
				else if (pCharacter->GetUID() == pDuel->QInfo.m_WaitQueue[1])
					sprintf(charName[2], "%s%d  %s", ZMsg(MSG_CHARINFO_LEVELMARKER), pCharacter->GetProperty()->nLevel, pCharacter->GetUserName());
			}
		}

		MBitmap* pBitmap = MBitmapManager::Get("duel-mode.tga");
		if (pBitmap)
		{
			pDC->SetBitmap(pBitmap);

			int nIcon = 50.0f*fRx;
			pDC->Draw(8.0f*fRx, 153.0f*fRy, nIcon, nIcon);
		}

		pBitmap = MBitmapManager::Get("icon_play.tga");
		if (pBitmap && (charName[1][0] != 0))
		{
			pDC->SetBitmap(pBitmap);

			int nIcon = 22.0f*fRx;
			pDC->Draw(60.0f*fRx, 175.0f*fRy, nIcon, nIcon);
			pDC->Draw(53.0f*fRx, 175.0f*fRy, nIcon, nIcon);
		}

		//MCOLOR color;

		int nTime = timeGetTime() % 200;
		if (nTime < 100)
			pDC->SetColor(MCOLOR(0xFFFFFF00));
		else
			pDC->SetColor(MCOLOR(0xFFA0A0A0));

		if (bIsChallengerDie)
			pDC->SetColor(MCOLOR(0xFF808080));

		int nPosY = 160.0f*fRy;
		pDC->Text(60.0f*fRx, nPosY, charName[0]);

		pDC->SetColor(MCOLOR(0xFF808080));
		nPosY += 20;
		pDC->Text(80.0f*fRx, nPosY, charName[1]);
		nPosY += 15;
		//pDC->Text( 80.0f*fRx, nPosY, charName[ 2]);
	}
}

void ZCombatInterface::DrawScore(MDrawContext* pDC)
{
	m_bDrawScoreBoard = false;
	if (ZIsActionKeyPressed(ZACTION_SCORE) == true) {
		if (m_Chat.IsShow() == false)
			m_bDrawScoreBoard = true;
	}
	else if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PREPARE) {
		int cur_round = ZGetGame()->GetMatch()->GetCurrRound();
		//		int max_round = ZGetGame()->GetMatch()->GetRoundCount();

		if (cur_round == 0) {
			m_bDrawScoreBoard = true;
		}
	}

	if (ZGetGameTypeManager()->IsTeamExtremeGame(ZGetGame()->GetMatch()->GetMatchType()))
	{
		DrawTDMScore(pDC);
	}

	if (m_bDrawScoreBoard) {
		// _DUELTOURNAMENT
		if (ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT)
			DrawScoreBoard(pDC);
		else
			DrawDuelTournamentScoreBoard(pDC);
	}
}

void ZCombatInterface::DrawBuffStatus(MDrawContext* pDC)
{
	//버프정보임시주석 
	/*	if(ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT)
	{
	ZCharacter* pCharacter = GetTargetCharacter();
	if (pCharacter == NULL) return;
	if (m_Observer.IsVisible()) return;

	ZCharacterBuff *pCharBuff = pCharacter->GetCharacterBuff();
	if( pCharBuff == NULL ) return;

	MFont *pFont = GetGameFont();

	pDC->SetFont(pFont);
	pDC->SetColor(MCOLOR(0xFFFFFFFF));

	int nMinutes, nSeconds;
	char szMsg[128] = { 0, };
	for(int i = 0; i < MAX_CHARACTER_SHORT_BUFF_COUNT; i++){
	ZShortBuff* pShortBuff = pCharBuff->GetShortBuff(i);
	nMinutes = pShortBuff->GetBuffPeriodRemainder(timeGetTime()) / 1000 / 60;
	nSeconds = pShortBuff->GetBuffPeriodRemainder(timeGetTime()) / 1000 - (60 * nMinutes);
	sprintf(szMsg, "%d:%d", nMinutes, nSeconds);

	if( nMinutes != 0 || nSeconds != 0 ) {
	TextRelative(pDC, (100.f + (i * 50)) / 800.f, 90.f / 600.f, szMsg);
	}
	}
	}
	else
	{
	}
	*/
}

void ZCombatInterface::DrawFinish()
{
	//	DrawResultBoard(pDC);
	// Finish 후에 일정 시간이 경과하면 결과 화면 보여줌
	if (!m_bShowResult && IsFinish())
	{
		// 배경 음악 볼륨을 서서히 낮춤
		float fVolume;
		DWORD dwClock = timeGetTime() - m_nReserveFinishTime;
		if (dwClock > 4000)
			fVolume = 0.0f;
		else
			fVolume = (float)(4000 - dwClock) / 4000.0f * m_fOrgMusicVolume;

		ZApplication::GetSoundEngine()->SetMusicVolume(fVolume);


		if (timeGetTime() >= m_nReservedOutTime)
		{
			MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Option");
			if (pWidget)
				pWidget->Show(false);
			pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatMenuFrame");
			if (pWidget)
				pWidget->Show(false);
			MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatChatInput");
			if (pLabel)
				pLabel->SetText("");
			ZGetCombatInterface()->EnableInputChat(false);



			// 만약 퀘스트 실패면 스테이지로 바로 넘어간다.
			if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType()))
			{
				if (!ZGetQuest()->IsQuestComplete())
				{
					ZChangeGameState(GUNZ_STAGE);
					m_bShowResult = true;

					return;
				}
			}

			// 배경 화면에 필요한 정보를 업데이트 한다
			GetResultInfo();

			pWidget = ZGetGameInterface()->GetIDLResource()->FindWidget("GameResult");
			if (pWidget)
				pWidget->Show(true, true);


			// 사운드를 출력한다
			ZApplication::GetSoundEngine()->SetMusicVolume(m_fOrgMusicVolume);
#ifdef _BIRDSOUND
			ZApplication::GetSoundEngine()->OpenMusic(BGMID_FIN);
			ZApplication::GetSoundEngine()->PlayMusic(false);
#else
			ZApplication::GetSoundEngine()->OpenMusic(BGMID_FIN, ZApplication::GetFileSystem());
			ZApplication::GetSoundEngine()->PlayMusic(false);
#endif
			m_nReservedOutTime = timeGetTime() + 15000;
			m_bShowResult = true;

#ifdef LOCALE_NHNUSA
			GetNHNUSAReport().ReportCompleteGameResult();
#endif

		}
	}
}


int ZCombatInterface::DrawVictory(MDrawContext* pDC, int x, int y, int nWinCount, bool bGetWidth)
{
	//	nWinCount = 99;										// for test

	// Get total width
	if (bGetWidth)
	{
		int nWidth = 0;

		int nNum = nWinCount % 5;
		if (nNum)
			nWidth += 17.0f + 17.0f * 0.63f * (nNum - 1);

		if ((nWinCount % 10) >= 5)
			nWidth += 19.0f * 0.2f + 19.0f * 1.1f;
		else
			nWidth += 19.0f * 0.5f;

		nNum = nWinCount / 10;
		if (nNum)
			nWidth += 22.0f + 22.0f * 0.5f * (nNum - 1);

		return nWidth;
	}


	// Get image
	MBitmap* pBitmap = MBitmapManager::Get("killstone.tga");
	if (!pBitmap)
		return 0;

	pDC->SetBitmap(pBitmap);

	// Get screen
	float fRx = (float)MGetWorkspaceWidth() / 800.0f;
	float fRy = (float)MGetWorkspaceHeight() / 600.0f;


	// Get Image Number
	int nImage = ((timeGetTime() / 100) % 20);
	if (nImage > 10)
		nImage = 0;
	nImage *= 32;
	nImage = ((timeGetTime() / 100) % 20);
	if (nImage > 10)
		nImage = 0;
	nImage *= 32;

	// Draw
	int nPosX = x * fRx;
	int nPosY = y * fRy;
	int nSize = 17.0f * fRx;								// 1 단위
	for (int i = 0; i < (nWinCount % 5); i++)
	{
		pDC->Draw(nPosX, nPosY, nSize, nSize, nImage, 0, 32, 32);
		nPosX -= nSize * 0.63f;
	}

	nSize = 19.0f * fRx;
	nPosY = (y - 2) * fRy;
	if ((nWinCount % 10) >= 5)								// 5 단위
	{
		nPosX -= nSize * 0.2f;
		pDC->Draw(nPosX, nPosY, nSize, nSize, nImage, 64, 32, 32);
		nPosX -= nSize * 1.1f;
	}
	else
		nPosX -= nSize * 0.5f;

	nSize = 22.0f * fRx;									// 10 단위
	nPosY = (y - 5) * fRy;
	for (int i = 0; i < (nWinCount / 10); i++)
	{
		pDC->Draw(nPosX, nPosY, nSize, nSize, nImage, 32, 32, 32);
		nPosX -= nSize * 0.5f;
	}

	// 연승 숫자 표시
	/*	if ( nWinCount >= 10)
	{
	pFont = MFontManager::Get( "FONTa9b");
	pDC->SetFont( pFont);
	pDC->SetColor( MCOLOR(0xFFFFFFFF));
	char szVictory[ 16];
	sprintf( szVictory, "%d", nWinCount);
	TextRelative( pDC, 0.195f, 0.01f, szVictory, true);
	}
	*/
	return 0;
}


// TODO : 이게 필요 없는듯.
// 그리는 순서때문에 만든 펑션
void ZCombatInterface::OnDrawCustom(MDrawContext* pDC)
{
	// 결과 화면 보인 이후에 일정 시간 후 자동 종료한다
	if (m_bShowResult)
	{
		// 숫자를 카운터한다.
		if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType()))
		{
			int nNumCount = (timeGetTime() - (m_nReservedOutTime - 15000)) * 3.6418424f;		// 3.6418424f는 gain 값이다.
			ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetPlusXP");
			if (pBmNumLabel)
			{
				if (nNumCount < ZGetQuest()->GetRewardXP())
					pBmNumLabel->SetNumber(nNumCount, false);
				else
					pBmNumLabel->SetNumber(ZGetQuest()->GetRewardXP(), false);
			}
			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetMinusXP");
			if (pBmNumLabel)
				pBmNumLabel->SetNumber(0, false);
			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetTotalXP");
			if (pBmNumLabel)
			{
				if (nNumCount < ZGetQuest()->GetRewardXP())
					pBmNumLabel->SetNumber(nNumCount, false);
				else
					pBmNumLabel->SetNumber(ZGetQuest()->GetRewardXP(), false);
			}
			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetBounty");
			if (pBmNumLabel)
			{
				if (nNumCount < ZGetQuest()->GetRewardBP())
					pBmNumLabel->SetNumber(nNumCount, false);
				else
					pBmNumLabel->SetNumber(ZGetQuest()->GetRewardBP(), false);
			}
		}
		else if (ZGetGameTypeManager()->IsSurvivalOnly(ZGetGame()->GetMatch()->GetMatchType()))
		{
			int nNumCount = (timeGetTime() - (m_nReservedOutTime - 15000)) * 3.6418424f;		// 3.6418424f는 gain 값이다.

			ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetReachedRound");
			if (pBmNumLabel)
				pBmNumLabel->SetNumber(static_cast< ZSurvival* >(ZGetQuest())->GetReachedRound(), false);

			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetPoint");
			if (pBmNumLabel)
			{
				if (nNumCount < ZGetQuest()->GetRewardXP())
					pBmNumLabel->SetNumber(nNumCount, false);
				else
					pBmNumLabel->SetNumber(static_cast< ZSurvival* >(ZGetQuest())->GetPoint(), false);
			}

			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetXP");
			if (pBmNumLabel)
			{
				if (nNumCount < ZGetQuest()->GetRewardXP())
					pBmNumLabel->SetNumber(nNumCount, false);
				else
					pBmNumLabel->SetNumber(ZGetQuest()->GetRewardXP(), false);
			}

			pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetBounty");
			if (pBmNumLabel)
			{
				if (nNumCount < ZGetQuest()->GetRewardBP())
					pBmNumLabel->SetNumber(nNumCount, false);
				else
					pBmNumLabel->SetNumber(ZGetQuest()->GetRewardBP(), false);
			}
		}

		// 남은 시간을 표시한다
		MLabel* pLabel = (MLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("GameResult_RemaindTime");
		if (pLabel)
		{
			char szRemaindTime[100];
			sprintf(szRemaindTime, "%d", (m_nReservedOutTime - timeGetTime()) / 1000);
			char szText[100];
			ZTransMsg(szText, MSG_GAME_EXIT_N_MIN_AFTER, 1, szRemaindTime);

			pLabel->SetAlignment(MAM_HCENTER);
			pLabel->SetText(szText);
		}

		return;
	}


	if (m_Observer.IsVisible())
	{
		if (!ZGetGameInterface()->IsMiniMapEnable())
		{
			if (!ZGetGame()->IsReplay() || ZGetGame()->IsShowReplayInfo())
				ZGetScreenEffectManager()->DrawSpectator();
		}

		m_Observer.OnDraw(pDC);
	}

	if (m_bDrawLeaveBattle)
		DrawLeaveBattleTimeMessage(pDC);
}


void ZCombatInterface::DrawSoloSpawnTimeMessage(MDrawContext* pDC)
{
	if (ZGetGame()->m_pMyCharacter->IsAdminHide()) return;

	ZMatch* pMatch = ZGetGame()->GetMatch();
	if (pMatch->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		if (!pMatch->IsWaitForRoundEnd())
		{
			if (ZGetGame()->m_pMyCharacter && ZGetGame()->m_pMyCharacter->IsDie())
			{
				char szMsg[128] = "";
				int nRemainTime = pMatch->GetRemainedSpawnTime();
				if ((nRemainTime > 0) && (nRemainTime <= 5))
				{
					char temp[4];
					sprintf(temp, "%d", nRemainTime);
					ZTransMsg(szMsg, MSG_GAME_WAIT_N_MIN, 1, temp);
				}
				else if ((nRemainTime == 0) && (!ZGetGame()->GetSpawnRequested()))
				{
					sprintf(szMsg, ZMsg(MSG_GAME_CLICK_FIRE));
				}

				MFont *pFont = GetGameFont();
				pDC->SetFont(pFont);
				pDC->SetColor(MCOLOR(0xFFFFFFFF));
				TextRelative(pDC, 400.f / 800.f, 400.f / 600.f, szMsg, true);
			}
		}
	}
}
#ifdef _MESSAGENBATTLE
void ZCombatInterface::DrawSoloSpawnTimeMessageBattle(MDrawContext* pDC)
{
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DROPMAGIC)
	{
	 if (ZGetGame()->m_pMyCharacter->IsAdminHide()) return;

	ZMatch* pMatch = ZGetGame()->GetMatch();
	if (pMatch->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		if (!pMatch->IsWaitForRoundEnd())
		{
			if (ZGetGame()->m_pMyCharacter && ZGetGame()->m_pMyCharacter->IsDie())
			{
				char szMsg[128] = "";
				int nRemainTime = pMatch->GetRemainedSpawnTime();
				if ((nRemainTime > 0) && (nRemainTime <= 9))
				{
					char temp[4];
					sprintf(temp, "%d", nRemainTime);
					ZTransMsg(szMsg, MSG_GAME_WAIT_N_MIN_BATTLE, 1, temp);
				}
				else if ((nRemainTime == 0) && (!ZGetGame()->GetSpawnRequested()))
				{
					sprintf(szMsg, ZMsg(MSG_GAME_CLICK_FIRE_BATTLE));
				}

				MFont *pFont = GetGameFont();
				pDC->SetFont(pFont);
				pDC->SetColor(MCOLOR(0xFFFFFFFF));
				TextRelative(pDC, 400.f / 800.f, 400.f / 600.f, szMsg, true);
			}
		}
	}
}
}
#endif
void ZCombatInterface::DrawLeaveBattleTimeMessage(MDrawContext* pDC)
{
	char szMsg[128] = "";
	//	sprintf(szMsg, "%d 초후에 게임에서 나갑니다", m_nDrawLeaveBattleSeconds);

	char temp[4];
	sprintf(temp, "%d", m_nDrawLeaveBattleSeconds);
	ZTransMsg(szMsg, MSG_GAME_EXIT_N_MIN_AFTER, 1, temp);

	MFont *pFont = GetGameFont();
	pDC->SetFont(pFont);
	pDC->SetColor(MCOLOR(0xFFFFFFFF));
	TextRelative(pDC, 400.f / 800.f, 350.f / 600.f, szMsg, true);
}


bool ZCombatInterface::IsDone()
{
	return false;
}

bool ZCombatInterface::OnEvent(MEvent* pEvent, MListener* pListener)
{
	return false;
}

void ZCombatInterface::Resize(int w, int h)
{
	SetSize(w, h);

}

void ZCombatInterface::ShowMenu(bool bVisible)
{
	if (m_bMenuVisible == bVisible) return;


	m_bMenuVisible = bVisible;
}

void ZCombatInterface::EnableInputChat(bool bInput, bool bTeamChat)
{
	// 채팅창 안보기인 상태일 때에는 입력도 안된다.
	//	if ((!ZGetConfiguration()->GetViewGameChat()) && (bInput)) return;

	m_Chat.EnableInput(bInput, bTeamChat);
}

void ZCombatInterface::OutputChatMsg(const char* szMsg)
{
	m_Chat.OutputChatMsg(szMsg);
}

#ifdef _IGNORECOMMD
void ZCombatInterface::OutputChatMsg(MCOLOR color, const char* szMsg)
{
	m_Chat.OutputChatMsg(color, szMsg);
	if (strstr(szMsg, ":")) {
		char szPlayerName[32];
		sscanf(szMsg, "%s : ", &szPlayerName);
		for (auto i = ignorecmd.begin(); i != ignorecmd.end(); i++) {
			if (i->find(szPlayerName) != string::npos) {
				return;
			}
		}
	}
}
#else
void ZCombatInterface::OutputChatMsg(MCOLOR color, const char* szMsg)
{
	m_Chat.OutputChatMsg(color, szMsg);
}
#endif


void ZCombatInterface::SetItemName(const char* szName)
{
	if (!strcmp(m_szItemName, szName)) return;

	strcpy(m_szItemName, szName);
}


void ZCombatInterface::ShowInfo(bool bVisible)
{
	MWidget* pWidget;
	char szTemp[256];
	for (int i = 0; i < 9; i++)
	{
		sprintf(szTemp, "%s%d", ZIITEM_COMBAT_INFO, i);
		pWidget = m_pIDLResource->FindWidget(szTemp);
		if (pWidget != NULL)
		{
			pWidget->Show(bVisible);
		}
	}
	pWidget = m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATFRAME);
	if (pWidget != NULL)
	{
		pWidget->Show(bVisible);
	}
}
#ifdef _GUNGAME
void ZCombatInterface::GunMode(ZCharacter* pCharacter, const int Level)
{
	//ZGetScreenEffectManager()->AddRoundStart(Level);
	int nItemMelee = 0;
	int nItemPistol = 0;
	int nItemSecun = 0;

	int aleatorio = rand() % 3;
	if (aleatorio == 0)
	{
		nItemMelee = 7;
		nItemPistol = 505005;
		nItemSecun = 5019;
	}
	if (aleatorio == 1)
	{
		nItemMelee = 502013;
		nItemPistol = 504006;
		nItemSecun = 507001;
	}
	if (aleatorio == 2)
	{
		nItemMelee = 18;
		nItemPistol = 506007;
		nItemSecun = 508001; //braker 8
	}

	pCharacter->GetItems()->EquipItem(MMCIP_PRIMARY, nItemPistol);  // Rocket
	pCharacter->ChangeWeapon(MMCIP_PRIMARY);
	pCharacter->ChangeWeapon(MMCIP_PRIMARY);
	pCharacter->GetItems()->EquipItem(MMCIP_SECONDARY, nItemSecun);  // grenade
	pCharacter->ChangeWeapon(MMCIP_SECONDARY);
	pCharacter->GetItems()->EquipItem(MMCIP_MELEE, nItemMelee);  // dagger
	pCharacter->ChangeWeapon(MMCIP_MELEE);
	pCharacter->ChangeWeapon(MMCIP_CUSTOM1);
	pCharacter->ChangeWeapon(MMCIP_CUSTOM2);
	pCharacter->InitItemBullet();

}
#endif
void ZCombatInterface::Update(float fElapsed)
{
	// Finish 후에 일정 시간이 경과하면 결과 화면 보여줌
	DrawFinish();

	// 결과 화면 보는 중이면 일정 시간 후 자동 종료한다
	if (m_bShowResult)
	{
		if (timeGetTime() > m_nReservedOutTime)
		{
			if (ZGetGameClient()->IsLadderGame()|| 
				ZGetGameClient()->IsDuelTournamentGame() || 
				ZGetGameClient()->IsPlayerWars())
				ZChangeGameState(GUNZ_LOBBY);
			else
				ZChangeGameState(GUNZ_STAGE);
		}
	}

	m_fElapsed = fElapsed;

	if (m_bReserveFinish) {
		if ((timeGetTime() - m_nReserveFinishTime) > 1000) {
			OnFinish();
			m_bReserveFinish = false;
		}
	}

	ZCharacter* pCharacter = GetTargetCharacter();
	if (pCharacter == NULL)				return;
	if (!pCharacter->GetInitialized())	return;
	if (ZGetScreenEffectManager() == NULL) return;
	if (pCharacter->GetProperty() == NULL) return;

	float fGauge = 100.f;
	float fCur, fMax;
	/*
	bool bPre = false;

	if(g_pGame&&g_pGame->GetMatch()) {
	//	if(g_pGame&&g_pGame->GetMatch()->GetRoundState()==MMATCH_ROUNDSTATE_PLAY) {
	//	if(g_pGame&&(g_pGame->GetReadyState()==ZGAME_READYSTATE_RUN)) {

	if(bPre)
	*/

	if (ZGetGame() && ZGetGame()->GetMatch())
	{
#ifdef _SRVRPNG
		unsigned long int nNowTime = timeGetTime();
		if ((nNowTime - ZGetGame()->st_nLastTime[2]) > 8000)
		{
			ZGetGame()->st_nLastTime[2] = nNowTime;
			ZPostServerPing(timeGetTime(), 2);
		}
#endif
		fMax = (float)pCharacter->GetMaxHP();
		fCur = (float)pCharacter->GetHP();

		if (fCur != 0.f && fMax != 0.f)	fGauge = fCur / fMax;
		else							fGauge = 0.f;

		if (ZGetGame()->GetMatch()->GetCurrRound() == 0 &&
			ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PREPARE)
			fGauge = 100.f;

		ZGetScreenEffectManager()->SetGauge_HP(fGauge);

		fMax = (float)pCharacter->GetMaxAP();
		fCur = (float)pCharacter->GetAP();

		if (fCur != 0.f && fMax != 0.f)	fGauge = fCur / fMax;
		else							fGauge = 0.f;


		ZGetScreenEffectManager()->SetGauge_AP(fGauge);
	}
	else
	{
		ZGetScreenEffectManager()->SetGauge_HP(fGauge);
		ZGetScreenEffectManager()->SetGauge_AP(fGauge);
	}

	//	ZGetScreenEffectManager()->SetGauge_EXP((float)pCharacter->GetStatus()->fStamina/100.f);//임시로 stamina 로 테스트
	//	ZGetScreenEffectManager()->SetGauge_EXP(100.f);//임시로 stamina 로 테스트

	MMatchWeaponType wtype = MWT_NONE;

	ZItem* pSItem = pCharacter->GetItems()->GetSelectedWeapon();

	MMatchItemDesc* pSelectedItemDesc = NULL;

	if (pSItem) {
		pSelectedItemDesc = pSItem->GetDesc();

		m_nBulletSpare = pSItem->GetBulletSpare();
		m_nBulletCurrMagazine = pSItem->GetBulletCurrMagazine();
	}

	if (pSelectedItemDesc) {
		wtype = pSelectedItemDesc->m_nWeaponType.Ref();
	}

	ZGetScreenEffectManager()->SetWeapon(wtype, pSelectedItemDesc);


	if ((pSelectedItemDesc) && (m_pLastItemDesc != pSelectedItemDesc)) {
		SetItemName(pSelectedItemDesc->m_pMItemName->Ref().m_szItemName);
	}

	UpdateCombo(pCharacter);
#ifdef _GUNGAME
	m_nGunLevel = AddGunLevel();
#endif


	m_Chat.Update();
	m_AdminMsg.Update();


	/*
	if (m_pScoreBoard->IsVisible())
	{
	m_pScoreBoard->Update();
	}
	*/

	if (pCharacter->GetItems()->GetSelectedWeaponParts() == MMCIP_MELEE) {
		ShowCrossHair(false);
	}
	else {
		ShowCrossHair(true);
	}

	GameCheckPickCharacter();

	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
		OnSpyUpdate(fElapsed);

}

bool GetUserInfoUID(MUID uid, MCOLOR& _color, char* sp_name, MMatchUserGradeID& gid);

void ZCombatInterface::SetPickTarget(bool bPick, ZCharacter* pCharacter)
{
	bool bFriend = false;
	if (bPick)
	{
		if (pCharacter == NULL) return;

		if (ZGetGame()->GetMatch()->IsTeamPlay())
		{
			ZCharacter *pTargetCharacter = GetTargetCharacter();
			if (pTargetCharacter && pTargetCharacter->GetTeamID() == pCharacter->GetTeamID())
			{
				bFriend = true;
			}
		}

		if (bFriend == false)
		{
			m_CrossHair.SetState(ZCS_PICKENEMY);
			m_pTargetLabel->SetTextColor(0xffff0000);
		}

		if (pCharacter->IsAdminName())
			m_pTargetLabel->SetTextColor(ZCOLOR_ADMIN_NAME);

		if (!bFriend == true && !pCharacter->IsDie())
		{
			strcpy(m_szTargetName, pCharacter->GetUserName());
#ifdef _DEBUG
			sprintf(m_szTargetName, "%s : %d", pCharacter->GetUserName(), pCharacter->GetHP());
#endif
			m_pTargetLabel->SetText(m_szTargetName);
		}

		int nCrosshairHeight = m_CrossHair.GetHeight();

		int nLen = m_pTargetLabel->GetRect().w;
		m_pTargetLabel->SetPosition(((MGetWorkspaceWidth() - m_pTargetLabel->GetRect().w) / 2), (MGetWorkspaceHeight() / 2) - nCrosshairHeight);
		m_pTargetLabel->SetAlignment(MAM_HCENTER);
	}
	else
	{
		m_CrossHair.SetState(ZCS_NORMAL);
		memset(m_szTargetName, 0, sizeof(m_szTargetName));
		m_pTargetLabel->Show(false);
	}

	m_bPickTarget = bPick;
}

void ZCombatInterface::SetItemImageIndex(int nIndex)
{
	char szTemp[256];
	sprintf(szTemp, "item%02d.png", nIndex);
	BEGIN_WIDGETLIST("CombatItemPic", ZApplication::GetGameInterface()->GetIDLResource(),
		MPicture*, pPicture);

	pPicture->SetBitmap(MBitmapManager::Get(szTemp));

	END_WIDGETLIST();
}
/*
void ZCombatInterface::SetMagazine(int nMagazine)
{
if (m_nMagazine == nMagazine) return;

char szTemp[256];
sprintf(szTemp, "%02d", nMagazine);
BEGIN_WIDGETLIST("CombatMagazine", ZApplication::GetGameInterface()->GetIDLResource(),
MWidget*, pWidget);

pWidget->SetText(szTemp);

END_WIDGETLIST();

m_nMagazine = nMagazine;
}
*/

void ZCombatInterface::UpdateCombo(ZCharacter* pCharacter)
{
	if (pCharacter == NULL) return;

	static int nComboX = -999, nComboY = -999;
	static int nLastCombo = 0;

	int nCurCombo = pCharacter->GetStatus().Ref().nCombo;

	if (nCurCombo != nLastCombo)
	{
		nLastCombo = nCurCombo;
		ZGetScreenEffectManager()->SetCombo(nLastCombo);
	}
	else if (nCurCombo != 0)
	{

	}
}


void ZCombatInterface::DrawFriendName(MDrawContext* pDC)
{
	if (ZGetGame()->m_pMyCharacter == NULL) return;

	if (ZGetGame()->GetMatch()->IsTeamPlay())
	{
		ZCharacter* pTargetCharacter = GetTargetCharacter();
		if (pTargetCharacter == NULL) return;
		float texty = 0.75000f;
		float x = 0.024f;
		float ysub = 0.05000f;
		for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();

			itor != ZGetGame()->m_CharacterManager.end(); ++itor)
		{
			rvector pos, screen_pos;
			ZCharacter* pCharacter = (*itor).second;
			if (!pCharacter->IsVisible()) continue;
			if (pCharacter->IsDie()) continue;
			if (pCharacter->GetTeamID() != pTargetCharacter->GetTeamID()) continue;
			if (pCharacter == pTargetCharacter) continue;

			if (Z_EXTRA_HPAP_SCOREBOARD) {
				MCOLOR tmpColor = pDC->GetColor();
				pDC->SetColor(MCOLOR(240, 248, 255));
				pDC->SetFont(NULL);
				pDC->Text(x*MGetWorkspaceWidth(), texty*MGetWorkspaceHeight() - 24, pCharacter->GetUserNameA());

				pDC->SetColor(MCOLOR(0x40FF0000));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, 0.08*MGetWorkspaceWidth(), 7);

				float nValue = 0.08 * pCharacter->GetHP() / pCharacter->GetMaxHP();
				pDC->SetColor(MCOLOR(0xFFFF0000));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, nValue*MGetWorkspaceWidth(), 7);

				pDC->SetColor(MCOLOR(0x4000FF00));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, 0.08*MGetWorkspaceWidth(), 7);

				nValue = 0.08 * pCharacter->GetAP() / pCharacter->GetMaxAP();//0099FF
				pDC->SetColor(MCOLOR(0x900099FF));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, nValue*MGetWorkspaceWidth(), 7);
				pDC->SetColor(tmpColor);
				texty -= ysub;
			}

			pos = pCharacter->GetPosition();
			RVisualMesh* pVMesh = pCharacter->m_pVMesh;
			RealSpace2::rboundingbox box;

			if (pVMesh == NULL) continue;

			//			box.vmax = pVMesh->m_vBMax + pos;
			//			box.vmin = pVMesh->m_vBMin + pos;

			box.vmax = pos + rvector(50.f, 50.f, 190.f);
			box.vmin = pos + rvector(-50.f, -50.f, 0.f);

			if (isInViewFrustum(&box, RGetViewFrustum()))
			{
				/*
				#define CHARACTER_HEIGHT	185.0f
				pos.z = pos.z + CHARACTER_HEIGHT;
				screen_pos = RGetTransformCoord(pos);
				*/
				screen_pos = RGetTransformCoord(pCharacter->GetVisualMesh()->GetHeadPosition() + rvector(0, 0, 30.f));

				MFont *pFont = NULL;

				if (pCharacter->IsAdminName()) {
					pFont = MFontManager::Get("FONTa12_O1Org");
					pDC->SetColor(MCOLOR(ZCOLOR_ADMIN_NAME));
				}
				else {
					pFont = MFontManager::Get("FONTa12_O1Blr");
					pDC->SetColor(MCOLOR(0xFF00FF00));
				}

				pDC->SetBitmap(NULL);

				/////// Outline Font //////////
				//				MFont *pFont=MFontManager::Get("FONTa12_O1Blr");
				pDC->SetFont(pFont);
				///////////////////////////////
				int x = screen_pos.x - pDC->GetFont()->GetWidth(pCharacter->GetUserName()) / 2;

				pDC->Text(x, screen_pos.y - 12, pCharacter->GetUserName());

				pDC->SetColor(MCOLOR(0x30000000));
				if (ZGetGame()->GetMatch()->IsTeamPlay())
				{
					MCOLOR color;
					const int BAR_HEIGHT = 3;
					const int BAR_WIDTH = 80;

					int nHP = pCharacter->GetHP();
					int nAP = pCharacter->GetAP();

					if (nHP == 0 && !pCharacter->IsDie())
						nHP += 1;

					int bar_hp_width = (int)(BAR_WIDTH * ((int)nHP / pCharacter->GetMaxHP()));
					int bar_ap_width = (int)(BAR_WIDTH * ((int)nAP / pCharacter->GetMaxAP()));
					//0xFF0099FF
					color = MCOLOR(0xFFFF0000);
					int screeny = screen_pos.y - 50;
					int screenx = x;
					int bar_y = screeny + pFont->GetHeight() + 2;


					pDC->SetColor(color);
					pDC->FillRectangle(screenx, bar_y, bar_hp_width, BAR_HEIGHT);
					pDC->Rectangle(screenx, bar_y, BAR_WIDTH, BAR_HEIGHT);

					if (nAP > 0)
					{
						color = MCOLOR(0xFF0087FF);
						pDC->SetColor(color);
						bar_y += (BAR_HEIGHT + 2);
						pDC->FillRectangle(screenx, bar_y, bar_ap_width, BAR_HEIGHT);
						pDC->Rectangle(screenx, bar_y, BAR_WIDTH, BAR_HEIGHT);
					}
				}
			}
		}
	}
}

void ZCombatInterface::DrawEnemyName(MDrawContext* pDC)
{
	MPOINT Cp = GetCrosshairPoint();

	ZPICKINFO pickinfo;

	rvector pos, dir;
	if (!RGetScreenLine(Cp.x, Cp.y, &pos, &dir))
		return;

	ZCharacter *pTargetCharacter = GetTargetCharacter();

	if (ZGetGame()->Pick(pTargetCharacter, pos, dir, &pickinfo))
	{
		if (pickinfo.pObject) {
			if (!IsPlayerObject(pickinfo.pObject)) return;
			if (pickinfo.pObject->IsDie()) return;

			ZCharacter* pPickedCharacter = (ZCharacter*)pickinfo.pObject;

			bool bFriend = false;
			if (ZGetGame()->GetMatch()->IsTeamPlay()) {
				if (pTargetCharacter && pPickedCharacter->GetTeamID() == pTargetCharacter->GetTeamID())
					bFriend = true;
			}

			if (bFriend == false) {

				/////// Outline Font //////////

				MFont *pFont = NULL;//MFontManager::Get("FONTa12_O1Red");
				MCOLOR CharNameColor;
				if (ZGetGame()->Pick(pTargetCharacter, pos, dir, &pickinfo))
					if (pPickedCharacter->IsAdminName()) {
						pFont = MFontManager::Get("FONTa12_O1Org");
					}
					else {
						pFont = MFontManager::Get("FONTa12_O1Red");
					}

					if (pFont == NULL) _ASSERT(0);
					pDC->SetFont(pFont);
					if (pPickedCharacter->IsAdminName())
						pDC->SetColor(CharNameColor);
					int x = Cp.x - pDC->GetFont()->GetWidth(pPickedCharacter->GetUserName()) / 2;
					pDC->Text(x, Cp.y - pDC->GetFont()->GetHeight() - 10, pPickedCharacter->GetUserName());

					if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY &&
						ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY &&
						pPickedCharacter->GetTeamID() == MMT_RED &&
						!m_bSpyLocationOpened)
					{
						const int nIconWidth = 48, nIconHeight = 48;
						rvector screen_pos = RGetTransformCoord(pPickedCharacter->GetVisualMesh()->GetHeadPosition() + rvector(0, 0, 30.f));

						pDC->SetBitmap((MBitmap*)m_pSpyIcon);
						pDC->Draw(screen_pos.x - nIconWidth / 2, screen_pos.y - nIconHeight / 2 - 34, nIconWidth, nIconHeight);
					}
			}
		}
	}
}
void ZCombatInterface::DrawAllClanName(MDrawContext* pDC)
{
	if (!ZGetGame()->m_pMyCharacter->IsAdminName()) return;
	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		rvector pos, screen_pos;
		ZCharacter* pCharacter = (*itor).second;
		if (!pCharacter->m_bRendered || !pCharacter->IsVisible()) continue;
		if (pCharacter->IsDie()) continue;
		pos = pCharacter->GetPosition();
		RVisualMesh* pVMesh = pCharacter->m_pVMesh;
		RealSpace2::rboundingbox box;
		if (pVMesh == NULL) continue;

		const float fIconSize = .2f;
		int nIconSize = fIconSize * MGetWorkspaceWidth();
		box.vmax = pos + rvector(50.f, 50.f, 190.f);
		box.vmin = pos + rvector(-50.f, -50.f, 0.f);
		if (isInViewFrustum(&box, RGetViewFrustum()))
		{
			if (ZGetCamera()->GetLookMode() == ZCAMERA_MINIMAP) {
				rvector pos = pCharacter->GetPosition();
				pos.z = 0;
				screen_pos = RGetTransformCoord(pos);
			}
			else
				screen_pos = RGetTransformCoord(pCharacter->GetVisualMesh()->GetBipTypePosition(eq_parts_pos_info_Spine2) + rvector(0, 0, 15));

			pDC->SetBitmap(NULL);
			char* text = "GunZ";
			int x = screen_pos.x - pDC->GetFont()->GetWidth(text) / 2;
			pDC->SetColor(0, 0, 0);
			pDC->Text(x, screen_pos.y - 12, text);
			MBitmap *pbmp = ZGetEmblemInterface()->GetClanEmblem(pCharacter->GetClanID());
			if (pbmp)
			{
				pDC->SetBitmap(pbmp);
				pDC->Draw(x, screen_pos.y - 17, nIconSize, nIconSize);
			}
		}
	}
}
/*
void ZCombatInterface::DrawAllPlayerName(MDrawContext* pDC)
{
	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		rvector pos, screen_pos;
		ZCharacter* pCharacter = (*itor).second;
		if (!pCharacter->IsVisible()) continue;
		if (pCharacter->IsDie()) continue;

		pos = pCharacter->GetPosition();
		RVisualMesh* pVMesh = pCharacter->m_pVMesh;
		RealSpace2::rboundingbox box;

		if (pVMesh == NULL) continue;

		box.vmax = pos + rvector(50.f, 50.f, 190.f);
		box.vmin = pos + rvector(-50.f, -50.f, 0.f);

		if (isInViewFrustum(&box, RGetViewFrustum()))
		{
			// 미니맵이면 z 값을 0에 맞춘다
			if (ZGetCamera()->GetLookMode() == ZCAMERA_MINIMAP) {
				rvector pos = pCharacter->GetPosition();	//mmemory proxy
				pos.z = 0;
				screen_pos = RGetTransformCoord(pos);
			}
			else
				screen_pos = RGetTransformCoord(pCharacter->GetVisualMesh()->GetHeadPosition() + rvector(0, 0, 30.f));

			MFont *pFont = NULL;

			if (pCharacter->IsAdminName()) {
				pFont = MFontManager::Get("FONTa12_O1Org");
				pDC->SetColor(MCOLOR(ZCOLOR_ADMIN_NAME));
			}
			else {
				if (pCharacter->GetTeamID() == MMT_RED)
					pFont = MFontManager::Get("FONTa12_O1Red");
				else if (pCharacter->GetTeamID() == MMT_BLUE)
					pFont = MFontManager::Get("FONTa12_O1Blr");
				else
					pFont = MFontManager::Get("FONTa12_O1Blr");

				pDC->SetColor(MCOLOR(0xFF00FF00));
			}

			pDC->SetBitmap(NULL);

			/////// Outline Font //////////
			//				MFont *pFont=MFontManager::Get("FONTa12_O1Blr");
			pDC->SetFont(pFont);
			///////////////////////////////

			int x = screen_pos.x - pDC->GetFont()->GetWidth(pCharacter->GetUserName()) / 2;

			pDC->Text(x, screen_pos.y - 12, pCharacter->GetUserName());
			pFont = MFontManager::Get("FONTa1_O1Wht");
			pDC->SetFont(pFont);
			pDC->SetColor(MCOLOR(0xFFFFFFFF));

			char HP_AP[128];
			sprintf(HP_AP, "(%d / %d)", (int)pCharacter->GetHP(), (int)pCharacter->GetAP()); //(HP/AP)
			pDC->Text(x + 13, screen_pos.y - 35, HP_AP);

			float nValue = 0.08 * ZGetGame()->m_pMyCharacter->GetHP() / ZGetGame()->m_pMyCharacter->GetMaxHP();
			pDC->SetColor(MCOLOR(0x90FF0000));
			pDC->FillRectangleW(x - 38, screen_pos.y - 20, nValue * 2200, 4);

			nValue = 0.08 * ZGetGame()->m_pMyCharacter->GetAP() / ZGetGame()->m_pMyCharacter->GetMaxAP();
			pDC->SetColor(MCOLOR(0x9000FF00));
			pDC->FillRectangleW(x - 38, screen_pos.y - 17, nValue * 2200, 3);

			pDC->SetColor(MCOLOR(0x30000000));
			pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, 0.08*MGetWorkspaceWidth(), 7);
			float nValue = (0.08 * (pQuestPlayerInfo->GetHP() / pQuestPlayerInfo->GetMaxHP()));
			pDC->SetColor(MCOLOR(0xFFFF0000));
			pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, nValue*MGetWorkspaceWidth(), 7);

			pDC->SetColor(MCOLOR(0x30000000));
			pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, 0.08*MGetWorkspaceWidth(), 3);
			float nValuee = (0.08 * (pQuestPlayerInfo->GetAP() / pQuestPlayerInfo->GetMaxAP()));
			pDC->SetColor(MCOLOR(0xFF00FF00));
			pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, nValuee*MGetWorkspaceWidth(), 3);

			pDC->SetColor(tmpColor);


		}
	}
}*/
void ZCombatInterface::DrawAllPlayerName(MDrawContext* pDC)
{
	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		rvector pos, screen_pos;
		ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
		if (!pCharacter->IsVisible()) continue;
		if (pCharacter->IsDie()) continue;

		pos = pCharacter->GetPosition();
		RealSpace2::rboundingbox box;

		if (pCharacter->m_pVMesh == NULL) continue;

		box.vmax = pos + rvector(50.f, 50.f, 190.f);
		box.vmin = pos + rvector(-50.f, -50.f, 0.f);

		if (isInViewFrustum(&box, RGetViewFrustum()))
		{
			// 미니맵이면 z 값을 0에 맞춘다
			if (ZGetCamera()->GetLookMode() == ZCAMERA_MINIMAP) {
				rvector pos = pCharacter->GetPosition();	//mmemory proxy
				pos.z = 0;
				screen_pos = RGetTransformCoord(pos);
			}
			else
				screen_pos = RGetTransformCoord(pCharacter->GetVisualMesh()->GetHeadPosition() + rvector(0, 0, 30.f));

			MFont *pFont = NULL;

			if (pCharacter->IsAdminName()) {
				pFont = MFontManager::Get("FONTa12_O1Org");
				pDC->SetColor(MCOLOR(ZCOLOR_ADMIN_NAME));
			}
			else {
				if (pCharacter->GetTeamID() == MMT_RED)
					pFont = MFontManager::Get("FONTa12_O1Red");
				else if (pCharacter->GetTeamID() == MMT_BLUE)
					pFont = MFontManager::Get("FONTa12_O1Blr");
				else
					pFont = MFontManager::Get("FONTa12_O1Blr");

				pDC->SetColor(MCOLOR(0xFF00FF00));
			}

			/////// Outline Font //////////
			//				MFont *pFont=MFontManager::Get("FONTa12_O1Blr");
			if (pFont == NULL) _ASSERT(0);
			pDC->SetFont(pFont);
			///////////////////////////////

			int x = screen_pos.x - pDC->GetFont()->GetWidth(pCharacter->GetUserName()) / 2;

			pDC->Text(x, screen_pos.y - 12, pCharacter->GetUserName());

			pFont = MFontManager::Get("FONTa1_O1Wht");
			pDC->SetFont(pFont);
			pDC->SetColor(MCOLOR(0xFFFFFFFF));

			char HP_AP[128];
			sprintf(HP_AP, "(%d / %d)", (int)pCharacter->GetHP(), (int)pCharacter->GetAP()); //(HP/AP)
			pDC->Text(x + 13, screen_pos.y - 35, HP_AP);

			float nValue = 0.08 * ZGetGame()->m_pMyCharacter->GetHP() / ZGetGame()->m_pMyCharacter->GetMaxHP();
			pDC->SetColor(MCOLOR(0x90FF0000));
			pDC->FillRectangleW(x - 38, screen_pos.y - 20, nValue * 2200, 4);

			nValue = 0.08 * ZGetGame()->m_pMyCharacter->GetAP() / ZGetGame()->m_pMyCharacter->GetMaxAP();
			pDC->SetColor(MCOLOR(0x9049E4FC));
			pDC->FillRectangleW(x - 38, screen_pos.y - 17, nValue * 2200, 3);


		}
	}
}
void ZCombatInterface::DrawSpyName(MDrawContext* pDC)
{
	ZCharacter* pTargetCharacter = GetTargetCharacter();
	if (!pTargetCharacter) return;

	bool bCountDown = ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN;
	bool bOpen = ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY && m_bSpyLocationOpened;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		rvector pos, screen_pos;
		ZCharacter* pCharacter = (*itor).second;
		if (!pCharacter->IsVisible()) continue;
		if (pCharacter->IsDie()) continue;
		if (pCharacter == pTargetCharacter) continue;

		if ((!bCountDown) && (!bOpen || pTargetCharacter->GetTeamID() == MMT_RED))
			if (pCharacter->GetTeamID() != pTargetCharacter->GetTeamID())
				continue;

		pos = pCharacter->GetPosition();
		RVisualMesh* pVMesh = pCharacter->m_pVMesh;
		RealSpace2::rboundingbox box;

		if (pVMesh == NULL) continue;

		box.vmax = pos + rvector(50.f, 50.f, 190.f);
		box.vmin = pos + rvector(-50.f, -50.f, 0.f);

		if (isInViewFrustum(&box, RGetViewFrustum()))
		{
			if (ZGetCamera()->GetLookMode() == ZCAMERA_MINIMAP) {
				rvector pos = pCharacter->GetPosition();
				pos.z = 0;
				screen_pos = RGetTransformCoord(pos);
			}
			else
				screen_pos = RGetTransformCoord(pCharacter->GetVisualMesh()->GetHeadPosition() + rvector(0, 0, 30.f));

			MFont *pFont = NULL;

			if (pCharacter->IsAdminName()) {
				pFont = MFontManager::Get("FONTa12_O1Org");
				pDC->SetColor(MCOLOR(ZCOLOR_ADMIN_NAME));
			}
			else {
				if (bCountDown)
					pFont = MFontManager::Get("FONTa12_O1Blr");
				else if (pTargetCharacter->GetTeamID() != pCharacter->GetTeamID())
					pFont = MFontManager::Get("FONTa12_O1Red");
				else
					pFont = MFontManager::Get("FONTa12_O1Blr");

				pDC->SetColor(MCOLOR(0xFF2669B2));
			}

			pDC->SetBitmap(NULL);

			if (pFont == NULL) _ASSERT(0);
			pDC->SetFont(pFont);
			int x = screen_pos.x - pDC->GetFont()->GetWidth(pCharacter->GetUserName()) / 2;

			pDC->Text(x, screen_pos.y - 12, pCharacter->GetUserName());

			if (bOpen && pTargetCharacter->GetTeamID() == MMT_BLUE && pCharacter->GetTeamID() == MMT_RED)
			{
				const int nIconWidth = 48, nIconHeight = 48;

				pDC->SetBitmap((MBitmap*)m_pSpyIcon);
				pDC->Draw(screen_pos.x - nIconWidth / 2, screen_pos.y - nIconHeight / 2 - 34, nIconWidth, nIconHeight);
			}
		}
	}
}
/*
void ZCombatInterface::DrawHPAPBar(MDrawContext* pDC)
{
bool bDraw = m_Observer.IsVisible();
float xPos = 100.f / 800.f;
float yPos = 310.f / 600.f;

float nValue = 0.08 * ZGetGame()->m_pMyCharacter->GetHP() / ZGetGame()->m_pMyCharacter->GetMaxHP();
pDC->SetColor(MCOLOR(0x90FF0000));
pDC->FillRectangleW((xPos * 2200), yPos*MGetWorkspaceHeight() + 6, nValue * 2200, 4);

nValue = 0.08 * ZGetGame()->m_pMyCharacter->GetAP() / ZGetGame()->m_pMyCharacter->GetMaxAP();
pDC->SetColor(MCOLOR(0x9000FF00));
pDC->FillRectangleW((xPos * 2200), yPos*MGetWorkspaceHeight() + 9, nValue * 2200, 3);
}*/
#ifdef	_EVENTCHANNEL
void ZCombatInterface::Alive(MDrawContext* pDC)
{
	ZCharacter* pCharacter = GetTargetCharacter();

	if (pCharacter == NULL || pCharacter->IsObserverTarget())
		return;

	char szAlive[64]; // alive
	pDC->SetFont(GetGameFont()); // default font
	pDC->SetColor(MCOLOR(255, 255, 255)); // color

	if (strstr(ZGetGameClient()->GetChannelName(), "Event") && ZGetMyInfo()->IsAdminGrade()) // channel
	{
		int nPlayers = 0;
		for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
		{
			ZCharacter* pCharacter = (*itor).second;
			if (ZGetGame()->GetMatch()->IsTeamPlay())
			{
				if (!pCharacter->IsDie() && pCharacter->GetTeamID() != ZGetGame()->m_pMyCharacter->GetTeamID())
				{
					++nPlayers;
				}
			}
			else if (!pCharacter->IsDie() && pCharacter != ZGetGame()->m_pMyCharacter)
			{
				++nPlayers;
			}
		}
		sprintf(szAlive, "Players: (%d)", nPlayers);
		TextRelative(pDC, 710.f / 800.f, 16.f / 600.f, szAlive);
	}
}
#endif
MFont *ZCombatInterface::GetGameFont()
{
	MFont *pFont = MFontManager::Get("FONTa10_O2Wht");
	return pFont;
}

// 듀얼순위 / 팀 / 생사 / 성적이 소트의 기준이다
bool CompareZScoreBoardItem(ZScoreBoardItem* a, ZScoreBoardItem* b) {
	if (a->nDuelQueueIdx < b->nDuelQueueIdx) return true;
	if (a->nDuelQueueIdx > b->nDuelQueueIdx) return false;

	if (a->nTeam < b->nTeam) return true;
	if (a->nTeam > b->nTeam) return false;

	/*
	if(!a->bDeath && b->bDeath) return true;
	if(a->bDeath && !b->bDeath) return false;
	*/

	if (a->nExp > b->nExp) return true;
	if (a->nExp < b->nExp) return false;

	if (a->nKills > b->nKills) return true;
	if (a->nKills < b->nKills) return false;
	return false;
}
void ZCombatInterface::DrawDuelTournamentScoreBoard(MDrawContext* pDC)	// 듀얼 토너먼트 대진표 화면 (tab키)
{
	// 보드를 그려준다.
	ZGetScreenEffectManager()->DrawScoreBoard();


	MFont *pFont = GetGameFont();
	pDC->SetFont(pFont);
	pFont = pDC->GetFont();	// 만약 폰트가 없으면 다시 디폴트 폰트를 얻는다
	pDC->SetColor(MCOLOR(TEXT_COLOR_TITLE));

	char szText[256];

	// 방제를 그려준다.
	sprintf(szText, "(%03d) %s", ZGetGameClient()->GetStageNumber(), ZGetGameClient()->GetStageName());
	TextRelative(pDC, 0.26f, 0.22f, szText);

	float x = 0.27f;
	float y = 0.284f;
	float linespace2 = 0.071f / 3.f;

	// 두번째줄 앞 : 맵이름
	strcpy(szText, ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	TextRelative(pDC, x, y, szText);

	y -= linespace2;
	// 게임 타입을 써준다.
	sprintf(szText, "%s", ZGetGameTypeManager()->GetGameTypeStr(ZGetGame()->GetMatch()->GetMatchType()));
	TextRelative(pDC, x, y, szText);

	x = 0.70f;
	y = 0.284f;

	// 남은 시간 표시( 클랜전 제외)
	DrawPlayTime(pDC, x, y);	// 플레이 시간
	y -= linespace2;

	// 대진표 그리기
	((ZRuleDuelTournament*)ZGetGame()->GetMatch()->GetRule())->ShowMatchOrder(pDC, false, m_fElapsed);
}
void ZCombatInterface::DrawPlayTime(MDrawContext* pDC, float xPos, float yPos)	// 플레이 시간
{
	// #남은 시간을 얻고 싶으면 GetPlayTime()을 사용하세요
	char szText[256];
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)				// 플레이 중이라면
	{
		DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
		DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
		if (dwLimitTime != -1)//99999)
		{
			//dwLimitTime *= 60000;
			if (dwTime <= dwLimitTime)
			{
				dwTime = (dwLimitTime - dwTime) / 1000;
#ifndef _HIDETIMEBOARD
				sprintf(szText, "%s : %d:%02d", ZMsg(MSG_WORD_REMAINTIME), (dwTime / 60), (dwTime % 60));
#endif
			}
			else
#ifndef _HIDETIMEBOARD
				sprintf(szText, "%s :", ZMsg(MSG_WORD_REMAINTIME));
#else
				sprintf(szText, "");
#endif
		}
		else
#ifndef _HIDETIMEBOARD
			sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
			sprintf(szText, "");
#endif
	}
	else
#ifndef _HIDETIMEBOARD
		sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
		sprintf(szText, "");
#endif

	TextRelative(pDC, xPos, yPos, szText);
}
// 원래라면 DrawPlayTime()에서 분리해내야 하는 내용이지만 건드리기 무섭게 생김; 남은시간 얻는 함수를 새로 만들었으니 시간을 얻고 싶으면 앞으로 이것을 사용
int ZCombatInterface::GetPlayTime()
{
	// 초단위로 남은 라운드 시간을 리턴, 무효값일 때 -1 리턴함
	if (ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
		return -1;

	DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
	if (dwLimitTime == 99999 || dwLimitTime == 0) // 서버에서는 무한대일때 nLimitTime을 0으로 보내준다.
		return -2; // 무한대 시간을 표시하기 위해 -2로 반환

	dwLimitTime *= 60000;
	DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
	if (dwTime > dwLimitTime)
		return -1;

	return (dwLimitTime - dwTime) / 1000;
}

typedef list<ZScoreBoardItem*> ZSCOREBOARDITEMLIST;
void ZCombatInterface::DrawScoreBoard(MDrawContext* pDC)
{
	//#define TEST_CLAN_SCOREBOARD
	bool bClanGame = ZGetGameClient()->IsLadderGame();
	bool IsTeamPlay = ZGetGame()->GetMatch()->IsTeamPlay();
	bool bPlayerWars = ZGetGameClient()->IsPlayerWars();
#ifdef TEST_CLAN_SCOREBOARD
	bClanGame = true;
	strcpy(m_szRedClanName, "레드클랜");
	strcpy(m_szBlueClanName, "블루클랜");
#endif

	ZSCOREBOARDITEMLIST items;

	bool bSpyOpenCountDown = (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY &&
		(ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN || ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FREE));


	ZGetScreenEffectManager()->DrawScoreBoard();

	MFont *pFont = GetGameFont();
	pDC->SetFont(pFont);
	pFont = pDC->GetFont();	// 만약 폰트가 없으면 다시 디폴트 폰트를 얻는다
	pDC->SetColor(MCOLOR(TEXT_COLOR_TITLE));

	char szText[256];
	int nRed = 0, nBlue = 0;
	if (bClanGame || bPlayerWars)
	{
		// 클랜전일 경우 클랜 이름을 표시한다

		for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
			itor != ZGetCharacterManager()->end(); ++itor)
		{
			ZCharacter* pCharacter = (*itor).second;

			if (pCharacter->GetTeamID() == MMT_BLUE) nBlue++;
			if (pCharacter->GetTeamID() == MMT_RED) nRed++;
		}
		if (bPlayerWars)
			sprintf(szText, "Maximo Ladder (%d) : (%d)", nRed, nBlue);
		else
		{
			switch (ZGetGame()->GetMatch()->GetMatchType())
			{
			case MMATCH_GAMETYPE_ASSASSINATE:
				sprintf(szText, "Clan War Assassinate %d:%d (%s vs %s)", nRed, nBlue, m_szRedClanName, m_szBlueClanName);
				break;
			case MMATCH_GAMETYPE_GLADIATOR_TEAM:
				sprintf(szText, "Clan War Gladiator %d:%d (%s vs %s)", nRed, nBlue, m_szRedClanName, m_szBlueClanName);
				break;
			default:
				sprintf(szText, "Clan War %d:%d (%s vs %s)", nRed, nBlue, m_szRedClanName, m_szBlueClanName);
				break;
			}
		}
		//char nvsn[32];
		//sprintf(nvsn,"%d:%d",nRed,nBlue);
		//ZTransMsg( szText, MSG_GAME_SCORESCREEN_STAGENAME, 3, nvsn, m_szRedClanName, m_szBlueClanName );

	}
	else
	{
		// 클랜전이 아니면 방제를 표시한다
		sprintf(szText, "(%03d) %s", ZGetGameClient()->GetStageNumber(), ZGetGameClient()->GetStageName());
	}
	TextRelative(pDC, 0.26f, 0.22f, szText);


	float x = 0.27f;
	float y = 0.284f;
	float linespace2 = 0.071f / 3.f;

	// 세번째줄 앞 : 점수(팀플)
	if (ZGetGame()->GetMatch()->IsTeamPlay())
	{
		if (bPlayerWars)
			sprintf(szText, "%d (Ladder Red)  VS  %d (Ladder Blue)", ZGetGame()->GetMatch()->GetTeamScore(MMT_RED), ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE));
		else if (bClanGame)
		{

			{

				sprintf(szText, "%d (%s)  VS  %d (%s)",
					ZGetGame()->GetMatch()->GetTeamScore(MMT_RED), m_szRedClanName,
					ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE), m_szBlueClanName);
			}
		}
		else
		{
			if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) // 팀픸E灸� 무한데스매치만 예외가 많이 발생합니다 =_=;
				sprintf(szText, "%s : %d(Red) vs %d(Blue)", ZGetGameTypeManager()->GetGameTypeStr(ZGetGame()->GetMatch()->GetMatchType()),
				ZGetGame()->GetMatch()->GetTeamKills(MMT_RED),
				ZGetGame()->GetMatch()->GetTeamKills(MMT_BLUE));
		}
	}
	else
		sprintf(szText, "%s", ZGetGameTypeManager()->GetGameTypeStr(ZGetGame()->GetMatch()->GetMatchType()));
	TextRelative(pDC, x, y, szText);
	y -= linespace2;
	strcpy(szText, ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType()))
	{
		sprintf(szText, "%s (%s %d)", szText, ZMsg(MSG_CHARINFO_LEVELMARKER), ZGetQuest()->GetGameInfo()->GetQuestLevel());
	}
	TextRelative(pDC, x, y, szText);

	x = 0.70f;
	y = 0.284f;

	// 보상 아이템
	if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType()))		// 퀘스트 모드일경우
	{
		sprintf(szText, "%s : %d", ZMsg(MSG_WORD_GETITEMQTY), ZGetQuest()->GetGameInfo()->GetNumOfObtainQuestItem());
		TextRelative(pDC, x, y, szText);
		y -= linespace2;
	}

	// NPC 수
	if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType())) 	// 퀘스트 모드일 경우
	{
		int nNPCKilled = ZGetQuest()->GetGameInfo()->GetNPCCount() - ZGetQuest()->GetGameInfo()->GetNPCKilled();
		if (nNPCKilled < 0)
			nNPCKilled = 0;

		MUID uidBoss = ZGetQuest()->GetGameInfo()->GetBoss();

		if (uidBoss != MUID(0, 0))
			sprintf(szText, "%s : -", ZMsg(MSG_WORD_REMAINNPC));
		else
			sprintf(szText, "%s : %d", ZMsg(MSG_WORD_REMAINNPC), nNPCKilled);
		TextRelative(pDC, x, y, szText);
		y -= linespace2;
	}
	else if (ZGetGameTypeManager()->IsSurvivalOnly(ZGetGame()->GetMatch()->GetMatchType())) 	// 서바이벌 모드일 경우
	{
		int nNPCKilled = ZGetQuest()->GetGameInfo()->GetNPCCount() - ZGetQuest()->GetGameInfo()->GetNPCKilled();
		if (nNPCKilled < 0)
			nNPCKilled = 0;

		MUID uidBoss = ZGetQuest()->GetGameInfo()->GetBoss();

		// 서바이벌엔 자코가 없으므로 보스가 나오더라도 잔여 npc 수는 표시 가능하다
		sprintf(szText, "%s : %d", ZMsg(MSG_WORD_REMAINNPC), nNPCKilled);
		TextRelative(pDC, x, y, szText);
		y -= linespace2;
	}

	// 진행도 표시
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
	{
		if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)				// 플레이 중이라면
		{
			DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
			DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
			if (dwLimitTime != -1)//99999)

			{
				//dwLimitTime *= 60000;
				if (dwTime <= dwLimitTime)
				{
					dwTime = (dwLimitTime - dwTime) / 1000;
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s : %d:%02d", ZMsg(MSG_WORD_REMAINTIME), (dwTime / 60), (dwTime % 60));
#endif
				}
				else
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s :", ZMsg(MSG_WORD_REMAINTIME));
#else
					sprintf(szText, "");
#endif
			}
			else
#ifndef _HIDETIMEBOARD
				sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
				sprintf(szText, "");
#endif
		}
		else
#ifndef _HIDETIMEBOARD
			sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
			sprintf(szText, "");
#endif

		TextRelative(pDC, x, y, szText);
		y -= linespace2;


		sprintf(szText, "%s : %d", ZMsg(MSG_WORD_ENDKILL), ZGetGame()->GetMatch()->GetRoundCount());
	}

	// 라운드를 기다려야 하는 종류라면 라운드 표시 아니면 시간 표시
	else if (ZGetGame()->GetMatch()->IsWaitForRoundEnd() && !bClanGame && !bPlayerWars)
	{
		if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)				// 플레이 중이라면
		{
			DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
			DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
			if (dwLimitTime != -1)//99999)

			{
				//dwLimitTime *= 60000;
				if (dwTime <= dwLimitTime)
				{
					dwTime = (dwLimitTime - dwTime) / 1000;
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s : %d:%02d", ZMsg(MSG_WORD_REMAINTIME), (dwTime / 60), (dwTime % 60));
#endif
				}
				else
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s :", ZMsg(MSG_WORD_REMAINTIME));
#else
					sprintf(szText, "");
#endif
			}
			else
#ifndef _HIDETIMEBOARD
				sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
				sprintf(szText, "");
#endif
		}
		else
#ifndef _HIDETIMEBOARD
			sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
			sprintf(szText, "");
#endif

		TextRelative(pDC, x, y, szText);
		y -= linespace2;

		sprintf(szText, "%s : %d / %d %s", ZMsg(MSG_WORD_RPROGRESS), ZGetGame()->GetMatch()->GetCurrRound() + 1, ZGetGame()->GetMatch()->GetRoundCount(), ZMsg(MSG_WORD_ROUND));
	}
	else if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType())) 	// 퀘스트 모드일 경우
	{
		sprintf(szText, "%s : %d / %d", ZMsg(MSG_WORD_RPROGRESS), ZGetQuest()->GetGameInfo()->GetCurrSectorIndex() + 1, ZGetQuest()->GetGameInfo()->GetMapSectorCount());
	}
	else if (ZGetGameTypeManager()->IsSurvivalOnly(ZGetGame()->GetMatch()->GetMatchType())) 	// 서바이벌 모드일 경우
	{
		int currSector = ZGetQuest()->GetGameInfo()->GetCurrSectorIndex() + 1;
		int sectorCount = ZGetQuest()->GetGameInfo()->GetMapSectorCount();
		int repeatCount = ZGetQuest()->GetGameInfo()->GetRepeatCount();

		currSector += ZGetQuest()->GetGameInfo()->GetCurrRepeatIndex() * sectorCount;
		sectorCount *= repeatCount;
		sprintf(szText, "%s : %d / %d", ZMsg(MSG_WORD_RPROGRESS), currSector, sectorCount);
	}

	// 남은 시간 표시( 클랜전 제외)
	else if (!bClanGame && !bPlayerWars)
	{
		if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)				// 플레이 중이라면
		{
			DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
			DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
			if (dwLimitTime != -1)//99999)

			{
				//dwLimitTime *= 60000;
				if (dwTime <= dwLimitTime)
				{
					dwTime = (dwLimitTime - dwTime) / 1000;
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s : %d:%02d", ZMsg(MSG_WORD_REMAINTIME), (dwTime / 60), (dwTime % 60));
#endif
				}
				else
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s :", ZMsg(MSG_WORD_REMAINTIME));
#else
					sprintf(szText, "");
#endif
			}
			else
#ifndef _HIDETIMEBOARD
				sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
				sprintf(szText, "");
#endif
		}
		else
#ifndef _HIDETIMEBOARD
					sprintf(szText, "%s : ---", ZMsg(MSG_WORD_REMAINTIME));
#else
			sprintf(szText, "");
#endif

		TextRelative(pDC, x, y, szText);
		y -= linespace2;

		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF)
		{
			sprintf(szText, "%s : %d", "Captures", ZGetGame()->GetMatch()->GetRoundCount());
		}
		else if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_VAMPIRE)
		{
			sprintf(szText, "%s : %d", "Hunts", ZGetGame()->GetMatch()->GetRoundCount());
		}
		else if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SNIPERMODE)
		{
			sprintf(szText, "%s : %d", "Player Kill", ZGetGame()->GetMatch()->GetRoundCount());
		}
		else
		{
			sprintf(szText, "%s : %d", ZMsg(MSG_WORD_ENDKILL), ZGetGame()->GetMatch()->GetRoundCount());
		}
	}
	TextRelative(pDC, x, y, szText);

#ifdef _SRVRPNG
	y -= linespace2;

	int nPing = ZGetGameClient()->GetServerPing();
	sprintf(szText, "Server Ping : %d", nPing);
	/*if (nPing == 999)
	pDC->SetColor(0, 0, 0); //Black
	else if (nPing >= 200)
	pDC->SetColor(255, 0, 0); //Red
	else if (nPing >= 150)
	pDC->SetColor(237, 126, 1); //Orange
	else if(nPing >= 100)
	pDC->SetColor(235, 242, 19); //Yellow
	else if (nPing >= 50)
	pDC->SetColor(64, 128, 1); //Darker Green
	else if (nPing <= 50)
	pDC->SetColor(111, 255, 0); //Light Green*/
	TextRelative(pDC, x, y, szText);

	/*sprintf(szText, "Server Ping : %d", nPing);
	TextRelative(pDC, x, y, szText);*/


#endif
	if (ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL && ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nPingLimitHigh != 0) {
		y -= linespace2;
		if (ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nPingType == true)
			sprintf(szText, "Server Ping Limiter : %d ~ %d", ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nPingLimitLow, ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nPingLimitHigh);
		else
			sprintf(szText, "Peer Ping Limiter : %d ~ %d", ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nPingLimitLow, ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nPingLimitHigh);

		TextRelative(pDC, x, y, szText);
	}
	//	const float normalXPOS[6] = {0.25f, 0.46f, 0.68f, 0.77f, 0.83f, 0.92f};
	//	const float clanXPOS[6] = {0.45f, 0.25f, 0.68f, 0.77f, 0.83f, 0.92f};
	const float normalXPOS[] = { 0.26f, 0.47f, 0.67f, 0.80f, 0.86f, 0.95f, 0.334f, 0.311f, 0.102f };
	const float clanXPOS[] = { 0.44f, 0.24f, 0.64f, 0.76f, 0.82f, 0.91f, 0.514f, 0.491f };
	bool clanpos = false;
	if (bPlayerWars || bClanGame) clanpos = true;
	const float *ITEM_XPOS = clanpos ? clanXPOS : normalXPOS;

	// player
	// 화면의 xy 상대위치 ( 0~1 로봤을때 ) 는 0.25 , 0.361
	y = 0.343f;
	const float fHeight = 0.578f;	// 공간의 높이

	// 컬럼 타이틀 출력
	char szBuff[25];
	pDC->SetColor(MCOLOR(TEXT_COLOR_TITLE));
	x = ITEM_XPOS[0];	// level
	sprintf(szBuff, "%s", ZMsg(MSG_CHARINFO_LEVEL));
	TextRelative(pDC, x, y, szBuff);
	x = ITEM_XPOS[6];	// 이름
	sprintf(szBuff, "%s", ZMsg(MSG_CHARINFO_NAME));
	TextRelative(pDC, x, y, szBuff);
	x = ITEM_XPOS[1] + .02f;;	// Clan
	TextRelative(pDC, x, y, ZMsg(MSG_CHARINFO_CLAN));
	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGame()->GetMatch()->GetMatchType()))
	{
		x = ITEM_XPOS[2];	// HP/AP
		sprintf(szBuff, "%s/%s", ZMsg(MSG_CHARINFO_HP), ZMsg(MSG_CHARINFO_AP));
		TextRelative(pDC, x, y, szBuff);
	}
	else if (bClanGame || bPlayerWars)
	{
		x = ITEM_XPOS[2];	// HP/AP
		sprintf(szBuff, "%s/%s", ZMsg(MSG_CHARINFO_HP), ZMsg(MSG_CHARINFO_AP));
		TextRelative(pDC, x, y, szBuff);
	}
	else
	{
		x = ITEM_XPOS[2] - .01f;	// Exp
		TextRelative(pDC, x, y, ZMsg(MSG_WORD_EXP));
	}
	x = ITEM_XPOS[3] - .01f;	// Kills
	TextRelative(pDC, x, y, ZMsg(MSG_WORD_KILL));
	x = ITEM_XPOS[4] - .01f;	// Deaths
	TextRelative(pDC, x, y, ZMsg(MSG_WORD_DEATH));
	x = ITEM_XPOS[5] - .01f;	// Ping
	TextRelative(pDC, x, y, ZMsg(MSG_WORD_PING));

	float fTitleHeight = (float)pFont->GetHeight()*1.1f / (float)RGetScreenHeight();
	y += fTitleHeight;

	// 그릴수있는 최대줄수 줄간격은 150%
	//	int nMaxLineCount=int((fHeight-fTitleHeight)*(float)RGetScreenHeight()/((float)pFont->GetHeight()*1.1f));
	int nMaxLineCount = 16;

	// 한줄사이의 간격(높이)
	float linespace = (fHeight - fTitleHeight) / (float)nMaxLineCount;

	// 좌측의 클랜마크및 이름,점수
	if (bClanGame)
	{
		for (int i = 0; i<2; i++)
		{
			MMatchTeam nTeam = (i == 0) ? MMT_RED : MMT_BLUE;
			char *szClanName = (i == 0) ? m_szRedClanName : m_szBlueClanName;
			int nClanID = (i == 0) ? m_nClanIDRed : m_nClanIDBlue;

			MFont *pClanFont = MFontManager::Get("FONTb11b");
			pDC->SetFont(pClanFont);
			pDC->SetColor(MCOLOR(TEXT_COLOR_CLAN_NAME));

			float clancenter = .5f*(ITEM_XPOS[0] - ITEM_XPOS[1]) + ITEM_XPOS[1];
			float clanx = clancenter - .5f*((float)pClanFont->GetWidth(szClanName) / (float)MGetWorkspaceWidth());
			float clany = y + linespace * ((nTeam == MMT_RED) ? .5f : 8.5f);

			// 아이콘 출력
			MBitmap *pbmp = ZGetEmblemInterface()->GetClanEmblem(nClanID);
#ifdef TEST_CLAN_SCOREBOARD
			pbmp = MBitmapManager::Get("btntxtr_gnd_on.png");//테스트용
#endif
			if (pbmp) {
				pDC->SetBitmap(pbmp);

				const float fIconSize = .1f;
				int nIconSize = fIconSize * MGetWorkspaceWidth();

				int screenx = (clancenter - .5f*fIconSize)*MGetWorkspaceWidth();
				int screeny = (clany)*MGetWorkspaceHeight();

				pDC->Draw(screenx, screeny, nIconSize, nIconSize);

				clany += fIconSize + 1.2*linespace;
			}

			// 팀이름 출력
			TextRelative(pDC, clanx, clany, szClanName);

			// 점수 출력. 가운데 정렬
			sprintf(szText, "%d", ZGetGame()->GetMatch()->GetTeamScore(nTeam));
			clanx = clancenter - .5f*((float)pClanFont->GetWidth(szText) / (float)MGetWorkspaceWidth());
			clany += 1.f*linespace;
			TextRelative(pDC, clanx, clany, szText);

		}
	}


	// 캐릭터 리스트
	ZCharacterManager::iterator itor;
	for (itor = ZGetCharacterManager()->begin();
		itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;

		if (pCharacter->GetTeamID() == MMT_SPECTATOR) continue;	// 옵저버는 뺸다

		if (pCharacter->IsAdminHide()) continue;

		ZScoreBoardItem *pItem = new ZScoreBoardItem;

		int nRed = pCharacter->GetCharInfo()->nRedColor, nGreen = pCharacter->GetCharInfo()->nGreenColor, nBlue = pCharacter->GetCharInfo()->nBlueColor;

		sprintf(pItem->szLevel, "%d%s", pCharacter->GetProperty()->nLevel, ZMsg(MSG_CHARINFO_LEVELMARKER));
		pItem->SetColor(MCOLOR(nRed, nGreen, nBlue));

		//sprintf(pItem->szLevel, "%d%s", pCharacter->GetProperty()->nLevel, ZMsg(MSG_CHARINFO_LEVELMARKER));
#ifdef _MASKLEVELGM
		if (pCharacter->IsAdminName()) 
		{
			sprintf(pItem->szLevel, "--%s", ZMsg(MSG_CHARINFO_LEVELMARKER)); pItem->SetColor(MCOLOR(nRed, nGreen, nBlue));
		}
#endif
		sprintf(pItem->szName, "%s", pCharacter->GetUserName());

		memcpy(pItem->szClan, pCharacter->GetProperty()->GetClanName(), CLAN_NAME_LENGTH);


		pItem->nClanID = pCharacter->GetClanID();
		pItem->nTeam = ZGetGame()->GetMatch()->IsTeamPlay() ? pCharacter->GetTeamID() : MMT_END;
		if (bSpyOpenCountDown) pItem->nTeam = MMT_BLUE;
		pItem->bDeath = pCharacter->IsDie();
		if (ZGetGameTypeManager()->IsQuestDerived(ZGetGame()->GetMatch()->GetMatchType()))
			pItem->nExp = pCharacter->GetStatus().Ref().nKills * 100;
		else
			pItem->nExp = pCharacter->GetStatus().Ref().nExp;

		if (bClanGame || bPlayerWars)
		{
			if (ZGetGame()->m_pMyCharacter->IsTeam(pCharacter))
			{
				pItem->HP = pCharacter->GetHP();
				pItem->AP = pCharacter->GetAP();
				pItem->MaxHP = pCharacter->GetMaxHP();
				pItem->MaxAP = pCharacter->GetMaxAP();
			}
			else
			{
				pItem->HP = 0;
				pItem->AP = 0;
			}
		}
		pItem->nKills = pCharacter->GetStatus().Ref().nKills;
		pItem->nDeaths = pCharacter->GetStatus().Ref().nDeaths;
		pItem->uidUID = pCharacter->GetUID();

		int nPing = (pCharacter->GetUID() == ZGetGameClient()->GetPlayerUID() ? 0 : MAX_PING);
		MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(pCharacter->GetUID());
		if (pPeer) {
			if (ZGetGame()->IsReplay())
				nPing = 0;
			else
				nPing = pPeer->GetPing(ZGetGame()->GetTickTime());
		}
		pItem->nPing = nPing;
		pItem->bMyChar = pCharacter->IsHero();

		if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_SPECTATOR &&
			m_Observer.IsVisible() && m_Observer.GetTargetCharacter() == pCharacter)
			pItem->bMyChar = true;



		if (pCharacter->GetTeamID() == ZGetGame()->m_pMyCharacter->GetTeamID() && pCharacter->m_dwStatusBitPackingValue.Ref().m_bCommander)
			pItem->bCommander = true;
		else
			pItem->bCommander = false;

		// 버서커모드의 버서커
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BERSERKER)
		{
			if (pCharacter->IsTagger()) pItem->bCommander = true;
		}
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF && pCharacter->IsTagger())
			pItem->bCommander = true;

		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED && pCharacter->m_dwStatusBitPackingValue.Ref().m_bCommander)
			pItem->bCommander = true;

		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
		{
			ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGame()->GetMatch()->GetRule();	// 위험 -_-
			pItem->nDuelQueueIdx = pDuel->GetQueueIdx(pCharacter->GetUID());
		}
		else
			pItem->nDuelQueueIdx = 0;


		// GameRoom User
		MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(pCharacter->GetUID());
		if (pCache)
			pItem->bGameRoomUser = (pCache->GetPGrade() == MMPG_PREMIUM_IP) ? true : false;
		else
			pItem->bGameRoomUser = false;

		// 지난주 듀얼토너먼트 등급
		pItem->nDTLastWeekGrade = pCharacter->GetDTLastWeekGrade();
		pItem->nCountry = pCharacter->GetCountry();

		items.push_back(pItem);
	}


	items.sort(CompareZScoreBoardItem);
	ZSCOREBOARDITEMLIST::iterator i;

	int nCurrentTeamIndex;
	if (ZGetGame()->GetMatch()->IsTeamPlay() && !bSpyOpenCountDown)
		nCurrentTeamIndex = MMT_RED;
	else
	{
		if (items.size() > 0)
			nCurrentTeamIndex = (*items.begin())->nTeam;
	}

	int nCount = 0;

#ifdef _MIPING
	int MiPing = 0;
	int total = 0;
#endif

	for (i = items.begin(); i != items.end(); i++)
	{
		ZScoreBoardItem *pItem = *i;

		if (nCurrentTeamIndex != pItem->nTeam)
		{
			int nLineSeparate = nMaxLineCount / 2;

			if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_SPY)
				nLineSeparate = 4;

			nCurrentTeamIndex = pItem->nTeam;
			nCount = max(nCount, min(nLineSeparate, nMaxLineCount - ((int)items.size() - nCount)));
		}

		float itemy = y + linespace * nCount;

		nCount++;

		if (nCount>nMaxLineCount) break;

		// 배경 색깔을 결정한다
		MCOLOR backgroundcolor;
		if (pItem->bMyChar) {
			backgroundcolor = BACKGROUND_COLOR_MYCHAR_DEATH_MATCH;
			if (!bClanGame && !bPlayerWars) {
				backgroundcolor =
					(pItem->nTeam == MMT_RED) ? BACKGROUND_COLOR_MYCHAR_RED_TEAM :
					(pItem->nTeam == MMT_BLUE) ? BACKGROUND_COLOR_MYCHAR_BLUE_TEAM :
					BACKGROUND_COLOR_MYCHAR_DEATH_MATCH;
			}
		}

		if (pItem->bCommander) backgroundcolor = BACKGROUND_COLOR_COMMANDER;

		if (pItem->bMyChar || pItem->bCommander)
		{
			int y1 = itemy * MGetWorkspaceHeight();
			int y2 = (y + linespace * nCount) * MGetWorkspaceHeight();
			bool clangame = false;
			if (bPlayerWars || bClanGame) clangame = true;
			int x1 = clangame ? 0.43*MGetWorkspaceWidth() : 0.255*MGetWorkspaceWidth();
			int x2 = (0.715 + 0.26)*MGetWorkspaceWidth();

			pDC->SetColor(backgroundcolor);
			pDC->FillRectangleW(x1, y1, x2 - x1, y2 - y1);
		}

		//		backgroundy=newbackgroundy;


		// PC방 유저일 경우에 PC방 마크를 표시한다.
		/*if ( pItem->bGameRoomUser)
		{
		int nIconSize = .8f * linespace * (float)MGetWorkspaceHeight();
		float icony = itemy + (linespace - (float)nIconSize / (float)MGetWorkspaceHeight())*.5f;
		BitmapRelative(pDC, ITEM_XPOS[0] - 0.043f, icony, nIconSize+4, nIconSize, MBitmapManager::Get( "icon_gameroom_s.tga"));
		}*/

		// 듀얼토너먼트 계급장 표시(이름 앞에)
		{
			int nIconSize = .8f * linespace * (float)MGetWorkspaceHeight();
			float icony = itemy + (linespace - (float)nIconSize / (float)MGetWorkspaceHeight())*.5f;

			//Logo Duel Tournament
			char szDTGradeIconFileName[64];
			GetDuelTournamentGradeIconFileName(szDTGradeIconFileName, pItem->nDTLastWeekGrade);
			MBitmap* pBmpDTGradeIcon = MBitmapManager::Get(szDTGradeIconFileName);

			BitmapRelative(pDC, ITEM_XPOS[7], icony, nIconSize, nIconSize, MBitmapManager::Get(szDTGradeIconFileName));

			char szCountryIconFileName[64];
			GetCountryFlagIconFileName(szCountryIconFileName, pItem->nCountry);
			MBitmap* pBmpCountryIcon = MBitmapManager::Get(szCountryIconFileName);
			BitmapRelative(pDC, ITEM_XPOS[0] - 0.043f, icony, nIconSize + 4, nIconSize, MBitmapManager::Get(szCountryIconFileName));
		}


		// 글자 색깔을 결정한다.. (팀과 생사여부)
		MCOLOR textcolor = pItem->bDeath ? TEXT_COLOR_DEATH_MATCH_DEAD : TEXT_COLOR_DEATH_MATCH;

		if (!bClanGame && !bPlayerWars)
		{
			if (pItem->nTeam == MMT_RED)		// red
				textcolor = pItem->bDeath ? TEXT_COLOR_RED_TEAM_DEAD : TEXT_COLOR_RED_TEAM;
			else
				if (pItem->nTeam == MMT_BLUE)		// blue
					textcolor = pItem->bDeath ? TEXT_COLOR_BLUE_TEAM_DEAD : TEXT_COLOR_BLUE_TEAM;
				else
					if (pItem->nTeam == MMT_SPECTATOR)
						textcolor = TEXT_COLOR_SPECTATOR;

		}

		if (pItem->bSpColor)	// 특수한 유저라면..고유의 컬러를 가지고 있다.
		{
			if (!pItem->bDeath)
				textcolor = pItem->GetColor();
			else
				textcolor = 0xff402010;
		}

		pDC->SetColor(textcolor);
		pDC->SetFont(pFont);

		// 글자를 가운데 정렬하기 위해 ..
		float texty = itemy + (linespace - (float)pFont->GetHeight() / (float)MGetWorkspaceHeight())*.5f;
		x = ITEM_XPOS[0];
		TextRelative(pDC, x, texty, pItem->szLevel);

		x = ITEM_XPOS[6];
		TextRelative(pDC, x, texty, pItem->szName);

		if (!bClanGame && !bPlayerWars)
		{
			x = ITEM_XPOS[1];
			//x = 0.399f;
			int nIconSize = .8f * linespace * (float)MGetWorkspaceHeight();
			float icony = itemy + (linespace - (float)nIconSize / (float)MGetWorkspaceHeight())*.5f;

			if (pItem->szClan[0]) {
				MBitmap *pbmp = ZGetEmblemInterface()->GetClanEmblem(pItem->nClanID);
				if (pbmp) {
					pDC->SetBitmap(pbmp);
					int screenx = x*MGetWorkspaceWidth();
					int screeny = icony*MGetWorkspaceHeight();

					pDC->Draw(screenx, screeny, nIconSize, nIconSize);

				}
			}
			x += (float)nIconSize / (float)MGetWorkspaceWidth() + 0.005f;
			TextRelative(pDC, x, texty, pItem->szClan);
		}

		if (ZGetGameTypeManager()->IsQuestDerived(ZGetGame()->GetMatch()->GetMatchType()))
		{
			bool bDraw = m_Observer.IsVisible();

			ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.find(pItem->uidUID);
			if (itor != ZGetGame()->m_CharacterManager.end())
			{
				ZCharacter* pQuestPlayerInfo = (*itor).second;

				MCOLOR tmpColor = pDC->GetColor();

				x = ITEM_XPOS[2];

				pDC->SetColor(MCOLOR(0x30000000));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, 0.08*MGetWorkspaceWidth(), 7);
				float nValue = (0.08 * (pQuestPlayerInfo->GetHP() / pQuestPlayerInfo->GetMaxHP()));
				pDC->SetColor(MCOLOR(0xFFFF0000));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, nValue*MGetWorkspaceWidth(), 7);

				pDC->SetColor(MCOLOR(0x30000000));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, 0.08*MGetWorkspaceWidth(), 3);
				float nValuee = (0.08 * (pQuestPlayerInfo->GetAP() / pQuestPlayerInfo->GetMaxAP()));
				pDC->SetColor(MCOLOR(0xFF00FF00));
				pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, nValuee*MGetWorkspaceWidth(), 3);

				pDC->SetColor(tmpColor);

				x = ITEM_XPOS[3];
				int nKills = 0;
				ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pQuestPlayerInfo->GetModule(ZMID_QUESTSTATUS);
				if (pMod)
					nKills = pMod->GetKills();
				sprintf(szText, "%d", nKills);
				TextRelative(pDC, x, texty, szText, true);
			}
		}
		else
		{
			if (bClanGame || bPlayerWars)
			{
				x = ITEM_XPOS[2] - 0.02f;
				sprintf(szText, "%d", pItem->nExp);
				if (!pItem->bDeath && ZGetGame()->m_pMyCharacter->GetTeamID() == pItem->nTeam)
				{
					x = ITEM_XPOS[2] + 0.04f;
					pDC->SetColor(MCOLOR(0x30000000));
					pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, 0.04*MGetWorkspaceWidth(), 7);
					float nValue = (0.04 * (pItem->HP / pItem->MaxHP));
					pDC->SetColor(MCOLOR(0xFFFF0000));
					pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 1, nValue*MGetWorkspaceWidth(), 7);

					pDC->SetColor(MCOLOR(0x30000000));
					pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, 0.04*MGetWorkspaceWidth(), 3);
					float nValuee = (0.04 * (pItem->AP / pItem->MaxAP));
					pDC->SetColor(MCOLOR(0xFF00FF00));
					pDC->FillRectangleW((x*MGetWorkspaceWidth()), texty*MGetWorkspaceHeight() + 9, nValuee*MGetWorkspaceWidth(), 3);
				}
			}
		    else
			{
				x = ITEM_XPOS[2];
				sprintf(szText, "%d", pItem->nExp);
				TextRelative(pDC, x, texty, szText, true);
			}	

			MCOLOR color = pDC->GetColor();

			if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
			{
				if (!pItem->bDeath)
					pDC->SetColor(200, 0, 0);
				else
					pDC->SetColor(120, 0, 0);
			}

			x = ITEM_XPOS[3];
			sprintf(szText, "%d", pItem->nKills);
			TextRelative(pDC, x, texty, szText, true);

			pDC->SetColor(color);
			}

		x = ITEM_XPOS[4];
		sprintf(szText, "%d", pItem->nDeaths);
		TextRelative(pDC, x, texty, szText, true);

		/*x = ITEM_XPOS[5];
		sprintf(szText, "%d", pItem->nPing);
		TextRelative(pDC, x, texty, szText, true);*/

#ifdef _MIPING
		int p = pItem->nPing;
		if (p != 999 && p > 0 && !pItem->bMyChar)
		{
			MiPing += p;
			total++;
		}
		if (pItem->uidUID == ZGetGameClient()->GetPlayerUID())
			pItem->nPing = ZGetGameClient()->MiPing;

		sprintf(szText, "%d", ((pItem->bMyChar && ZGetGameClient()->MiPing != NULL) ? ZGetGameClient()->MiPing : pItem->nPing));
#endif

#ifdef COLORANDBARPING
		x = ITEM_XPOS[5];
		sprintf(szText, "%d", pItem->nPing);
		if (pItem->nPing == 0 || pItem->nPing > 250)
			pDC->SetColor(244, 176, 66);
		else if (pItem->nPing > 1)
			pDC->SetColor(247, 118, 118);
		else if (pItem->nPing > 150)
			pDC->SetColor(137, 255, 141);
		TextRelative(pDC, x, texty, szText, true);
#else
		x = ITEM_XPOS[5]; //Removido para que funcione el Ping Bar
		sprintf(szText, "%d", pItem->nPing);
		TextRelative(pDC, x, texty, szText, true);

         // Ping bar.         
      /*   x = ITEM_XPOS[5];
        int nIconSize = .8f * linespace * (float)MGetWorkspaceHeight();
        float icony = itemy + (linespace - (float)nIconSize / (float)MGetWorkspaceHeight())*.5f;
        if (pItem->nPing == 0 || pItem->nPing == 999)
        {
            BitmapRelative(pDC, x, texty, nIconSize + 4, nIconSize, MBitmapManager::Get("AgentPing01.tga"));
        }
        else if (pItem->nPing >= 250)
        {
            BitmapRelative(pDC, x, texty, nIconSize + 4, nIconSize, MBitmapManager::Get("AgentPing03.tga"));
        }
        else if (pItem->nPing >= 1)
        {
            BitmapRelative(pDC, x, texty, nIconSize + 4, nIconSize, MBitmapManager::Get("AgentPing05.tga"));
        }*/
#endif
		//		y+=linespace; //bug random clan name
	}

#ifdef _MIPING
	if (total > 2)
		MiPing = (int)MiPing / total;
	ZGetGameClient()->MiPing = MiPing;
#endif

	while (!items.empty())
	{
		delete *items.begin();
		items.erase(items.begin());
	}
}

// 팀 / 생사 / 성적이 소트의 기준이다
bool CompareZResultBoardItem(ZResultBoardItem* a, ZResultBoardItem* b) {
	if (a->nScore > b->nScore) return true;
	else if (a->nScore < b->nScore) return false;

	if (a->nKills > b->nKills) return true;
	else if (a->nKills < b->nKills) return false;

	if (a->nDeaths < b->nDeaths) return true;
	else if (a->nDeaths > b->nDeaths) return false;

	return false;
}

void AddCombatResultInfo(const char* szName, int nScore, int nKill, int nDeath, int bMyChar, bool bGameRoomUser)
{
	char szText[128];

	MTextArea* pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerNameList");
	if (pWidget)
		pWidget->AddText(szName, (bMyChar ? MCOLOR(0xFFFFF794) : MCOLOR(0xFFFFF794)));

	for (int i = 0; i < 16; i++)
	{
		char szWidget[128];
		sprintf(szWidget, "CombatResult_PlayerScore%d", i);
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pLabel)
		{
			if (strcmp(pLabel->GetText(), "") == 0)
			{
				sprintf(szText, "%d", nScore);
				pLabel->SetText(szText);
				pLabel->SetAlignment(MAM_RIGHT);

				sprintf(szWidget, "CombatResult_GameRoomImg%02d", i);
				MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
				if (pWidget)
					pWidget->Show(bGameRoomUser);

				break;
			}
		}
	}

	pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerKillList");
	if (pWidget)
	{
		sprintf(szText, "%d", nKill);
		pWidget->AddText(szText, MCOLOR(0xFFFFF794));
	}

	pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerDeathList");
	if (pWidget)
	{
		sprintf(szText, "%d", nDeath);
		pWidget->AddText(szText, MCOLOR(0xFFFFF794));
	}
}


void AddClanResultInfoWin(const char* szName, int nScore, int nKill, int nDeath, int bMyChar, bool bGameRoomUser)
{
	char szText[125];

	MTextArea* pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerNameList1");
	if (pWidget)
		pWidget->AddText(szName, (bMyChar ? MCOLOR(0xFFFFF794) : MCOLOR(0xFFFFF794)));

	for (int i = 0; i < 4; i++)
	{
		char szWidget[128];
		sprintf(szWidget, "ClanResult_PlayerScore1%d", i);
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pLabel)
		{
			if (strcmp(pLabel->GetText(), "") == 0)
			{
				sprintf(szText, "%d", nScore);
				pLabel->SetText(szText);
				pLabel->SetAlignment(MAM_RIGHT);

				sprintf(szWidget, "ClanResult_GameRoomImg1%d", i);
				MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
				if (pWidget)
					pWidget->Show(bGameRoomUser);

				break;
			}
		}
	}

	pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerKillList1");
	if (pWidget)
	{
		sprintf(szText, "%d", nKill);
		pWidget->AddText(szText, MCOLOR(0xFFFFF794));
	}

	pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerDeathList1");
	if (pWidget)
	{
		sprintf(szText, "%d", nDeath);
		pWidget->AddText(szText, MCOLOR(0xFFFFF794));
	}
}

void AddClanResultInfoLose(const char* szName, int nScore, int nKill, int nDeath, int bMyChar, bool bGameRoomUser)
{
	char szText[125];

	MTextArea* pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerNameList2");
	if (pWidget)
		pWidget->AddText(szName, (bMyChar ? MCOLOR(0xFFFFF794) : MCOLOR(0xFFFFF794)));

	for (int i = 0; i < 4; i++)
	{
		char szWidget[128];
		sprintf(szWidget, "ClanResult_PlayerScore2%d", i);
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pLabel)
		{
			if (strcmp(pLabel->GetText(), "") == 0)
			{
				sprintf(szText, "%d", nScore);
				pLabel->SetText(szText);
				pLabel->SetAlignment(MAM_RIGHT);

				sprintf(szWidget, "ClanResult_GameRoomImg2%d", i);
				MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
				if (pWidget)
					pWidget->Show(bGameRoomUser);

				break;
			}
		}
	}

	pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerKillList2");
	if (pWidget)
	{
		sprintf(szText, "%d", nKill);
		pWidget->AddText(szText, MCOLOR(0xFFFFF794));
	}

	pWidget = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerDeathList2");
	if (pWidget)
	{
		sprintf(szText, "%d", nDeath);
		pWidget->AddText(szText, MCOLOR(0xFFFFF794));
	}
}

// 결과 화면을 그린다.
void ZCombatInterface::GetResultInfo(void)
{
	// Sort list
#ifdef _DEBUG
	m_ResultItems.push_back(new ZResultBoardItem("test01", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test02", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test03", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test04", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test05", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test06", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test07", "RED Clan", MMT_RED, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test08", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test09", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test10", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test11", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test12", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test13", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
	m_ResultItems.push_back(new ZResultBoardItem("test14", "BLUE Clan", MMT_BLUE, ((rand() % 80000) - 5000), (rand() % 100), (rand() % 100)));
#endif
	m_ResultItems.sort(CompareZResultBoardItem);

	// Set UI
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult");
	if (pWidget)  pWidget->Show(false);
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult");
	if (pWidget)  pWidget->Show(false);
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult");
	if (pWidget)  pWidget->Show(false);
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult");
	if (pWidget)  pWidget->Show(false);
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("DuelTournamentResult");
	if (pWidget)  pWidget->Show(false);

	const int _H18 = CONVERT600(18);
	const int _H2 = CONVERT600(2);
	const int _H21 = CONVERT600(21);
	const int _W17 = CONVERT800(17);
	const int _W21 = CONVERT800(21);
	const int _W20 = CONVERT800(20);

	MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerNameList");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	for (int i = 0; i < 16; i++)
	{
		char szWidget[128];
		sprintf(szWidget, "CombatResult_PlayerScore%d", i);
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pLabel)
		{
			MRECT rect;
			rect = pLabel->GetRect();
			rect.y = pTextArea->GetRect().y + _H18 * i - _H2;
			rect.h = _H21;
			pLabel->SetBounds(rect);

			pLabel->SetText("");
			pLabel->SetAlignment(MAM_LEFT | MAM_TOP);
		}

		sprintf(szWidget, "CombatResult_GameRoomImg%02d", i);
		MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pWidget)
			pWidget->Show(false);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerKillList");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerDeathList");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerNameList1");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	for (int i = 0; i < 4; i++)
	{
		char szWidget[128];
		sprintf(szWidget, "ClanResult_PlayerScore1%d", i);
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pLabel)
		{
			MRECT rect;
			rect = pLabel->GetRect();
			rect.y = pTextArea->GetRect().y + _H18 * i - _H2;
			rect.h = _H21;
			pLabel->SetBounds(rect);

			pLabel->SetText("");
			pLabel->SetAlignment(MAM_LEFT | MAM_TOP);
		}

		sprintf(szWidget, "ClanResult_GameRoomImg1%d", i);
		MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pWidget)
			pWidget->Show(false);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerKillList1");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerDeathList1");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerNameList2");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	for (int i = 0; i < 4; i++)
	{
		char szWidget[128];
		sprintf(szWidget, "ClanResult_PlayerScore2%d", i);
		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pLabel)
		{
			MRECT rect;
			rect = pLabel->GetRect();
			rect.y = pTextArea->GetRect().y + _H18 * i - _H2;
			rect.h = _H21;
			pLabel->SetBounds(rect);

			pLabel->SetText("");
			pLabel->SetAlignment(MAM_LEFT | MAM_TOP);
		}

		sprintf(szWidget, "ClanResult_GameRoomImg2%d", i);
		MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szWidget);
		if (pWidget)
			pWidget->Show(false);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerKillList2");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}
	pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerDeathList2");
	if (pTextArea) {
		pTextArea->Clear();
		pTextArea->SetCustomLineHeight(_H18);
	}


	char szFileName[256];
	szFileName[0] = 0;

	// Set player info
	if (ZGetGameTypeManager()->IsQuestOnly(ZGetGame()->GetMatch()->GetMatchType()))	// 퀘스트이면...
	{
		// 초기 UI 설정
		strcpy(szFileName, "interface/loadable/rstbg_quest.jpg");
		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult");
		if (pWidget)
			pWidget->Show(true);

		//  경험치 및 바운티 초기화
		ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetPlusXP");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetMinusXP");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetTotalXP");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuestResult_GetBounty");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
	}

	else if (ZGetGameTypeManager()->IsSurvivalOnly(ZGetGame()->GetMatch()->GetMatchType()))	// 서바이벌이면...
	{
		// 초기 UI 설정
		strcpy(szFileName, "interface/loadable/rstbg_survival.jpg");
		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult");
		if (pWidget)
			pWidget->Show(true);

		//  경험치 및 바운티 초기화
		ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetReachedRound");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetPoint");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetXP");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("SurvivalResult_GetBounty");
		if (pBmNumLabel)
			pBmNumLabel->SetNumber(0, false);
	}

	else if (ZGetGameClient()->IsLadderGame())		// 클랜전이면...
	{
		// 초기 UI 설정
		strcpy(szFileName, "interface/loadable/rstbg_clan.jpg");
		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult");
		if (pWidget)
			pWidget->Show(true);

		// Get winner team
		int nWinnerTeam;
		if (ZGetGame()->GetMatch()->GetTeamScore(MMT_RED) == ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE))  // draw 
		{
			MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_Win");
			if (pPicture) 	pPicture->SetBitmap(MBitmapManager::Get("result_draw.tga"));

			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_Lose");
			if (pPicture) 	pPicture->SetBitmap(MBitmapManager::Get("result_draw.tga"));
		}
		else
		{
			MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_Win");
			if (pPicture) 	pPicture->SetBitmap(MBitmapManager::Get("result_win.tga"));

			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_Lose");
			if (pPicture) 	pPicture->SetBitmap(MBitmapManager::Get("result_lose.tga"));
		}


		if (ZGetGame()->GetMatch()->GetTeamScore(MMT_RED) > ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE))
			nWinnerTeam = MMT_RED;
		else
			nWinnerTeam = MMT_BLUE;

		for (int i = 0; i < 2; i++)
		{
			MMatchTeam nTeam = (i == 0) ? MMT_RED : MMT_BLUE;
			char *szClanName = (i == 0) ? m_szRedClanName : m_szBlueClanName;
			int nClanID = (i == 0) ? m_nClanIDRed : m_nClanIDBlue;

			// Put clan mark
			MPicture* pPicture;
			if (nWinnerTeam == nTeam)
				pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_ClanBitmap1");
			else
				pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_ClanBitmap2");
			if (pPicture)
			{
				MBitmap* pBitmap = ZGetEmblemInterface()->GetClanEmblem2(nClanID);
				if (pBitmap)
				{
					pPicture->SetBitmap(pBitmap);
					pPicture->Show(true);
				}
			}

			// Put label
			MLabel* pLabel;
			if (nWinnerTeam == nTeam)
				pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerNameListLabel1");
			else
				pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ClanResult_PlayerNameListLabel2");
			if (pLabel)
			{
				pLabel->SetText(szClanName);
				pLabel->Show(true);
			}


			// 게임방 표시 정렬
			int nStartX = 0;
			int nStartY = 0;
			char szName[256];
			sprintf(szName, "ClanResult_PlayerNameList%d", i + 1);
			pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szName);
			if (pWidget)
			{
				nStartX = pWidget->GetRect().x;
				nStartY = pWidget->GetRect().y;
			}

			for (int j = 0; j < 4; j++)
			{
				char szName[256];
				sprintf(szName, "ClanResult_GameRoomImg%d%d", i + 1, j);
				pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szName);
				if (pWidget)
				{
					MRECT rect2;
					rect2.x = nStartX - _W17;
					rect2.y = _H18 * j + nStartY;
					rect2.w = _W21;
					rect2.h = _H18;

					pWidget->SetBounds(rect2);
				}
			}
		}


		// 팀 정보 추가
		for (ZResultBoardList::iterator i = m_ResultItems.begin(); i != m_ResultItems.end(); i++)
		{
			ZResultBoardItem *pItem = *i;

			if ((pItem->nTeam != MMT_RED) && (pItem->nTeam != MMT_BLUE))
				continue;

			// Put info
			if (nWinnerTeam == pItem->nTeam)
				AddClanResultInfoWin(pItem->szName, pItem->nScore, pItem->nKills, pItem->nDeaths, pItem->bMyChar, pItem->bGameRoomUser);
			else
				AddClanResultInfoLose(pItem->szName, pItem->nScore, pItem->nKills, pItem->nDeaths, pItem->bMyChar, pItem->bGameRoomUser);
		}
	}
	else if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)	// 듀얼토너먼트면..
	{
		strcpy(szFileName, "Interface/loadable/rstbg_deathmatch.jpg");
		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("DuelTournamentResult");
		if (pWidget)
			pWidget->Show(true);

		// 우승자 캐릭터 뷰 보여주기
		ZCharacterView* pCharView = GetWidgetCharViewResult();
		if (pCharView) {
			MUID uidChampion = ((ZRuleDuelTournament*)ZGetGame()->GetMatch()->GetRule())->GetChampion();
			pCharView->SetCharacter(uidChampion);
		}
	}
	else
	{
		// 초기 UI 설정
		if ((ZGetLocale()->GetCountry() == MC_US) || (ZGetLocale()->GetCountry() == MC_BRAZIL) || (ZGetLocale()->GetCountry() == MC_INDIA))
		{
			// 인터내셔널 및 브라질 버젼 한정 옵션
			if ((rand() % 2))
				strcpy(szFileName, "interface/loadable/rstbg_deathmatch.jpg");
			else
				strcpy(szFileName, "interface/loadable/rstbg_clan.jpg");
		}
		else
			strcpy(szFileName, "interface/loadable/rstbg_deathmatch.jpg");

		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult");
		if (pWidget)
			pWidget->Show(true);


		int nStartY = 0;
		pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_PlayerNameList");
		if (pWidget)
			nStartY = pWidget->GetRect().y;


		ZResultBoardList::iterator itrList = m_ResultItems.begin();
		for (int i = 0; i < 16; i++)
		{
			int nTeam = 0;

			if (itrList != m_ResultItems.end())
			{
				ZResultBoardItem *pItem = *itrList;

				if ((pItem->nTeam == MMT_RED) || (pItem->nTeam == MMT_BLUE) || (pItem->nTeam == 4))
					AddCombatResultInfo(pItem->szName, pItem->nScore, pItem->nKills, pItem->nDeaths, pItem->bMyChar, pItem->bGameRoomUser);

				nTeam = pItem->nTeam;
				itrList++;
			}


			for (int j = MMT_RED; j <= MMT_BLUE; j++)
			{
				char szName[128];
				sprintf(szName, "CombatResult_%sTeamBG%02d", ((j == MMT_RED) ? "Red" : "Blue"), i);

				pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szName);
				if (pWidget)
				{
					MRECT rect;
					rect = pWidget->GetRect();
					rect.y = _H18 * i + nStartY;
					rect.h = _H18;

					pWidget->SetBounds(rect);

					if (nTeam == j)
						pWidget->Show(true);
					else
						pWidget->Show(false);

					pWidget->SetOpacity(110);


					// 좀 안좋은 위치지만...  -_-;
					sprintf(szName, "CombatResult_GameRoomImg%02d", i);
					pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szName);
					if (pWidget)
					{
						MRECT rect2;
						rect2 = pWidget->GetRect();
						rect2.x = rect.x - _W20;
						rect2.y = _H18 * i + nStartY;
						rect2.w = _W21;
						rect2.h = _H18;

						pWidget->SetBounds(rect2);
					}
				}
			}
		}


		// 이미지 설정
		MPicture* pPicture;
		if (ZGetGameTypeManager()->IsTeamExtremeGame(ZGetGame()->GetMatch()->GetMatchType()))		// 으아아아악
		{
			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_Finish");
			if (pPicture)
				pPicture->Show(false);

			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_WinLoseDraw");
			if (pPicture)
			{
				if (ZGetGame()->GetMatch()->GetTeamKills(MMT_RED) == ZGetGame()->GetMatch()->GetTeamKills(MMT_BLUE))
					pPicture->SetBitmap(MBitmapManager::Get("result_draw.tga"));
				else
				{
					if (ZGetGame()->GetMatch()->GetTeamKills(MMT_RED) > ZGetGame()->GetMatch()->GetTeamKills(MMT_BLUE))
					{
						if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED)
							pPicture->SetBitmap(MBitmapManager::Get("result_win.tga"));
						else
							pPicture->SetBitmap(MBitmapManager::Get("result_lose.tga"));
					}
					else
					{
						if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_BLUE)
							pPicture->SetBitmap(MBitmapManager::Get("result_win.tga"));
						else
							pPicture->SetBitmap(MBitmapManager::Get("result_lose.tga"));
					}
				}

				pPicture->Show(true);
			}
		}
		else if (ZGetGameInterface()->m_bTeamPlay && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SPY)
		{
			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_Finish");
			if (pPicture)
				pPicture->Show(false);

			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_WinLoseDraw");
			if (pPicture)
			{
				if (ZGetGame()->GetMatch()->GetTeamScore(MMT_RED) == ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE))
					pPicture->SetBitmap(MBitmapManager::Get("result_draw.tga"));
				else
				{
					if (ZGetGame()->GetMatch()->GetTeamScore(MMT_RED) > ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE))
					{
						if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED)
							pPicture->SetBitmap(MBitmapManager::Get("result_win.tga"));
						else
							pPicture->SetBitmap(MBitmapManager::Get("result_lose.tga"));
					}
					else
					{
						if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_BLUE)
							pPicture->SetBitmap(MBitmapManager::Get("result_win.tga"));
						else
							pPicture->SetBitmap(MBitmapManager::Get("result_lose.tga"));
					}
				}

				pPicture->Show(true);
			}
		}
		else										// 개인전이면...
		{
			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_Finish");
			if (pPicture)
				pPicture->Show(true);

			pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatResult_WinLoseDraw");
			if (pPicture)
				pPicture->Show(false);
		}
	}


	// 배경이미지 로딩
	m_pResultBgImg = new MBitmapR2;
	((MBitmapR2*)m_pResultBgImg)->Create("resultbackground.png", RGetDevice(), szFileName);
	if (m_pResultBgImg != nullptr)
	{
		// 읽어온 비트맵 이미지 포인터를 해당 위젯에 넘겨줘서 표시한다
		MPicture* pBgImage = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("GameResult_Background");
		if (pBgImage)
			pBgImage->SetBitmap(m_pResultBgImg->GetSourceBitmap());
	}
}

void ZCombatInterface::DrawResultBoard(MDrawContext* pDC)
{
	////////////////////////////////////////////////////////////////////////
	// 이함수는 호출하는 곳도 없다. 버리는 함수인것 같다. 실제로는 GetResultInfo()에서 그리는 듯.
	////////////////////////////////////////////////////////////////////////

	// 클랜전일때 내 팀을 왼편에, 상대팀을 오른쪽에 보여준다

	bool bClanGame = ZGetGameClient()->IsLadderGame();
	//	bool bClanGame = true;

	if (!m_pResultPanel) return;

	m_pResultPanel->Draw(0);

	if (m_pResultPanel_Team)
		m_pResultPanel_Team->Draw(0);

	/*
	if(m_pResultPanel->GetVMesh()->isOncePlayDone())
	{
	SAFE_DELETE(m_pResultPanel);
	return;
	}
	*/

#define FADE_START_FRAME	20000

	RVisualMesh* pvm = m_pResultPanel->GetVMesh();
	if (bClanGame && pvm->isOncePlayDone())
	{
		if (!m_pResultLeft)
		{
			char* szEffectNames[] = { "clan_win", "clan_draw", "clan_lose" };

			int nRed = ZGetGame()->GetMatch()->GetTeamScore(MMT_RED);
			int nBlue = ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE);
			int nLeft, nRight;

			if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED) {
				nLeft = (nRed == nBlue) ? 1 : (nRed > nBlue) ? 0 : 2;
			}
			else
				nLeft = (nRed == nBlue) ? 1 : (nRed < nBlue) ? 0 : 2;

			// 오른쪽은 왼쪽의 반대
			nRight = 2 - nLeft;

			m_pResultLeft = ZGetScreenEffectManager()->CreateScreenEffect(szEffectNames[nLeft],
				rvector(-240.f, -267.f, 0));
			m_pResultRight = ZGetScreenEffectManager()->CreateScreenEffect(szEffectNames[nRight],
				rvector(240.f, -267.f, 0));
		}

		m_pResultLeft->Draw(0);
		m_pResultRight->Draw(0);
	}

	int nFrame = pvm->GetFrameInfo(ani_mode_lower)->m_nFrame;

	float fOpacity = min(1.f, max(0, float(nFrame - FADE_START_FRAME)
		/ float(pvm->GetFrameInfo(ani_mode_lower)->m_pAniSet->GetMaxFrame() - FADE_START_FRAME)));

	MFont* pFont = GetGameFont();
	pDC->SetFont(pFont);
	pFont = pDC->GetFont();	// 만약 폰트가 없으면 다시 디폴트 폰트를 얻는다

	MCOLOR opacity = MCOLOR(0, 0, 0, 255 * fOpacity);
	pDC->SetOpacity(255 * fOpacity);

	float x, y;

	char szText[256];

	x = 0.026f;
	y = 0.107f;

	const float fHeight = 0.651f;	// 공간의 높이

	// 그릴수있는 최대줄수 줄간격은 150%
	//	int nMaxLineCount=int(fHeight*RGetScreenHeight()/((float)pFont->GetHeight()*1.5f));
	int nMaxLineCount = 16;

	// 한줄사이의 간격(높이)
	float linespace = fHeight / (float)nMaxLineCount;

	m_ResultItems.sort(CompareZResultBoardItem);

	/*
	m_ResultItems.clear();
	g_pGame->m_pMyCharacter->SetTeamID(MMT_RED);
	m_ResultItems.push_back(new ZResultBoardItem("test1","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test2","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test3","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test4","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test5","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test6","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test7","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test8","지옥의발차기",MMT_RED,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test1","대략낭패",MMT_BLUE,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test2","대략낭패",MMT_BLUE,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test3","대략낭패",MMT_BLUE,0,0,0));
	m_ResultItems.push_back(new ZResultBoardItem("test4","대략낭패",MMT_BLUE,0,0,0,true));
	*/

	if (bClanGame)
	{
		int nLeft = 0;
		int nRight = 0;

		y = 0.387f;

		// TODO : 클랜 이름및 emblem 출력은 stagesetting 혹은 match쪽에 정보가 추가되는대로 수정
		for (ZResultBoardList::iterator i = m_ResultItems.begin(); i != m_ResultItems.end(); i++)
		{
			ZResultBoardItem* pItem = *i;

			int y1, y2;
			float itemy;

			float clancenter;
			bool bDrawClanName = false;

			MCOLOR backgroundcolor;

			if (pItem->nTeam == ZGetGame()->m_pMyCharacter->GetTeamID()) {
				x = 0.035f;
				itemy = y + linespace * nLeft;
				nLeft++;
				if (nLeft == 1)
				{
					bDrawClanName = true;
					clancenter = 0.25f;
				}
				backgroundcolor = (nLeft % 2 == 0) ? BACKGROUND_COLOR1 : BACKGROUND_COLOR2;
				y1 = itemy * MGetWorkspaceHeight();
				y2 = (y + linespace * nLeft) * MGetWorkspaceHeight();
			}
			else {
				x = 0.55f;
				itemy = y + linespace * nRight;
				nRight++;
				if (nRight == 1)
				{
					bDrawClanName = true;
					clancenter = 0.75f;
				}
				backgroundcolor = (nRight % 2 == 1) ? BACKGROUND_COLOR1 : BACKGROUND_COLOR2;
				y1 = itemy * MGetWorkspaceHeight();
				y2 = (y + linespace * nRight) * MGetWorkspaceHeight();
			}

			if (bDrawClanName)
			{
				MCOLOR textcolor = TEXT_COLOR_CLAN_NAME;
				textcolor.a = opacity.a;
				pDC->SetColor(textcolor);

				MFont* pClanFont = MFontManager::Get("FONTb11b");
				pDC->SetFont(pClanFont);

				float clanx = clancenter - .5f * (float)pClanFont->GetWidth(pItem->szClan) / (float)MGetWorkspaceWidth();
				TextRelative(pDC, clanx, 0.15, pItem->szClan);

				char szText[32];
				sprintf(szText, "%d", ZGetGame()->GetMatch()->GetTeamScore((MMatchTeam)pItem->nTeam));

				clanx = clancenter - .5f * (float)pClanFont->GetWidth(szText) / (float)MGetWorkspaceWidth();
				TextRelative(pDC, clanx, 0.2, szText);

				// 칼럼표시
				textcolor = TEXT_COLOR_TITLE;
				textcolor.a = opacity.a;
				pDC->SetColor(textcolor);
				float texty = itemy - linespace + (linespace - (float)pFont->GetHeight() / (float)RGetScreenHeight()) * .5f;
				TextRelative(pDC, x, texty, "Level Name");
				TextRelative(pDC, x + .25f, texty, "Exp", true);
				TextRelative(pDC, x + .32f, texty, "Kill", true);
				TextRelative(pDC, x + .39f, texty, "Death", true);

			}

			if (pItem->bMyChar)
				backgroundcolor = BACKGROUND_COLOR_MYCHAR_DEATH_MATCH;
			backgroundcolor.a = opacity.a >> 1;
			pDC->SetColor(backgroundcolor);
			pDC->FillRectangleW(
				(x - .01f) * MGetWorkspaceWidth(), y1,
				.44f * MGetWorkspaceWidth(), y2 - y1);

			MCOLOR textcolor = TEXT_COLOR_DEATH_MATCH;
			textcolor.a = opacity.a;
			pDC->SetColor(textcolor);
			pDC->SetFont(pFont);

			// 글자를 가운데 정렬하기 위해 ..
			float texty = itemy + (linespace - (float)pFont->GetHeight() / (float)RGetScreenHeight()) * .5f;
			TextRelative(pDC, x, texty, pItem->szName);

			sprintf(szText, "%d", pItem->nScore);
			TextRelative(pDC, x + .25f, texty, szText, true);

			sprintf(szText, "%d", pItem->nKills);
			TextRelative(pDC, x + .32f, texty, szText, true);

			sprintf(szText, "%d", pItem->nDeaths);
			TextRelative(pDC, x + .39f, texty, szText, true);
		}
	}
	else
	{
		//	int backgroundy=y*MGetWorkspaceHeight();
		int nCount = 0;

		for (ZResultBoardList::iterator i = m_ResultItems.begin(); i != m_ResultItems.end(); i++)
		{
			ZResultBoardItem* pItem = *i;

			float itemy = y + linespace * nCount;

			nCount++;

			/*
			// 마지막줄이면 ... 을찍고 넘어간다
			if(nCount==nMaxLineCount)
			{
			pDC->SetColor(MCOLOR(255,255,255,opacity.a));
			x=0.50f;
			TextRelative(pDC,x,y,".....");
			break;
			}
			*/

			// 배경 색깔을 결정한다
			MCOLOR backgroundcolor = (nCount % 2 == 0) ? BACKGROUND_COLOR1 : BACKGROUND_COLOR2;
			if (pItem->bMyChar) backgroundcolor =
				(pItem->nTeam == MMT_RED) ? BACKGROUND_COLOR_MYCHAR_RED_TEAM :
				(pItem->nTeam == MMT_BLUE) ? BACKGROUND_COLOR_MYCHAR_BLUE_TEAM :
				BACKGROUND_COLOR_MYCHAR_DEATH_MATCH;

			backgroundcolor.a = opacity.a >> 1;
			pDC->SetColor(backgroundcolor);

			int y1 = itemy * MGetWorkspaceHeight();
			int y2 = (y + linespace * nCount) * MGetWorkspaceHeight();

			pDC->FillRectangleW(
				0.022f * MGetWorkspaceWidth(), y1,
				0.960 * MGetWorkspaceWidth(), y2 - y1);
			//		backgroundy=newbackgroundy;

			// 글자 색깔을 결정한다.. 
			MCOLOR textcolor = TEXT_COLOR_DEATH_MATCH;

			if (pItem->nTeam == MMT_RED)		// red
				textcolor = TEXT_COLOR_RED_TEAM;
			else
				if (pItem->nTeam == MMT_BLUE)		// blue
					textcolor = TEXT_COLOR_BLUE_TEAM;
				else
					if (pItem->nTeam == MMT_SPECTATOR)
						textcolor = TEXT_COLOR_SPECTATOR;

			textcolor.a = opacity.a;
			pDC->SetColor(textcolor);

			// 글자를 가운데 정렬하기 위해 ..
			float texty = itemy + (linespace - (float)pFont->GetHeight() / (float)RGetScreenHeight()) * .5f;

			x = 0.025f;
			TextRelative(pDC, x, texty, pItem->szName);

			x = 0.43f;
			sprintf(szText, "%d", pItem->nScore);
			TextRelative(pDC, x, texty, szText, true);

			x = 0.52f;
			sprintf(szText, "%d", pItem->nKills);
			TextRelative(pDC, x, texty, szText, true);

			x = 0.61f;
			sprintf(szText, "%d", pItem->nDeaths);
			TextRelative(pDC, x, texty, szText, true);

			const float iconspace = 0.053f;

			x = 0.705f;

			pDC->SetBitmapColor(MCOLOR(255, 255, 255, 255 * fOpacity));

			IconRelative(pDC, x, texty, 0); x += iconspace;
			IconRelative(pDC, x, texty, 1); x += iconspace;
			IconRelative(pDC, x, texty, 2); x += iconspace;
			IconRelative(pDC, x, texty, 3); x += iconspace;
			IconRelative(pDC, x, texty, 4);

			pDC->SetBitmapColor(MCOLOR(255, 255, 255, 255));

			x = 0.705f + (float(pFont->GetHeight() * 1.3f) / MGetWorkspaceWidth());
			sprintf(szText, "%d", pItem->nAllKill);
			TextRelative(pDC, x, texty, szText); x += iconspace;
			sprintf(szText, "%d", pItem->nUnbelievable);
			TextRelative(pDC, x, texty, szText); x += iconspace;
			sprintf(szText, "%d", pItem->nExcellent);
			TextRelative(pDC, x, texty, szText); x += iconspace;
			sprintf(szText, "%d", pItem->nFantastic);
			TextRelative(pDC, x, texty, szText); x += iconspace;
			sprintf(szText, "%d", pItem->nHeadShot);
			TextRelative(pDC, x, texty, szText); x += iconspace;

			//		y+=linespace;
		}
	}
}

void ZCombatInterface::IconRelative(MDrawContext* pDC, float x, float y, int nIcon)
{
	MBitmap *pbmp = m_ppIcons[nIcon];
	if (!pbmp) return;

	pDC->SetBitmap(pbmp);
	int screenx = x*MGetWorkspaceWidth();
	int screeny = y*MGetWorkspaceHeight();

	int nSize = pDC->GetFont()->GetHeight();
	pDC->Draw(screenx, screeny, nSize, nSize);
}

void ZCombatInterface::Finish()
{
	if (IsFinish())
		return;

	ZGetFlashBangEffect()->End();

	m_fOrgMusicVolume = ZApplication::GetSoundEngine()->GetMusicVolume();
	m_nReserveFinishTime = timeGetTime();
	m_bReserveFinish = true;

	m_CrossHair.Show(false);

#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->Set3DSoundUpdate(false);
#endif

}

bool ZCombatInterface::IsFinish()
{
	//	if(m_pResultPanel)
	//		return m_pResultPanel->GetVMesh()->isOncePlayDone();
	//	return false;

	return m_bOnFinish;
}

void ZCombatInterface::OnFinish()
{
	if (m_pResultPanel) return;

	m_pResultLeft = NULL;
	m_pResultRight = NULL;

	ZGetScreenEffectManager()->AddRoundFinish();

	//	m_pResultPanel=ZGetScreenEffectManager()->CreateScreenEffect("ef_in_result.elu");

	if (ZGetGame()->GetMatch()->IsTeamPlay() && !ZGetGameClient()->IsLadderGame() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SPY)
	{
		int nRed = ZGetGame()->GetMatch()->GetTeamScore(MMT_RED), nBlue = ZGetGame()->GetMatch()->GetTeamScore(MMT_BLUE);
		if (nRed == nBlue)
			m_pResultPanel_Team = ZGetScreenEffectManager()->CreateScreenEffect("teamdraw");
		else
			if (nRed>nBlue)
				m_pResultPanel_Team = ZGetScreenEffectManager()->CreateScreenEffect("teamredwin");
			else
				m_pResultPanel_Team = ZGetScreenEffectManager()->CreateScreenEffect("teambluewin");
	}

	m_ResultItems.Destroy();

	ZCharacterManager::iterator itor;
	for (itor = ZGetCharacterManager()->begin();
		itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		ZResultBoardItem *pItem = new ZResultBoardItem;

		if (pCharacter->IsAdminHide()) continue;

		sprintf(pItem->szName, "%d%s %s", pCharacter->GetProperty()->nLevel, ZMsg(MSG_CHARINFO_LEVELMARKER), pCharacter->GetUserName());
#ifdef _MASKLEVELGM
		if (pCharacter->IsAdminName()) 
		{
			sprintf(pItem->szName, "--%s  %s", ZMsg(MSG_CHARINFO_LEVELMARKER), pCharacter->GetUserName());
		}
#endif
		strcpy(pItem->szClan, pCharacter->GetProperty()->GetClanName());
		pItem->nClanID = pCharacter->GetClanID();
		pItem->nTeam = ZGetGame()->GetMatch()->IsTeamPlay() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SPY ? pCharacter->GetTeamID() : MMT_END;
		pItem->nScore = pCharacter->GetStatus().Ref().nExp;
		pItem->nKills = pCharacter->GetStatus().Ref().nKills;
		pItem->nDeaths = pCharacter->GetStatus().Ref().nDeaths;

		pItem->nAllKill = pCharacter->GetStatus().Ref().nAllKill;
		pItem->nExcellent = pCharacter->GetStatus().Ref().nExcellent;
		pItem->nFantastic = pCharacter->GetStatus().Ref().nFantastic;
		pItem->nHeadShot = pCharacter->GetStatus().Ref().nHeadShot;
		pItem->nUnbelievable = pCharacter->GetStatus().Ref().nUnbelievable;

		pItem->bMyChar = pCharacter->IsHero();

		MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(pCharacter->GetUID());
		if (pCache)
			pItem->bGameRoomUser = (pCache->GetPGrade() == MMPG_PREMIUM_IP) ? true : false;
		else
			pItem->bGameRoomUser = false;

		m_ResultItems.push_back(pItem);
	}

	m_Observer.Show(false);

	m_nReservedOutTime = timeGetTime() + 5000;		// 5초 후에 자동 종료.
	m_bOnFinish = true;
}

void ZCombatInterface::SetObserverMode(bool bEnable)
{
	if (bEnable) ZGetScreenEffectManager()->ResetSpectator();
	m_Observer.Show(bEnable);
}


ZCharacter* ZCombatInterface::GetTargetCharacter()
{
	if (m_Observer.IsVisible())
	{
		return m_Observer.GetTargetCharacter();
	}

	return ZGetGame()->m_pMyCharacter;
}

MUID ZCombatInterface::GetTargetUID()
{
	return GetTargetCharacter()->GetUID();
}


void ZCombatInterface::GameCheckPickCharacter()
{
	MPOINT Cp = GetCrosshairPoint();

	ZPICKINFO pickinfo;

	rvector pos, dir;
	RGetScreenLine(Cp.x, Cp.y, &pos, &dir);

	ZMyCharacter* pMyChar = NULL;

	pMyChar = ZGetGame()->m_pMyCharacter;

	bool bPick = false;

	if (ZGetGame()->Pick(pMyChar, pos, dir, &pickinfo, RM_FLAG_ADDITIVE | RM_FLAG_HIDE, true)) {

		if (pickinfo.pObject)	{
			if (pickinfo.info.parts == eq_parts_head) bPick = true;
			bPick = true;
		}
	}

	if (pMyChar && pMyChar->m_pVMesh) {

		RWeaponMotionType type = (RWeaponMotionType)pMyChar->m_pVMesh->GetSetectedWeaponMotionID();

		if ((type == eq_wd_katana) || (type == eq_wd_grenade) || (type == eq_ws_dagger) || (type == eq_wd_dagger)
			|| (type == eq_wd_item) || (type == eq_wd_sword) || (type == eq_wd_blade) || (type == eq_wd_spycase))
		{
			bPick = false;
		}

		if (pMyChar->m_pVMesh->m_vRotXYZ.y  > -20.f &&  pMyChar->m_pVMesh->m_vRotXYZ.y < 30.f) {
			bPick = false;
		}

		if (pMyChar->m_pVMesh->m_vRotXYZ.y < -25.f)
			bPick = true;

		if (pMyChar->IsMan()) { // 모델이 남자고
			if (pMyChar->m_pVMesh->m_vRotXYZ.x < -20.f) {//오른쪽으로 이동중에
				if (RCameraDirection.z < -0.2f)
					bPick = true;
			}
		}
		if ((pMyChar->m_AniState_Lower.Ref() == ZC_STATE_LOWER_TUMBLE_RIGHT) ||
			(pMyChar->m_AniState_Lower.Ref() == ZC_STATE_LOWER_TUMBLE_LEFT))
		{
			if (RCameraDirection.z < -0.3f)
				bPick = true;
		}


		if (RCameraDirection.z < -0.6f)
			bPick = true;

		#ifdef _CHARACTERVISIBLE
		{
			/*if(bPick) 
			{
			pMyChar->m_pVMesh->SetVisibility(0.4f);
			} else 
			{
			pMyChar->m_pVMesh->SetVisibility(1.0f);
			}*/
		}
        #else
		{
			if (bPick) 
			{
				pMyChar->m_pVMesh->SetVisibility(0.4f);
			}
			else
			{
				pMyChar->m_pVMesh->SetVisibility(1.0f);
			}
		}
#endif

		//Custom: Respawn Trasparente with time animation By Jorklenis2
		if (ZGetGame())
		{
			for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
			{
				ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
				int zsTimeTras = (timeGetTime() / 50 % 3);
				int zsRespawnTrasp = 0;
				for (zsRespawnTrasp = 0; zsRespawnTrasp < zsTimeTras; zsRespawnTrasp++);
				if (pCharacter && pCharacter->m_pVMesh)
				{
					if (pCharacter->isInvincible() && zsRespawnTrasp == 1)
					{
						pCharacter->m_pVMesh->SetVisibility(0.0f);
					}
					if (pCharacter->isInvincible() && zsRespawnTrasp == 2)
					{
						pCharacter->m_pVMesh->SetVisibility(0.5f);
					}
					if (pCharacter->isInvincible() && zsRespawnTrasp == 3)
					{
						pCharacter->m_pVMesh->SetVisibility(1.0f);
					}
				}
			}
		}

	if (ZGetGame()->Pick(pMyChar, pos, dir, &pickinfo))
	{
		if (pickinfo.pObject)
		{
			rvector v1;
			v1 = pickinfo.info.vOut;

			if (IsPlayerObject(pickinfo.pObject)) {
				SetPickTarget(true, (ZCharacter*)pickinfo.pObject);
			}
			else
			{
				m_CrossHair.SetState(ZCS_PICKENEMY);
			}
		}
		else
			SetPickTarget(false);
	}
  }
}

void ZCombatInterface::OnGadget(MMatchWeaponType nWeaponType)
{
	if (m_pWeaponScreenEffect) m_pWeaponScreenEffect->OnGadget(nWeaponType);
	m_CrossHair.Show(false);
}

void ZCombatInterface::OnGadgetOff()
{
	if (m_pWeaponScreenEffect) m_pWeaponScreenEffect->OnGadgetOff();
	m_CrossHair.Show(true);
}


void ZCombatInterface::SetDrawLeaveBattle(bool bShow, int nSeconds)
{
	m_bDrawLeaveBattle = bShow;
	m_nDrawLeaveBattleSeconds = nSeconds;
}

void ZCombatInterface::OnAddCharacter(ZCharacter *pChar)
{
	bool bClanGame = ZGetGameClient()->IsLadderGame();
	if (bClanGame) {
		if (pChar->GetTeamID() == MMT_RED) {
			if (m_nClanIDRed == 0) {
				m_nClanIDRed = pChar->GetClanID();
				ZGetEmblemInterface()->AddClanInfo(m_nClanIDRed);
				strcpy(m_szRedClanName, pChar->GetProperty()->GetClanName());
			}
		}
		else if (pChar->GetTeamID() == MMT_BLUE) {
			if (m_nClanIDBlue == 0) {
				m_nClanIDBlue = pChar->GetClanID();
				ZGetEmblemInterface()->AddClanInfo(m_nClanIDBlue);
				strcpy(m_szBlueClanName, pChar->GetProperty()->GetClanName());
			}
		}
	}
}

void ZCombatInterface::ShowChatOutput(bool bShow)
{
	m_Chat.ShowOutput(bShow);
	ZGetConfiguration()->SetViewGameChat(bShow);
}

void ZCombatInterface::DrawAfterWidgets(MDrawContext* pDC)
{
	// 듀얼토너먼트 결과창용 대진표를 그린다.
	if (m_bShowResult)
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
			((ZRuleDuelTournament*)ZGetGame()->GetMatch()->GetRule())->ShowMatchOrder(pDC, true, m_fElapsed);
}
void ZCombatInterface::UpdateCTFMsg(const char* szMsg)
{
	g_bShowCTFMsg = true;
	g_dwCTFMsgShowTime = GetTickCount();
	strcpy_s(g_szCTFMsg, szMsg);
}
void ZCombatInterface::OnInvalidate()
{
	ZCharacterView* pCharView = GetWidgetCharViewLeft();
	if (pCharView)
		pCharView->OnInvalidate();
	pCharView = GetWidgetCharViewRight();
	if (pCharView)
		pCharView->OnInvalidate();
	pCharView = GetWidgetCharViewResult();
	if (pCharView)
		pCharView->OnInvalidate();
}

void ZCombatInterface::OnRestore()
{
	ZCharacterView* pCharView = GetWidgetCharViewLeft();
	if (pCharView)
		pCharView->OnRestore();
	pCharView = GetWidgetCharViewRight();
	if (pCharView)
		pCharView->OnRestore();
	pCharView = GetWidgetCharViewResult();
	if (pCharView)
		pCharView->OnRestore();
}

////////// Spy mode ////////// 
void ZCombatInterface::OnSpyCreate()
{
	CreateSpyWaitInterface();
}

void ZCombatInterface::OnSpyDestroy()
{
	ZIDLResource* pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	MFrame* pFrame = (MFrame*)pIDLResource->FindWidget("CombatSpyInfo");
	if (pFrame)
		pFrame->Show(false);

	ZBmNumLabel* pNumLabel = (ZBmNumLabel*)pIDLResource->FindWidget("Spy_CountDownTime");
	if (pNumLabel)
		pNumLabel->Show(false);

	pNumLabel = (ZBmNumLabel*)pIDLResource->FindWidget("Spy_CountDownTime_Red");
	if (pNumLabel)
		pNumLabel->Show(false);

	SetSpyEventMsg(NULL);
	SetSpyTip(NULL);
	SetSpyNotice(NULL);
}

void ZCombatInterface::CreateSpyGameInterface()
{
	ZIDLResource* pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	MFrame* pFrame = (MFrame*)pIDLResource->FindWidget("CombatSpyInfo");
	if (pFrame)
		pFrame->Show(true);

	ZBmNumLabel* pNumLabel = (ZBmNumLabel*)pIDLResource->FindWidget("Spy_CountDownTime");
	if (pNumLabel)
		pNumLabel->Show(true);

	pNumLabel = (ZBmNumLabel*)pIDLResource->FindWidget("Spy_CountDownTime_Red");
	if (pNumLabel)
		pNumLabel->Show(false);

	SetSpyEventMsg(NULL);
	SetSpyTip(NULL);
	SetSpyNotice(NULL);

	m_bSpyLocationOpened = false;
}

void ZCombatInterface::CreateSpyWaitInterface()
{
	ZIDLResource* pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	MFrame* pFrame = (MFrame*)pIDLResource->FindWidget("CombatSpyInfo");
	if (pFrame)
		pFrame->Show(true);

	ZBmNumLabel* pNumLabel = (ZBmNumLabel*)pIDLResource->FindWidget("Spy_CountDownTime");
	if (pNumLabel)
		pNumLabel->Show(false);

	pNumLabel = (ZBmNumLabel*)pIDLResource->FindWidget("Spy_CountDownTime_Red");
	if (pNumLabel)
		pNumLabel->Show(false);

	SetSpyEventMsg(NULL);

	char minPlayers[32];
	sprintf(minPlayers, "%d", MMatchSpyMode::GetMinPlayers());

	char tip[256];
	ZTransMsg(tip, MSG_SPY_WAITING_FOR_PLAYERS, 4, minPlayers);

	SetSpyTip(tip);

	SetSpyNotice("Spy_Notice_Wait_OtherPlayer.png");

	m_bSpyLocationOpened = false;
}

void ZCombatInterface::OnSpyUpdate(float fElapsed)
{
	DWORD dwNowTime = timeGetTime();

	int nRemainedTime = (int)(m_dwSpyTimer - dwNowTime);
	SetSpyTimeLimitValue((DWORD)((nRemainedTime < 0) ? (0) : (nRemainedTime)));

	if (ZGetGame())
	{
		if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
		{
			const MMatchSpyMapNode* pSpyMap = MMatchSpyMap::GetMap(ZGetGame()->GetMatch()->GetMapName());
			_ASSERT(pSpyMap != NULL);

			if (((nRemainedTime / 1000) <= pSpyMap->nSpyOpenTime))
			{
#define SPYTIME_FLASHING_INTERVAL	300

				bool bRedTime = ((nRemainedTime % (SPYTIME_FLASHING_INTERVAL * 2)) < SPYTIME_FLASHING_INTERVAL);

				ZBmNumLabel* pNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime");
				if (pNumLabel)
					pNumLabel->Show(!bRedTime);

				pNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime_Red");
				if (pNumLabel)
					pNumLabel->Show(bRedTime);

				if (!m_bSpyLocationOpened)
				{
					if (ZGetScreenEffectManager())
						ZGetScreenEffectManager()->AddScreenEffect("spy_location");

					m_bSpyLocationOpened = true;

					SetDefaultSpyTip(ZGetGame()->m_pMyCharacter->GetTeamID());
				}
			}
		}
	}

	if (m_bSpyNoticePlaying)
	{
		MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_Notice");
		if (pPicture)
		{
#define SPYNOTICE_PLAYER_WAITTIME	1000
#define SPYNOTICE_PLAYER_FADETIME	300
#define SPYNOTICE_PLAYER_SHOWTIME	3000

			/*
			DWORD dwPlayTime = dwNowTime - m_dwSpyNoticePlayStartedTime;

			if(dwPlayTime < SPYNOTICE_PLAYER_WAITTIME)
			{
			pPicture->SetOpacity(0);
			}
			else if(dwPlayTime < (SPYNOTICE_PLAYER_WAITTIME + SPYNOTICE_PLAYER_FADETIME))
			{
			float fOpacity = ((float)dwPlayTime / ((float)255 / (float)SPYNOTICE_PLAYER_FADETIME));
			if(fOpacity < 0.f) fOpacity = 0.f;

			pPicture->SetOpacity((unsigned char)fOpacity);
			}
			else if(dwPlayTime < (SPYNOTICE_PLAYER_WAITTIME + SPYNOTICE_PLAYER_FADETIME + SPYNOTICE_PLAYER_SHOWTIME))
			{
			pPicture->SetOpacity(255);
			}
			else if(dwPlayTime < (SPYNOTICE_PLAYER_WAITTIME + SPYNOTICE_PLAYER_FADETIME + SPYNOTICE_PLAYER_SHOWTIME + SPYNOTICE_PLAYER_FADETIME))
			{
			int fOpacity = (((float)SPYNOTICE_PLAYER_FADETIME - (float)dwPlayTime) / ((float)SPYNOTICE_PLAYER_FADETIME / (float)255));
			if(fOpacity > 255.f) fOpacity = 255.f;

			pPicture->SetOpacity((unsigned char)fOpacity);
			}
			else
			{
			pPicture->Show(false);
			pPicture->SetBitmap(NULL);
			}
			*/

			DWORD dwPlayTime = dwNowTime - m_dwSpyNoticePlayStartedTime;

			if (dwPlayTime >= (SPYNOTICE_PLAYER_WAITTIME + SPYNOTICE_PLAYER_FADETIME + SPYNOTICE_PLAYER_SHOWTIME + SPYNOTICE_PLAYER_FADETIME))
			{
				pPicture->Show(false);
				pPicture->SetBitmap(NULL);
			}
			else if (dwPlayTime >= (SPYNOTICE_PLAYER_WAITTIME + SPYNOTICE_PLAYER_FADETIME + SPYNOTICE_PLAYER_SHOWTIME))
			{
				pPicture->SetOpacity(0);
			}
			else if (dwPlayTime >= (SPYNOTICE_PLAYER_WAITTIME + SPYNOTICE_PLAYER_FADETIME))
			{
				pPicture->SetOpacity(255);
			}
		}
	}
}

void ZCombatInterface::OnSpyDraw(MDrawContext* pDC)
{
	if (!ZGetGame())
	{
		_ASSERT(0);
		return;
	}

	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		int nElapsedTime = (int)ZGetGame()->GetMatch()->GetRemaindTime();

		if (nElapsedTime >= 2500 && nElapsedTime < 7500)
		{
			if (!m_Observer.IsVisible())
			{
				MFont* pFont = MFontManager::Get("FONTa12_O2Wht");
				_ASSERT(pFont != NULL);

				pDC->SetFont(pFont);
				pDC->Text(MRECT(MGetWorkspaceWidth() / 2, MGetWorkspaceHeight() / 2, 0, 0), ZMsg(MSG_SPY_IDENTITY), MAM_HCENTER | MAM_VCENTER);

				int nFontHeight = pFont->GetHeight();

				pFont = MFontManager::Get("FONTa12_O1Red");
				_ASSERT(pFont != NULL);

				char szSpyName[256] = "";

				for (ZCharacterManager::iterator it = ZGetGame()->m_CharacterManager.begin(); it != ZGetGame()->m_CharacterManager.end(); it++)
				{
					ZCharacter* pCharacter = (*it).second;

					if (pCharacter->GetTeamID() == MMT_RED)
					{
						strcat(szSpyName, pCharacter->GetUserName());
						strcat(szSpyName, " ");
					}
				}

				int nTextLen = (int)strlen(szSpyName);

				if (szSpyName[nTextLen] == ' ')
					szSpyName[nTextLen] = '\0';

				pDC->SetFont(pFont);
				pDC->Text(MRECT(MGetWorkspaceWidth() / 2, MGetWorkspaceHeight() / 2 + nFontHeight, 0, 0), szSpyName, MAM_HCENTER | MAM_VCENTER);
			}
		}
	}
}

void ZCombatInterface::SetSpyTip(const char* msg)
{
	MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_Tip");
	if (pLabel)
	{
		if (pLabel->GetFont())
		{
			pLabel->SetSize(pLabel->GetFont()->GetWidth(msg), 20.f / 600.f * MGetWorkspaceHeight());
			pLabel->SetPosition(MGetWorkspaceWidth() / 2 - pLabel->GetRect().w / 2, 540.f / 600.f * MGetWorkspaceHeight());
		}

		if (msg)
		{
			pLabel->SetText(msg);
			pLabel->Show(true);
		}
		else
		{
			pLabel->SetText("");
			pLabel->Show(false);
		}
	}

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_TipBG");
	if (pPicture)
	{
		if (pLabel && pLabel->GetFont())
		{
			pPicture->SetSize(pLabel->GetFont()->GetWidth(msg) * 1.04f, 20.f / 600.f * MGetWorkspaceWidth());
			pPicture->SetPosition(MGetWorkspaceWidth() / 2 - pPicture->GetRect().w / 2, 540.f / 600.f * MGetWorkspaceHeight());
		}

		if (msg)
		{
			pPicture->SetOpacity(100);
			pPicture->Show(true);
		}
		else
		{
			pPicture->SetOpacity(255);
			pPicture->Show(false);
		}
	}
}

void ZCombatInterface::SetSpyEventMsg(const char* imgName)
{
	/*
	Spy_EventMsg_ComingSoonOpenSpy.png  :   for when Counting down.
	Spy_EventMsg_Survive.png            :   for Spy side.
	Spy_EventMsg_EliminateSpy.png       :   for Tracker side.
	*/

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_EventMsg");
	if (pPicture)
	{
		if (imgName)
		{
			MBitmap* pBitmap = (MBitmap*)MBitmapManager::Get(imgName);
			if (pBitmap)
			{
				pPicture->SetBitmap(pBitmap);
				pPicture->Show(true);
			}
			else _ASSERT(0);
		}
		else
		{
			pPicture->Show(false);
			pPicture->SetBitmap(NULL);
		}
	}
}

void ZCombatInterface::SetSpyNotice(const char* imgName)
{
	/*
	Spy_Notice_Wait_OtherPlayer.png    :   Waiting for other players.
	Spy_Notice_Tracer.png              :   You are selected as tracker.
	Spy_Notice_Spy.png                 :   You are selected as spy.
	*/

	m_bSpyNoticePlaying = false;

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_Notice");
	if (pPicture)
	{
		if (imgName)
		{
			MBitmap* pBitmap = (MBitmap*)MBitmapManager::Get(imgName);
			if (pBitmap)
			{
				pPicture->SetBitmap(pBitmap);
				pPicture->SetOpacity(255);
				pPicture->Show(true);
			}
			else _ASSERT(0);
		}
		else
		{
			pPicture->Show(false);
			pPicture->SetBitmap(NULL);
		}
	}
}

void ZCombatInterface::SetDefaultSpyTip(MMatchTeam team)
{
	if (m_Observer.IsVisible())
	{
		SetSpyTip(ZMsg(MSG_SPY_PARTICIPATION_GUIDANCE));
	}
	else
	{
		if (team == MMT_RED)
		{
			if (m_bSpyLocationOpened)
				SetSpyTip(ZMsg(MSG_SPY_SPYSIDE_SPY_LOCATION_OPEN));
			else
				SetSpyTip(ZMsg(MSG_SPY_TIP_SPYSIDE));
		}
		else
		{
			if (m_bSpyLocationOpened)
				SetSpyTip(ZMsg(MSG_SPY_TRACKERSIDE_SPY_LOCATION_OPEN));
			else
				SetSpyTip(ZMsg(MSG_SPY_TIP_TRACKERSIDE));
		}
	}
}

const char* ZCombatInterface::GetSuitableSpyItemTip(int itemid)
{
	switch (itemid)
	{
	case 601001:	// flashbang.
		return ZMsg(MSG_SPY_TIP_FLASH_BANG);
	case 601002:	// smoke.
		return ZMsg(MSG_SPY_TIP_SMOKE);
	case 601003:	// frozen trap.
		return ZMsg(MSG_SPY_TIP_FROZEN_TRAP);
	case 601004:	// stun grenade.
		return ZMsg(MSG_SPY_TIP_STUN_GRENADE);
	case 601005:	// landmine.
		return ZMsg(MSG_SPY_TIP_LANDMINE);
	}

	return NULL;	// nothing found.
}

void ZCombatInterface::SetSpyTimeLimitValue(int m, int s, int ms)
{
	char szText[256];
	sprintf(szText, "%d:%02d:%02d", m, s, ms);

	ZBmNumLabel* pNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime");
	if (pNumLabel)
		pNumLabel->SetText(szText);

	pNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime_Red");
	if (pNumLabel)
		pNumLabel->SetText(szText);
}

void ZCombatInterface::SetSpyTimeLimitValue(DWORD dwTime)
{
	int m = (int)(dwTime / 1000 / 60);
	int s = (int)(dwTime / 1000 % 60);
	int ms = (int)(dwTime % 1000 / 10);

	SetSpyTimeLimitValue(m, s, ms);
}

void ZCombatInterface::SetSpyTimer(DWORD dwTimeLimit)
{
	DWORD dwNowTime = timeGetTime();
	DWORD dwEndTime = dwNowTime + dwTimeLimit;

	SetSpyTimeLimitValue(dwTimeLimit);

	m_dwSpyTimer = dwEndTime;
}

void ZCombatInterface::PlaySpyNotice(const char* imgName)
{
	if (!imgName)
	{
		_ASSERT(0);
		return;
	}

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_Notice");
	if (pPicture)
	{
		MBitmap* pBitmap = (MBitmap*)MBitmapManager::Get(imgName);
		if (pBitmap)
		{
			pPicture->SetBitmap(pBitmap);
			pPicture->SetOpacity(0);
			pPicture->Show(true);
		}
		else _ASSERT(0);
	}

	m_dwSpyNoticePlayStartedTime = timeGetTime();
	m_bSpyNoticePlaying = true;
}

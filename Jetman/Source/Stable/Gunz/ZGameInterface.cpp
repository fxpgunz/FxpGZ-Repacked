#include "stdafx.h"

#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZPost.h"
#include "UPnP.h"
#include "ZConsole.h"
#include "MCommandLogFrame.h"
#include "ZConfiguration.h"
#include "FileInfo.h"
#include "ZInterfaceItem.h"
#include "MPicture.h"
#include "ZInterfaceListener.h"
#include "ZEffectSmoke.h"
#include "ZEffectLightTracer.h"
#include "MProfiler.h"
//#include "MActionKey.h"
#include "ZActionDef.h"
#include "MSlider.h"
#include "ZMsgBox.h"
#include "MDebug.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MListBox.h"
#include "MTextArea.h"
#include "MTabCtrl.h"
#include "MComboBox.h"
#include "ZInterfaceBackground.h"
#include "ZCharacterSelectView.h"
#include "ZCharacterViewList.h"
#include "ZCharacterView.h"
#include "MMatchStage.h"
#include "ZScreenEffectManager.h"
#include "RShaderMgr.h"
#include "ZShopEquipInterface.h"
#include "ZShopEquipListbox.h"
#include "ZMyItemList.h"
#include "ZMyInfo.h"
#include "ZStageSetting.h"
#include "ZInitialLoading.h"
#include "RShaderMgr.h"
#include "zeffectflashbang.h"
#include "MToolTip.h"
#include "ZRoomListbox.h"
#include "ZPlayerListBox.h"
#include "MMatchNotify.h"
#include "ZMapListBox.h"
#include "ZDuelTournamentRankingListBox.h"
#include "ZToolTip.h"
#include "ZCanvas.h"
#include "ZCrossHair.h"
#include "ZPlayerMenu.h"
#include "ZItemMenu.h"
#include "MPanel.h"
#include "ZNetRepository.h"
#include "ZStencilLight.h"
#include "MUtil.h"
#include "ZMap.h"
#include "ZBmNumLabel.h"
#include "ZItemSlotView.h"
#include "ZMapDesc.h"
#include "MStringTable.h"

#include "ZReplay.h"
#include "MFileDialog.h"
#include "ZServerView.h"
#include "ZLocale.h"

#include "ZLocatorList.h"
#include "ZSecurity.h"
#include "ZInput.h"
#include "ZActionKey.h"
#include "ZMonsterBookInterface.h"
#include "ZGameInput.h"
#include "ZOptionInterface.h"
#include "ZNHN_USA_Report.h"

#include "ZPlayerManager.h"
#include "ZShop.h"
#include "ZGambleItemDefine.h"
#include "ZCombatInterface.h"

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#endif
#include "ZMapCache.h"

extern MCommandLogFrame* m_pLogFrame;

static int g_debug_tex_update_cnt;
bool ZGameInterface::m_bSkipGlobalEvent = false;

bool ZGameInterface::m_sbRemainClientConnectionForResetApp = false;
ZGameClient* ZGameInterface::m_spGameClient = NULL;

void ZChangeGameState(GunzState state)
{
	PostMessage(g_hWnd, WM_CHANGE_GAMESTATE, int(state), 0);
}


void ZEmptyBitmap()
{
	MBitmapManager::Destroy();
	MBitmapManager::DestroyAniBitmap();
}

void ZGameInterface::LoadBitmaps(const char* szDir, const char* szSubDir, ZLoadingProgress *pLoadingProgress)
{
	// szSubDir(��eA���Ʃ���I)�������� ��OAu ��AAIA�� A�̡�i ����A����e szDir�������� ��I��uCN��U
	// (��eA���Ʃ���I������A ��c��eAU�Ƣ� ��E��C�������� ����AACN ������i��e ��n���碬EA�� ��O��i�쨢��U)
	mlog("start log bipmap\n");

	const char *loadExts[] = { ".png", ".bmp", ".tga" };

#define EXT_LEN 4

	MZFileSystem *pfs=ZGetFileSystem();

	int nDirLen = (int)strlen(szDir);

	int nTotalCount = 0;
	// �Ʃ���o��| ������aA��CN copy
	for(int i=0; i<pfs->GetFileCount(); i++){
		const char* szFileName = pfs->GetFileName(i);
		const MZFILEDESC* desc = pfs->GetFileDesc(i);
		int nLen = (int)strlen(szFileName);

		for(int j=0;j<sizeof(loadExts)/sizeof(loadExts[0]);j++) {
			// E��AaAU�Ƣ� ��AA����e..
			if( nLen>EXT_LEN && _stricmp(szFileName+nLen-EXT_LEN, loadExts[j])==0 )
			{
				bool bAddDirToAliasName = false;
				bool bMatch = false;
				// �Ʃ���I�Ƣ� ��A��������CN��U
				if(nDirLen==0 || strnicmp(desc->m_szFileName,szDir,nDirLen)==0)
					nTotalCount++;
				// custom crosshair
				if(strnicmp(desc->m_szFileName,PATH_CUSTOM_CROSSHAIR,strlen(PATH_CUSTOM_CROSSHAIR))==0)
					nTotalCount++;
			}
		}
	} // �Ʃ���o��| ������aA��CN A��CC

	int nCount = 0;
	for(int i=0; i<pfs->GetFileCount(); i++){

		const char* szFileName = pfs->GetFileName(i);
		const MZFILEDESC* desc = pfs->GetFileDesc(i);
		const MZFILEDESC* subDesc = NULL;
		int nLen = (int)strlen(szFileName);
		const char* szTargetFile = NULL;

		for(int j=0;j<sizeof(loadExts)/sizeof(loadExts[0]);j++) {
			// E��AaAU�Ƣ� ��AA����e..
			if( nLen>EXT_LEN && _stricmp(szFileName+nLen-EXT_LEN, loadExts[j])==0 )
			{
				bool bAddDirToAliasName = false;
				bool bMatch = false;
				// �Ʃ���I�Ƣ� ��A��������CN��U
				if(nDirLen==0 || strnicmp(desc->m_szFileName,szDir,nDirLen)==0)
					bMatch = true;
				// custom crosshair
				if(strnicmp(desc->m_szFileName,PATH_CUSTOM_CROSSHAIR,strlen(PATH_CUSTOM_CROSSHAIR))==0)
				{
					bMatch = true;
					bAddDirToAliasName = true;
				}

				if(bMatch) {
					nCount++;
					if(pLoadingProgress && nCount%10==0)
						pLoadingProgress->UpdateAndDraw((float)nCount/(float)nTotalCount);


					char aliasname[256];
					char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
					_splitpath(szFileName,drive,dir,fname,ext);

					if (!bAddDirToAliasName) sprintf(aliasname,"%s%s",fname,ext);
					else sprintf(aliasname, "%s%s%s", dir, fname,ext);

#ifdef _PUBLISH
					MZFile::SetReadMode( MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE );
#endif
					// ��eA�� �Ʃ���I���� �ơ�A�� AI����AC ��AAIAI A��AcCI��e ������IA�� ��I��u. ����A����e �쩣��u���� �Ʃ���I�������� ��I��u
					szTargetFile = desc->m_szFileName;

					MBitmapR2* pBitmap = new MBitmapR2;
					if(pBitmap->Create(aliasname, RGetDevice(), szTargetFile)==true)
						MBitmapManager::Add(pBitmap);
					else
						delete pBitmap;

#ifdef _PUBLISH
					MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
#endif

				}
			}
		}
	}

	mlog("end of load bitmaps2\n");
	//ZLoadBitmap(PATH_CUSTOM_CROSSHAIR, ".png", true);

}

/*
void InitHotBar(MWidget* pHotBar)
{
if(pHotBar==NULL) return;

#define HOTBAR_BTN_COUNT	10
#define HOTBAR_BTN_WIDTH	32
#define HOTBAR_SPINBTN_WIDTH	16
MRECT HotBarClientRect = pHotBar->GetClientRect();
MButton* pNew = new MButton("<", pHotBar, pHotBar);
pNew->SetBounds(HotBarClientRect.x, HotBarClientRect.y, HOTBAR_SPINBTN_WIDTH, HOTBAR_SPINBTN_WIDTH);
pNew = new MButton(">", pHotBar, pHotBar);
pNew->SetBounds(HotBarClientRect.x, HotBarClientRect.y+HOTBAR_SPINBTN_WIDTH, HOTBAR_SPINBTN_WIDTH, HOTBAR_SPINBTN_WIDTH);

for(int i=0; i<HOTBAR_BTN_COUNT; i++){
MButton* pNew = new MHotBarButton(NULL, pHotBar, &g_HotBarButtonListener);
pNew->SetBounds(HotBarClientRect.x+HOTBAR_SPINBTN_WIDTH+1+i*(HOTBAR_BTN_WIDTH+1), HotBarClientRect.y, HOTBAR_BTN_WIDTH, HOTBAR_BTN_WIDTH);
}
}
*/

void AddListItem(MListBox* pList, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	class MDragableListItem : public MDefaultListItem{
		char m_szDragItemString[256];
	public:
		MDragableListItem(MBitmap* pBitmap, const char* szText, const char* szItemString)
			: MDefaultListItem(pBitmap, szText){
				if(szItemString!=NULL) strcpy(m_szDragItemString, szItemString);
				else m_szDragItemString[0] = 0;
			}
			virtual bool GetDragItem(MBitmap** ppDragBitmap, char* szDragString, char* szDragItemString){
				*ppDragBitmap = GetBitmap(0);
				if(GetString(1)!=NULL) strcpy(szDragString, GetString(1));
				else szDragString[0] = 0;
				strcpy(szDragItemString, m_szDragItemString);
				return true;
			};
	};
	MDefaultListItem* pNew = new MDragableListItem(pBitmap, szString, szItemString);
	pList->Add(pNew);
}

bool InitSkillList(MWidget* pWidget)
{
	if(pWidget==NULL) return false;

	if(strcmp(pWidget->GetClassName(), MINT_LISTBOX)!=0) return false;
	MListBox* pList = (MListBox*)pWidget;

	pList->SetItemHeight(32);
	pList->SetVisibleHeader(false);

	pList->AddField("Icon", 32);
	pList->AddField("Name", 600);
	AddListItem(pList, MBitmapManager::Get("skill000.png"), "Fire-Ball", "Object.Skill $player $target 0");
	AddListItem(pList, MBitmapManager::Get("skill001.png"), "Bull-Shit", "Object.Skill $player $target 1");
	return true;
}

bool InitItemList(MWidget* pWidget)
{
	if(pWidget==NULL) return false;

	if(strcmp(pWidget->GetClassName(), MINT_LISTBOX)!=0) return false;
	MListBox* pList = (MListBox*)pWidget;

	//	pList->SetViewStyle(MVS_ICON);
	pList->SetVisibleHeader(false);
	pList->SetItemHeight(40);

	pList->AddField("Icon", 42);
	pList->AddField("Name", 600);
	// A���������硤I 30�Ʃ� ��O��i��oA��
	for (int i = 0; i < 30; i++)
	{
		char szName[256], szItem[256];
		int d = i % 6;
		sprintf(szItem, "item%03d.png", d);
		sprintf(szName, "Item Name %d", i);
		AddListItem(pList, MBitmapManager::Get(szItem), szName, "Command Something");
	}

	return true;
}

#define DEFAULT_SLIDER_MAX			10000

ZGameInterface::ZGameInterface(const char* szName, MWidget* pParent, MListener* pListener) : ZInterface(szName,pParent,pListener)
{
	MSetString( 1, ZMsg(MSG_MENUITEM_OK));
	MSetString( 2, ZMsg(MSG_MENUITEM_CANCEL));
	MSetString( 3, ZMsg(MSG_MENUITEM_YES));
	MSetString( 4, ZMsg(MSG_MENUITEM_NO));
	MSetString( 5, ZMsg(MSG_MENUITEM_MESSAGE));

	m_pShopEquipInterface = new ZShopEquipInterface;

	m_bShowInterface = true;
	m_bViewUI = true;
	m_bWaitingArrangedGame = false;

	m_pMyCharacter = NULL;

	SetBounds(0, 0, MGetWorkspaceWidth(), MGetWorkspaceHeight());

	m_pGame = NULL;
	m_pCombatInterface = NULL;
	m_pLoadingInterface = NULL;

	m_dwFrameMoveClock = 0;

	m_nInitialState = GUNZ_LOGIN;
	m_nPreviousState = GUNZ_LOGIN;

	m_nState = GUNZ_NA;

	m_bCursor = true;
	MCursorSystem::Show(m_bCursor);

	//m_bCursor = false;
	m_bLogin = false;
	m_bLoading = false;

	m_pRoomListFrame = NULL;
	m_pDuelTournamentLobbyFrame = NULL;
	m_pBottomFrame = NULL;
	m_pClanInfoBg = NULL;
	m_pDuelTournamentInfoBg = NULL;
	m_pDuelTournamentRankingLabel = NULL;

	m_pMsgBox = new ZMsgBox("", Mint::GetInstance()->GetMainFrame(), this, MT_OK);
	m_pConfirmMsgBox = new ZMsgBox("", Mint::GetInstance()->GetMainFrame(), this, MT_YESNO);

	m_pMonsterBookInterface = new ZMonsterBookInterface();

//	ZApplication::GetStageInterface()->SetMapName( MMATCH_DEFAULT_STAGESETTING_MAPNAME);
	m_pMapThumbnail=NULL;

	m_pCharacterSelectView = NULL;
	m_pBackground = new ZInterfaceBackground();

	m_pCursorSurface=NULL;
	
	if (!m_sbRemainClientConnectionForResetApp)
		m_spGameClient=NULL;

	m_nDrawCount = 0;

	m_bTeenVersion = true;
//	m_pAmbSound = NULL;
//	m_bisReserveChangeWeapon = false;
//	m_nReserveChangeWeapon = ZCWT_MELEE;

	m_pScreenEffectManager = NULL;
	m_pEffectManager = NULL;
	m_pGameInput = NULL;
	
	m_bReservedWeapon = false;
	m_ReservedWeapon = ZCWT_NONE;

	m_bLeaveBattleReserved = false;
	m_bLeaveStageReserved = false;

	Mint::GetInstance()->SetGlobalEvent(ZGameInterface::OnGlobalEvent);
	ZGetInput()->SetEventListener(ZGameInterface::OnGlobalEvent);

	m_pPlayerMenu = NULL;
	m_pMiniMap = NULL;

    m_bOnEndOfReplay = false;
	m_nLevelPercentCache = 0;

	m_pLoginBG = NULL;
	m_pLoginPanel = NULL;

	m_nLoginState = LOGINSTATE_STANDBY;
	m_dwLoginTimer = 0;

	m_nLocServ = 0;

	m_dwHourCount = 0;
	m_dwTimeCount = timeGetTime() + 3600000;

	// Lobby Bitmap
	if ( m_pRoomListFrame != NULL)
	{
        delete m_pRoomListFrame;
		m_pRoomListFrame = NULL;
	}
	if (m_pDuelTournamentLobbyFrame != NULL)
	{
		delete m_pDuelTournamentLobbyFrame;
		m_pDuelTournamentLobbyFrame = NULL;
	}
	if ( m_pBottomFrame != NULL)
	{
		delete m_pBottomFrame;
		m_pBottomFrame = NULL;
	}	
	if ( m_pClanInfoBg != NULL)
	{
		delete m_pClanInfoBg;
		m_pClanInfoBg = NULL;
	}
	if ( m_pDuelTournamentInfoBg != NULL)
	{
		delete m_pDuelTournamentInfoBg;
		m_pDuelTournamentInfoBg = NULL;
	}
	if ( m_pDuelTournamentRankingLabel != NULL)
	{
		delete m_pDuelTournamentRankingLabel;
		m_pDuelTournamentRankingLabel = NULL;
	}

	// Login Bitmap
	if ( m_pLoginBG != NULL)
	{
		delete m_pLoginBG;
		m_pLoginBG = NULL;
	}

	if ( m_pLoginPanel != NULL)
	{
		delete m_pLoginPanel;
		m_pLoginPanel = NULL;
	}

	m_dwRefreshTime = 0;

	m_pLocatorList = ZGetConfiguration()->GetLocatorList();
	m_pTLocatorList = ZGetConfiguration()->GetTLocatorList();

	// ����AI���� ��u��A AE��aE��
	m_dwVoiceTime = 0;
	m_szCurrVoice[ 0] = 0;
	m_szNextVoice[ 0] = 0;
	m_dwNextVoiceTime = 0;

	m_bReservedQuit = false;
	m_bReserveResetApp = false;

	m_dErrMaxPalyerDelayTime = 0;
	m_bErrMaxPalyer = false;

	m_bGameFinishLeaveBattle = true;

	m_MyPort = 0;
}

ZGameInterface::~ZGameInterface()
{
	ZEmptyBitmap();

	OnDestroy();

	SAFE_DELETE(m_pMiniMap);
	SAFE_RELEASE(m_pCursorSurface);
	SAFE_DELETE(m_pScreenEffectManager);
	SAFE_DELETE(m_pEffectManager);
	SAFE_DELETE(m_pCharacterSelectView);
	SAFE_DELETE(m_pBackground);
	SAFE_DELETE(m_pMsgBox);
	SAFE_DELETE(m_pConfirmMsgBox);
	SAFE_DELETE(m_pMonsterBookInterface);

	SAFE_DELETE(m_pRoomListFrame);
	SAFE_DELETE(m_pDuelTournamentLobbyFrame);
	SAFE_DELETE(m_pBottomFrame);
	SAFE_DELETE(m_pClanInfoBg);
	SAFE_DELETE(m_pDuelTournamentInfoBg);
	SAFE_DELETE(m_pDuelTournamentRankingLabel);

	SAFE_DELETE(m_pLoginPanel);

	SAFE_DELETE(m_pShopEquipInterface);
}


bool ZGameInterface::InitInterface(const char* szSkinName, ZLoadingProgress *pLoadingProgress)
{
	DWORD _begin_time,_end_time;
#define BEGIN_ { _begin_time = timeGetTime(); }
#define END_(x) { _end_time = timeGetTime(); float f_time = (_end_time - _begin_time) / 1000.f; mlog("%s : %f \n", x,f_time ); }


	SetObjectTextureLevel(ZGetConfiguration()->GetVideo()->nCharTexLevel);
	//SetMapTextureLevel(ZGetConfiguration()->GetVideo()->nMapTexLevel);
	if (!ZGetConfiguration()->GetVideo()->bArchetype)
	{
		SetMapTextureLevel(ZGetConfiguration()->GetVideo()->nMapTexLevel);
	}
	else
	{
		SetMapTextureLevel(8);
	}
	SetEffectLevel(ZGetConfiguration()->GetVideo()->nEffectLevel);
	SetTextureFormat(ZGetConfiguration()->GetVideo()->nTextureFormat);

	SetBandiCaptureConfig(ZGetConfiguration()->GetMovingPicture()->iResolution);	// �좯�����o A��AA C����o��� ������A
	SetBandiCaptureFileSize(ZGetConfiguration()->GetMovingPicture()->iFileSize);		// �좯�����o A��AA ��AAIA�ϡ�a(��e����A|CN)

	bool bRet = true;
	char szPath[256];
//	char szSubPath[256] = "interface/grm/default/";	// ������i��?�Ʃ�A�� A��CN ��eA�� �Ʃ���I
	char szFileName[256];
	char a_szSkinName[256];
	strcpy(a_szSkinName, szSkinName);

	ZGetInterfaceSkinPath(szPath, a_szSkinName);
	//ZGetInterfaceSkinPathSubLanguage(szSubPath, a_szSkinName);
	sprintf(szFileName, "%s%s", szPath, FILENAME_INTERFACE_MAIN);

	ZEmptyBitmap();

	ZLoadingProgress pictureProgress("pictures",pLoadingProgress,.7f);
	BEGIN_;
	//ZLoadBitmap(szPath, ".png");
	//ZLoadBitmap(szPath, ".bmp");
	//ZLoadBitmap(szPath, ".tga");
	////	ZLoadBitmap(szPath, ".dds");
	//ZLoadBitmap(PATH_CUSTOM_CROSSHAIR, ".png", true);
	LoadBitmaps(szPath, "", &pictureProgress);

	END_("loading pictures");

	BEGIN_;
	if (!m_IDLResource.LoadFromFile(szFileName, this, ZGetFileSystem()))
	{
		// ��I��a ��C����CI��e Default��I ��I��a
		strcpy(a_szSkinName, DEFAULT_INTERFACE_SKIN);
		ZGetInterfaceSkinPath(szPath, a_szSkinName);
		sprintf(szFileName, "%s%s", szPath, FILENAME_INTERFACE_MAIN);
		//ZLoadBitmap(szPath, ".png");
		//ZLoadBitmap(szPath, ".bmp");
		//ZLoadBitmap(szPath, ".tga");
		//ZLoadBitmap(szPath, ".dds");
		LoadBitmaps(szPath, "", &pictureProgress);

		if (m_IDLResource.LoadFromFile(szFileName, this, ZGetFileSystem()))
		{
			mlog("IDLResource Loading Success!!\n");
		}
		else
		{
			mlog("IDLResource Loading Failed!!\n");
		}
		bRet = false;
	}
	else
	{
		mlog("IDLResource Loading Success!!\n");
	}
	END_("IDL resources");

// ��UAI��E��I���� look ������A
//	MBFrameLook* pFrameLook = (MBFrameLook*)m_IDLResource.FindFrameLook("Custom1FrameLook");
	MBFrameLook* pFrameLook = (MBFrameLook*)m_IDLResource.FindFrameLook("DefaultFrameLook");
	if (pFrameLook != NULL)
	{
		m_pMsgBox->ChangeCustomLook((MFrameLook*)pFrameLook);
		m_pConfirmMsgBox->ChangeCustomLook((MFrameLook*)pFrameLook);
	}
	else
	{
		_ASSERT(0);
	}

	ZStageSetting::InitStageSettingDialog();

// EquipmentListBox AI���ͨ��� ������A
	// ��oA������AO��A��I
	ZShopEquipListbox* pShopEquipListBox = (ZShopEquipListbox*)m_IDLResource.FindWidget("AllEquipmentList");
	if (pShopEquipListBox)
	{
		pShopEquipListBox->SetOnDropCallback(ShopPurchaseItemListBoxOnDrop);
	}
	pShopEquipListBox = (ZShopEquipListbox*)m_IDLResource.FindWidget("MyAllEquipmentList");
	if (pShopEquipListBox)
	{
		pShopEquipListBox->SetOnDropCallback(ShopSaleItemListBoxOnDrop);
	}
	//pShopEquipListBox = (ZShopEquipListbox*)m_IDLResource.FindWidget("CashEquipmentList");
	//if (pShopEquipListBox)
	//{
	//	pShopEquipListBox->SetOnDropCallback(ShopPurchaseLadderItemListBoxOnDrop);
	//}

	// Aa��n���� AO��A��I
	pShopEquipListBox = (ZShopEquipListbox*)m_IDLResource.FindWidget("EquipmentList");
	if (pShopEquipListBox)
	{
		pShopEquipListBox->SetOnDropCallback(CharacterEquipmentItemListBoxOnDrop);
	}

	// ��eA������ AO��A��I
	pShopEquipListBox = (ZShopEquipListbox*)m_IDLResource.FindWidget("AccountItemList");
	if (pShopEquipListBox)
	{
//		pShopEquipListBox->SetOnDropCallback(CharacterEquipmentItemListBoxOnDrop);
	}

	// ����AIAU ������i textareaAC A������AO ��eA�� AoA��
	m_textAreaLookItemDesc.SetBgColor(MCOLOR(10, 10, 10, 220));

	MTextArea* pTextArea = (MTextArea*)m_IDLResource.FindWidget("Shop_ItemDescription");
	if (pTextArea) pTextArea->ChangeCustomLook(&m_textAreaLookItemDesc);
	pTextArea = (MTextArea*)m_IDLResource.FindWidget("Equip_ItemDescription");
	if (pTextArea) pTextArea->ChangeCustomLook(&m_textAreaLookItemDesc);
/*
	MPanel* pPanel = new MPanel();
	pPanel->SetPosition(10, 10);
	pPanel->SetSize(300, 300);
*/
	InitInterfaceListener();

	// CenterMessage �Ƣ���i��� A����A
#define CENTERMESSAGE	"CenterMessage"
	BEGIN_WIDGETLIST(CENTERMESSAGE, &m_IDLResource, MLabel*, pWidget);
	pWidget->SetAlignment(MAM_HCENTER);
	END_WIDGETLIST();

	ZGetOptionInterface()->InitInterfaceOption();

#ifdef _FASTDEBUG
	m_IDLResource.FindWidget("LoginPassword")->SetText("1111");
//	m_IDLResource.FindWidget("LoginPassword")->SetText("2708");
#endif
	InitMaps(m_IDLResource.FindWidget("MapSelection"));

	_ASSERT(m_pPlayerMenu==NULL);
	// Player Menu
	m_pPlayerMenu = new ZPlayerMenu("PlayerMenu", this, this, MPMT_VERTICAL);
	((MPopupMenu*)m_pPlayerMenu)->Show(false);

	return true;
}

bool ZGameInterface::InitInterfaceListener()
{
	// ��UAI��E��I����
	m_pMsgBox->SetListener(ZGetMsgBoxListener());
	m_pConfirmMsgBox->SetListener(ZGetConfirmMsgBoxListener());


	// ��E��C
	SetListenerWidget("VideoGamma", ZGetOptionGammaSliderChangeListener());
	SetListenerWidget("ResizeCancel", ZGetCancelResizeConfirmListener());
	SetListenerWidget("ResizeRequest", ZGetRequestResizeListener());
	SetListenerWidget("ResizeReject", ZGetViewConfirmCancelListener());
	SetListenerWidget("ResizeAccept", ZGetViewConfrimAcceptListener());
	SetListenerWidget("16BitSound",ZGet16BitSoundListener());
	SetListenerWidget("8BitSound",ZGet8BitSoundListener());
	SetListenerWidget("TrackSlider", ZGetSongPositionListener());

	SetListenerWidget("NetworkPortChangeRestart", ZGetNetworkPortChangeRestartListener());
	SetListenerWidget("NetworkPortChangeCancel", ZGetNetworkPortChangeCancelListener());

	SetListenerWidget("MouseSensitivitySlider", ZGetMouseSensitivitySliderListener());
	SetListenerWidget("MouseSensitivityEdit", ZGetMouseSensitivityEditListener());

	// ��I����AIE����e
	SetListenerWidget("Exit", ZGetExitListener());
	SetListenerWidget("LoginOK", ZGetLoginListener());
	SetListenerWidget("OptionFrame", ZGetOptionFrameButtonListener());
	SetListenerWidget("Register", ZGetRegisterListener());
	SetListenerWidget("Stage_OptionFrame", ZGetOptionFrameButtonListener());
	SetListenerWidget("LobbyOptionFrame", ZGetOptionFrameButtonListener());
	SetListenerWidget("LoginOption", ZGetOptionFrameButtonListener());
	SetListenerWidget("WarningExit", ZGetExitListener());

	// ��I��n
	SetListenerWidget("Logout", ZGetLogoutListener());
	SetListenerWidget("ChannelChattingInput", ZGetChannelChatInputListener());
	SetListenerWidget("GameStart", ZGetGameStartListener());
	SetListenerWidget("ChatInput", ZGetChatInputListener());
	SetListenerWidget("ParentClose", ZGetParentCloseListener());
	SetListenerWidget("ChannelListFrameCaller", ZGetChannelListFrameCallerListener());
	SetListenerWidget("MapList", ZGetMapListListener());
	SetListenerWidget("Lobby_StageExit", ZGetLobbyListener());
	SetListenerWidget("LoginState", ZGetLoginStateButtonListener());
	SetListenerWidget("GreeterState", ZGetGreeterStateButtonListener());
	SetListenerWidget("CombatMenuClose", ZGetCombatMenuCloseButtonListener());
	SetListenerWidget("PreviousState", ZGetPreviousStateButtonListener());
	SetListenerWidget("QuickJoin", ZGetQuickJoinButtonListener());
	SetListenerWidget("QuickJoin2", ZGetQuickJoinButtonListener());
//	SetListenerWidget("Lobby_Charviewer_info", ZGetLobbyCharInfoCallerButtonListener());
	SetListenerWidget("StageBeforeBtn", ZGetLobbyPrevRoomListButtonListener());
	SetListenerWidget("StageAfterBtn", ZGetLobbyNextRoomListPrevButtonListener());
//	SetListenerWidget("Lobby_StageList", )

	SetListenerWidget("Lobby_RoomNo1", ZGetLobbyNextRoomNoButtonListener());
	SetListenerWidget("Lobby_RoomNo2", ZGetLobbyNextRoomNoButtonListener());
	SetListenerWidget("Lobby_RoomNo3", ZGetLobbyNextRoomNoButtonListener());
	SetListenerWidget("Lobby_RoomNo4", ZGetLobbyNextRoomNoButtonListener());
	SetListenerWidget("Lobby_RoomNo5", ZGetLobbyNextRoomNoButtonListener());
	SetListenerWidget("Lobby_RoomNo6", ZGetLobbyNextRoomNoButtonListener());

	/*
	SetListenerWidget("ChannelPlayerList_all", ZGetChannelPlayerOptionGroupAll());
	SetListenerWidget("ChannelPlayerList_friend", ZGetChannelPlayerOptionGroupFriend());
	SetListenerWidget("ChannelPlayerList_clan", ZGetChannelPlayerOptionGroupClan());

	SetListenerWidget("LobbyPlayerListTabChannel", ZGetListenerLobbyPlayerListTabChannel());
	SetListenerWidget("LobbyPlayerListTabFriend", ZGetListenerLobbyPlayerListTabFriend());
	SetListenerWidget("LobbyPlayerListTabClan", ZGetListenerLobbyPlayerListTabClan());
	*/
	
	SetListenerWidget("LobbyChannelPlayerListPrev", ZGetPlayerListPrevListener());
	SetListenerWidget("LobbyChannelPlayerListNext", ZGetPlayerListNextListener());

//	SetListenerWidget("StagePlayerListTabGame", ZGetListenerGamePlayerListTabGame());
//	SetListenerWidget("StagePlayerListTabFriend", ZGetListenerGamePlayerListTabFriend());

	SetListenerWidget("StagePlayerListPrev", ZGetStagePlayerListPrevListener());
	SetListenerWidget("StagePlayerListNext", ZGetStagePlayerListNextListener());

	SetListenerWidget("ArrangedTeamGame", ZGetArrangedTeamGameListener());
	SetListenerWidget("ArrangedTeamDialogOk", ZGetArrangedTeamDialogOkListener());
	SetListenerWidget("ArrangedTeamDialogClose", ZGetArrangedTeamDialogCloseListener());
	SetListenerWidget("ArrangedTeamGame_Cancel", ZGetArrangedTeamGame_CancelListener());

	SetListenerWidget("LeaveClanOK",		ZGetLeaveClanOKListener());
	SetListenerWidget("LeaveClanCancel",	ZGetLeaveClanCancelListener());

	SetListenerWidget("DuelTournamentGame",				ZGetDuelTournamentGameButtonListener());
	SetListenerWidget("DuelTournamentGame_2Test",		ZGetDuelTournamentGame2TestButtonListener());
	SetListenerWidget("DuelTournamentGame_4Test",		ZGetDuelTournamentGame4TestButtonListener());
	SetListenerWidget("DuelTournamentWaitMatchCancel",	ZGetDuelTournamentWaitCancelButtonListener());

	// channel
	SetListenerWidget("ChannelJoin", ZGetChannelListJoinButtonListener());
	SetListenerWidget("ChannelListClose", ZGetChannelListCloseButtonListener());
	SetListenerWidget("ChannelList", ZGetChannelListListener());
	SetListenerWidget("PrivateChannelEnter", ZGetPrivateChannelEnterListener());
	
	SetListenerWidget("ChannelList_Normal", ZGetChannelList());
	SetListenerWidget("ChannelList_Private", ZGetChannelList());
	SetListenerWidget("ChannelList_Clan", ZGetChannelList());
	SetListenerWidget("ChannelList_DuelTournament", ZGetChannelList());
	SetListenerWidget("MyClanChannel", ZGetMyClanChannel());
	
	// stage 
	SetListenerWidget("StageCreateFrameCaller", ZGetStageCreateFrameCallerListener());

	SetListenerWidget("StageListFrameCaller", ZGetStageListFrameCallerListener());
	SetListenerWidget("StageCreateBtn", ZGetStageCreateBtnListener());
	SetListenerWidget("PrivateStageJoinBtn", ZGetPrivateStageJoinBtnListener());
	SetListenerWidget("StageJoin", ZGetStageJoinListener());
	SetListenerWidget("StageChattingInput", ZGetStageChatInputListener());
	SetListenerWidget("StageSettingCaller", ZGetStageSettingCallerListener());
	SetListenerWidget("StageType", ZGetStageSettingStageTypeListener());
	SetListenerWidget("StageMaxPlayer", ZGetStageSettingChangedComboboxListener());			// �좯E?AI�Ƣ� A���Ƣ� : AO��eAI����
	SetListenerWidget("StageRoundCount", ZGetStageSettingChangedComboboxListener());		// �좯E?AI�Ƣ� A���Ƣ� : �Ʃ���aE����o
	SetListenerWidget("StageLimitTime", ZGetStageSettingChangedComboboxListener());			// �좯E?AI�Ƣ� A���Ƣ� : A|CN��A�ơ�
	SetListenerWidget("StageIntrude", ZGetStageSettingChangedComboboxListener());			// �좯E?AI�Ƣ� A���Ƣ� : ����AOCa��e
	SetListenerWidget("StageTeamRed", ZGetStageTeamRedListener());
	SetListenerWidget("StageTeamRed2", ZGetStageTeamRedListener());
	SetListenerWidget("StageTeamBlue", ZGetStageTeamBlueListener());
	SetListenerWidget("StageTeamBlue2", ZGetStageTeamBlueListener());
	SetListenerWidget("StageObserverBtn", ZGetStageObserverBtnListener());					// �좯E?AI�Ƣ� A���Ƣ� : ��uAu A��A��
	SetListenerWidget("StageReady", ZGetStageReadyListener());
	SetListenerWidget("StageSettingApplyBtn", ZGetStageSettingApplyBtnListener());
	SetListenerWidget("BattleExit", ZGetBattleExitButtonListener());
	SetListenerWidget("StageExit", ZGetStageExitButtonListener());

	SetListenerWidget("MapChange", ZGetMapChangeListener());
	SetListenerWidget("MapSelect", ZGetMapSelectListener());

	SetListenerWidget("MapList", ZGetStageMapListSelectionListener());
//	SetListenerWidget("MapList_Caller", ZGetStageMapListCallerListener());

	SetListenerWidget("Stage_SacrificeItemButton0", ZStageSacrificeItem0());
	SetListenerWidget("Stage_SacrificeItemButton1", ZStageSacrificeItem1());
	SetListenerWidget("Stage_PutSacrificeItem", ZStagePutSacrificeItem());
	SetListenerWidget("Stage_SacrificeItemBoxOpen", ZStageSacrificeItemBoxOpen());
	SetListenerWidget("Stage_SacrificeItemBoxClose", ZStageSacrificeItemBoxClose());
	SetListenerWidget("Stage_SacrificeItemListbox", ZGetSacrificeItemListBoxListener());
	SetListenerWidget("MonsterBookCaller", ZGetMonsterBookCaller());

	// ��E ����AA
	SetListenerWidget("MapSelection",  ZGetMapComboListener());

	// ��������AI��E
	SetListenerWidget("Stage_RelayMapType",			ZGetRelayMapTypeListener());
	SetListenerWidget("Stage_RelayMapRepeatCount",	ZGetRelayMapTurnCountListener());
	SetListenerWidget("Stage_RelayMapListbox",		ZGetRelayMapListBoxListener());
	SetListenerWidget("Stage_MapListbox",			ZGetMapListBoxListener());
	SetListenerWidget("Stage_RelayMap_OK_Button",	ZGetRelayMapOKButtonListener());
	SetListenerWidget("Stage_RelayMapBoxOpen",		ZStageRelayMapBoxOpen());
	SetListenerWidget("Stage_RelayMapBoxClose",		ZStageRelayMapBoxClose());

	//Custom: spymode
	SetListenerWidget("Stage_SpyBanMapBoxOpen", ZGetSpyBanMapBoxOpen());
	SetListenerWidget("Stage_SpyBanMapBoxClose", ZGetSpyBanMapBoxClose());

	// ��E��C
	SetListenerWidget("SaveOptionButton", ZGetSaveOptionButtonListener());
	SetListenerWidget("CancelOptionButton", ZGetCancelOptionButtonListener());

	/*
	SetListenerWidget("ShowVideoOptionGroup", ZGetShowVideoOptionGroupButtonListener());
	SetListenerWidget("ShowAudioOptionGroup", ZGetShowAudioOptionGroupButtonListener());
	SetListenerWidget("ShowMouseOptionGroup", ZGetShowMouseOptionGroupButtonListener());
	SetListenerWidget("ShowKeyboardOptionGroup", ZGetShowKeyboardOptionGroupButtonListener());
	SetListenerWidget("ShowEtcOptionGroup", ZGetShowEtcOptionGroupButtonListener());
	*/
	SetListenerWidget("DefaultSettingLoad", ZGetLoadDefaultKeySettingListener() );
	SetListenerWidget("Optimization",ZSetOptimizationListener());

	//======================================================================================================
	// »óÁ¡
	SetListenerWidget("LobbyShopCaller", ZGetShopCallerButtonListener());
	SetListenerWidget("StageShopCaller", ZGetShopCallerButtonListener());
	SetListenerWidget("ShopClose", ZGetShopCloseButtonListener());

	SetListenerWidget("LobbyEquipmentCaller", ZGetEquipmentCallerButtonListener());
	SetListenerWidget("StageEquipmentCaller", ZGetEquipmentCallerButtonListener());
	SetListenerWidget("EquipmentClose", ZGetEquipmentCloseButtonListener());

	SetListenerWidget("CharSelectionCaller", ZGetCharSelectionCallerButtonListener());

	SetListenerWidget("BuyConfirmCaller", ZGetBuyButtonListener());
	SetListenerWidget("BuyCashConfirmCaller", ZGetBuyCashButtonListener());

	SetListenerWidget("SellConfirmCaller", ZGetSellButtonListener());
	SetListenerWidget("TradeCountableItem_Cancel", ZGetCountableItemTradeDlgCloseListener());
	SetListenerWidget("TradeCountableItem_Ok", ZGetCountableItemTradeDlgOkButtonListener());
	SetListenerWidget("TradeCountableItem_CountUp", ZGetCountableItemTradeDlgCountUpButtonListener());
	SetListenerWidget("TradeCountableItem_CountDn", ZGetCountableItemTradeDlgCountDnButtonListener());	
	SetListenerWidget("TradeCountableItem_CountNum", ZGetCountableItemTradeDlgCountChangeListener());
	SetListenerWidget("SellCashItemConfirmFrame_Sell", ZGetSellCashItemConfirmDlgOkButtonListener());
	SetListenerWidget("SellCashItemConfirmFrame_Cancel", ZGetSellCashItemConfirmDlgCancelListener());
	//======================================================================================================

	SetListenerWidget("Equip", ZGetEquipButtonListener());
	SetListenerWidget("EquipmentSearch", ZGetEquipmentSearchButtonListener());
	SetListenerWidget("ForcedEntryToGame", ZGetStageForcedEntryToGameListener());
	SetListenerWidget("ForcedEntryToGame2", ZGetStageForcedEntryToGameListener());
	SetListenerWidget("AllEquipmentListCaller", ZGetAllEquipmentListCallerButtonListener());
	SetListenerWidget("MyAllEquipmentListCaller", ZGetMyAllEquipmentListCallerButtonListener());
	SetListenerWidget("CashEquipmentListCaller", ZGetCashEquipmentListCallerButtonListener());
	//	SetListenerWidget("Shop_Ask", ZGetShopAskButtonListener());
	//	SetListenerWidget("Shop_Gift", ZGetShopGiftButtonListener());
	SetListenerWidget("CashRecharge", ZGetShopCachRechargeButtonListener());
	SetListenerWidget("ShopSearchFrameCaller", ZGetShopSearchCallerButtonListener());

	SetListenerWidget("AllEquipmentList", ZGetShopPurchaseItemListBoxListener());
	SetListenerWidget("MyAllEquipmentList", ZGetShopSellItemListBoxListener());
	SetListenerWidget("CashEquipmentList", ZGetCashShopItemListBoxListener());

	SetListenerWidget("Shop_AllEquipmentFilter", ZGetShopListFilterListener());
	SetListenerWidget("Equip_AllEquipmentFilter", ZGetEquipListFilterListener());
	SetListenerWidget("Shop_to_Equipment", ZGetShopEquipmentCallerButtonListener() );

	SetListenerWidget("Shop_ArmorEquipmentTab",  ZShopItemEquipmentTabOpen());
	SetListenerWidget("Shop_WeaponEquipmentTab",  ZShopWeaponEquipmentTabOpen());	
	SetListenerWidget("Shop_EquipListFrameCloseButton", ZShopListFrameClose());
	SetListenerWidget("Shop_EquipListFrameOpenButton",  ZShopListFrameOpen());

	// Aa��n
	SetListenerWidget("Equipment_CharacterTab", ZGetEquipmentCharacterTabButtonListener());
	SetListenerWidget("Equipment_AccountTab", ZGetEquipmentAccountTabButtonListener());
	SetListenerWidget("EquipmentList", ZGetEquipmentMyItemListBoxListener());
	SetListenerWidget("AccountItemList", ZGetAccountItemListBoxListener());
	SetListenerWidget("Equipment_to_Shop", ZGetEquipmentShopCallerButtonListener());		
	SetListenerWidget("Equip_ArmorEquipmentTab",  ZEquipItemEquipmentTabOpen());
	SetListenerWidget("Equip_WeaponEquipmentTab",  ZEquipWeaponEquipmentTabOpen());	
	SetListenerWidget("Equip_EquipListFrameOpenButton",  ZEquipListFrameOpen());
	SetListenerWidget("Equip_EquipListFrameCloseButton", ZEquipListFrameClose());
	SetListenerWidget("Equipment_CharacterRotate", ZEquipmetRotateBtn());

	SetListenerWidget("BringAccountItemBtn", ZGetBringAccountItemButtonListener());
	SetListenerWidget("SendAccountItemBtn", ZGetSendAccountItemButtonListener());

	// A����?AI ����AA ��I����
	SetListenerWidget("CS_SelectChar", ZGetSelectCharacterButtonListener());
	SetListenerWidget("CS_SelectCharDefKey", ZGetSelectCharacterButtonListener());
	SetListenerWidget("CS_CreateChar", ZGetShowCreateCharacterButtonListener());
	SetListenerWidget("CS_DeleteChar", ZGetDeleteCharacterButtonListener());
	SetListenerWidget("CS_Prev", ZGetLogoutListener());
	SetListenerWidget("CharSel_SelectBtn0", ZGetSelectCharacterButtonListener0());
	SetListenerWidget("CharSel_SelectBtn1", ZGetSelectCharacterButtonListener1());
	SetListenerWidget("CharSel_SelectBtn2", ZGetSelectCharacterButtonListener2());
	SetListenerWidget("CharSel_SelectBtn3", ZGetSelectCharacterButtonListener3());
	SetListenerWidget("ShowChar_infoGroup", ZGetShowCharInfoGroupListener());
	SetListenerWidget("ShowEquip_InfoGroup", ZGetShowEquipInfoGroupListener());

	//	SetListenerWidget("CS_CharList", ZGetSelectCharacterListBoxDBClickListener);
	// A����?AI ��y����
	SetListenerWidget("CC_CreateChar", ZGetCreateCharacterButtonListener());
	SetListenerWidget("CC_Cancel", ZGetCancelCreateCharacterButtonListener());
	SetListenerWidget("CC_Sex", ZChangeCreateCharInfoListener());
	SetListenerWidget("CC_Hair", ZChangeCreateCharInfoListener());
	SetListenerWidget("CC_Face", ZChangeCreateCharInfoListener());
	SetListenerWidget("CC_Face", ZChangeCreateCharInfoListener());
	SetListenerWidget("CC_Costume", ZChangeCreateCharInfoListener());
	SetListenerWidget("Charviewer_Rotate_L", ZGetCreateCharacterLeftButtonListener());
	SetListenerWidget("Charviewer_Rotate_R", ZGetCreateCharacterRightButtonListener());

	//SetListenerWidget("Lobby_Charviewer_Rotate_L", ZGetSelectCameraLeftButtonListener());
	//SetListenerWidget("Lobby_Charviewer_Rotate_R", ZGetSelectCameraRightButtonListener());

	SetListenerWidget("CS_DeleteCharButton", ZGetConfirmDeleteCharacterButtonListener());
	SetListenerWidget("CS_CloseConfirmDeleteCharButton", ZGetCloseConfirmDeleteCharButtonListener());

	// ��o����AI ���ơ�
	SetListenerWidget("MonsterBook_PrevPageButton", ZGetMonsterInterfacePrevPage());
	SetListenerWidget("MonsterBook_NextPageButton", ZGetMonsterInterfaceNextPage());
	SetListenerWidget("MonsterBook_Close", ZGetMonsterInterfaceQuit());
	SetListenerWidget("MonsterBook_Close2", ZGetMonsterInterfaceQuit());


	// ��OAO ��a��u E����e
	SetListenerWidget("GameResult_ButtonQuit", ZGetGameResultQuit());


	//112
	SetListenerWidget("112_ConfirmEdit", ZGet112ConfirmEditListener());
	SetListenerWidget("112_ConfirmOKButton", ZGet112ConfirmOKButtonListener());
	SetListenerWidget("112_ConfirmCancelButton", ZGet112ConfirmCancelButtonListener());


	// A������
	SetListenerWidget("ClanSponsorAgreementConfirm_OK", ZGetClanSponsorAgreementConfirm_OKButtonListener());
	SetListenerWidget("ClanSponsorAgreementConfirm_Cancel", ZGetClanSponsorAgreementConfirm_CancelButtonListener());
	SetListenerWidget("ClanSponsorAgreementWait_Cancel", ZGetClanSponsorAgreementWait_CancelButtonListener());
	SetListenerWidget("ClanJoinerAgreementConfirm_OK", ZGetClanJoinerAgreementConfirm_OKButtonListener());
	SetListenerWidget("ClanJoinerAgreementConfirm_Cancel", ZGetClanJoinerAgreementConfirm_CancelButtonListener());
	SetListenerWidget("ClanJoinerAgreementWait_Cancel", ZGetClanJoinerAgreementWait_CancelButtonListener());
	
	// A������ ��y���� ��UAI��o��I����
	SetListenerWidget("LobbyPlayerListTabClanCreateButton", ZGetLobbyPlayerListTabClanCreateButtonListener());
	SetListenerWidget("ClanCreateDialogOk", ZGetClanCreateDialogOk());
	SetListenerWidget("ClanCreateDialogClose", ZGetClanCreateDialogClose());


	// �좯AC ��u��A
	SetListenerWidget("ProposalAgreementWait_Cancel", ZGetProposalAgreementWait_CancelButtonListener());
	SetListenerWidget("ProposalAgreementConfirm_OK", ZGetProposalAgreementConfirm_OKButtonListener());
	SetListenerWidget("ProposalAgreementConfirm_Cancel", ZGetProposalAgreementConfirm_CancelButtonListener());

	// ����CA����AI ��e��o��iA�� ��u��A
	SetListenerWidget("ReplayOkButton",				ZReplayOk());
	SetListenerWidget("ReplayCaller",				ZGetReplayCallerButtonListener());
	SetListenerWidget("Replay_View",				ZGetReplayViewButtonListener());
	SetListenerWidget("ReplayClose",				ZGetReplayExitButtonListener());
	SetListenerWidget("Replay_FileList",			ZGetReplayFileListBoxListener());

	////Custom:
	//SetListenerWidget("Gladiator_Blue", ZGetBlitzBlueGladiatorListener());
	//SetListenerWidget("Gladiator_Red", ZGetBlitzRedGladiatorListener());
	//SetListenerWidget("Hunter_Blue", ZGetBlitzBlueGladiatorListener());
	//SetListenerWidget("Hunter_Red", ZGetBlitzRedGladiatorListener());
	//SetListenerWidget("Terrorist_Blue", ZGetBlitzBlueTerroristListener());
	//SetListenerWidget("Terrorist_Red", ZGetBlitzRedTerroristListener());

	////Achievements
	//SetListenerWidget("AchievementCaller", ZGetAchievementCallerButtonListener());
	//SetListenerWidget("AchievementClose", ZGetAchievementExitButtonListener());

	//Music
	SetListenerWidget("CustomMusicList", ZGetCustomMusicListListener());
	SetListenerWidget("EnableCustomMusic",ZGetEnableCustomMusicListener());
	//End Custom

	////Custom: create account
	SetListenerWidget("CreateAccountFrameCaller", ZGetCreateAccountFrameCallerListener());
	SetListenerWidget("CreateAccountBtn", ZGetCreateAccountBtnListener());
	////End 


	// ����AA��E A����?AI A�좬����o��I ������碬c ������a

	// ���������E �������AI�����E ACAU���硤N �ơ��AC��AO��a.
	MTabCtrl *pTab = (MTabCtrl*)m_IDLResource.FindWidget("PlayerListControl");
	if( pTab ) pTab->UpdateListeners();

	return true;
}

void ZGameInterface::FinalInterface()
{
	// Player Menu
	SAFE_DELETE(m_pPlayerMenu);

	m_IDLResource.Clear();

	mlog("clear IDL resource end.\n");

	SetCursor(NULL);

	mlog("Final interface end.\n");
}

bool ZGameInterface::ChangeInterfaceSkin(const char* szNewSkinName)
{
	char szPath[256];
	char szFileName[256];
	ZGetInterfaceSkinPath(szPath, szNewSkinName);
	sprintf(szFileName, "%s%s", szPath, FILENAME_INTERFACE_MAIN);

	FinalInterface();
	bool bSuccess=InitInterface(szNewSkinName);

	if(bSuccess)
	{
		switch(m_nState)
		{
		case GUNZ_LOGIN:	OnLoginCreate(); break;//("Login", true); break;
		case GUNZ_LOBBY:	OnLobbyCreate(); break;//ShowWidget("Lobby", true); 	break;
		case GUNZ_STAGE: 	OnStageCreate(); break;//;("Stage", true); 	break;
		case GUNZ_CHARSELECTION:
			if (m_bShowInterface)
			{
				OnCharSelectionCreate();//ShowWidget("CharSelection", true);
			}break;
		case GUNZ_CHARCREATION: OnCharCreationCreate(); break;//ShowWidget("CharCreation", true); break;
		}
		ZGetOptionInterface()->Resize(MGetWorkspaceWidth(),MGetWorkspaceHeight());
	}

	return bSuccess;
}


// ��AA������E?��u ���졾aAa��nAC AO��A ���AIAI ��e....

static bool g_parts[10];	// A����?AI Aa��n ��IA�� A���������碯e AO��A ��?��o
static bool g_parts_change;

//static int	g_select_weapon=0;
//static bool g_weapon[10];
//static bool g_weapon_change;



/// OnCommand��| ZGameInterface_OnCommand.cpp��I ��E��e. 

bool ZGameInterface::ShowWidget(const char* szName, bool bVisible, bool bModal)
{
	MWidget* pWidget = m_IDLResource.FindWidget(szName);

	if ( pWidget == NULL)
		return false;

	if ( strcmp( szName, "Lobby") == 0)
	{
		pWidget->Show(bVisible, bModal);

		pWidget = m_IDLResource.FindWidget( "Shop");
		pWidget->Show( false);
		pWidget = m_IDLResource.FindWidget( "Equipment");
		pWidget->Show( false);
	}
	else
		pWidget->Show(bVisible, bModal);

	return true;
}

void ZGameInterface::SetListenerWidget(const char* szName, MListener* pListener)
{
	BEGIN_WIDGETLIST(szName, &m_IDLResource, MWidget*, pWidget);
	pWidget->SetListener(pListener);
	END_WIDGETLIST();
}

void ZGameInterface::EnableWidget(const char* szName, bool bEnable)
{
	MWidget* pWidget = m_IDLResource.FindWidget(szName);
	if (pWidget) pWidget->Enable(bEnable);
}

void ZGameInterface::SetTextWidget(const char* szName, const char* szText)
{
	BEGIN_WIDGETLIST(szName, &m_IDLResource, MWidget*, pWidget);
	pWidget->SetText(szText);
	END_WIDGETLIST();
}

bool ZGameInterface::OnGameCreate(void)
{
	// ��e���� A��AO��A ��U���� A��A��A�� ������I false A������
	HideAllWidgets();

	// ��������AI AI��IAo ������I��aC������ ������������ E�稬��
	GetItemThumbnailMgr()->UnloadTextureTemporarily();
	///TODO: uncomment code
//	// WPE hacking protect
//#ifndef _DEBUG
	HMODULE hMod = GetModuleHandle( "ws2_32.dll"); 
	FARPROC RetVal = GetProcAddress( hMod, "recv"); 
	if ( (BYTE)RetVal == (BYTE)0xE9)
	{
//		mlog( "Hacking detected");

//		MessageBox(NULL, ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		//ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);

		ZPostDisconnect();
	}
//#endif

	m_Camera.Init();
	ClearMapThumbnail();

	g_parts[6] = true;//A��A�� Aa��i ��AAU AO��A..

	//DrawLoadingScreen("Now Loading...", 0.0f);
	if (CustomMusic::m_customMusic.size() <= 0)
		ZApplication::GetSoundEngine()->CloseMusic();

	m_bLoading = true;
	ZLoadingProgress gameLoading("Game");

#ifndef _MAP_CACHING
	//m_pLoadingInterface = new ZLoading("loading", this, this);

//	ZLoadingProgress gameLoading("Game");

	ZGetInitialLoading()->Initialize( 1, 0,0, RGetScreenWidth(), RGetScreenHeight(), 0,0, 1024, 768, true );
#else
	if (!ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()))
	{
		//m_pLoadingInterface = new ZLoading("loading", this, this);


		ZGetInitialLoading()->Initialize( 1, 0,0, RGetScreenWidth(), RGetScreenHeight(), 0,0, 1024, 768, true );
	}
#endif
	// ��I��u AI��IAo ��I��a
	char szFileName[256];
	int nBitmap = rand() % 9;

#if defined(LOCALE_NHNUSA) || defined(LOCALE_BRAZIL) || defined(LOCALE_JAPAN)
	switch ( rand() % 3)
	{
	case ( 0) :
		strcpy( szFileName, "Interface/default/loading/loading_1.jpg");
		break;
	case ( 1) :
		strcpy( szFileName, "Interface/default/loading/loading_2.jpg");
		break;
	case ( 2) :
		strcpy( szFileName, "Interface/default/loading/loading_3.jpg");
		break;
	}
#endif

#ifndef _MAP_CACHING
	//ZGetInitialLoading()->AddBitmap( 0, szFileName);
	//ZGetInitialLoading()->AddBitmapBar( "Interface/default/loading/loading.bmp" );
#else
	if (!ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()))
	{
		ZGetInitialLoading()->AddBitmap(0, szFileName);
		ZGetInitialLoading()->AddBitmapBar("Interface/default/loading/loading.bmp");
	}
#endif


#ifndef _FASTDEBUG
#ifndef _MAP_CACHING
	ZGetInitialLoading()->SetPercentage( 0.0f );
	ZGetInitialLoading()->Draw( MODE_FADEIN, 0 , true  );
#else
	if (!ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()))
	{
		ZGetInitialLoading()->SetPercentage(0.0f);
		ZGetInitialLoading()->Draw(MODE_FADEIN, 0, true);
	}
#endif
#else
	ZGetInitialLoading()->SetPercentage( 10.f );
	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0, true );
#endif

	//m_pLoadingInterface->OnCreate();

	//m_pLoadingInterface->Progress( LOADING_BEGIN );
	//Redraw();	// Loading ScreenA����I ��U��A �������Ƣ�U.

	m_pGame=new ZGame;
#ifndef _MAP_CACHING
	if(!m_pGame->Create(ZApplication::GetFileSystem(), &gameLoading ))
	{
		mlog("ZGame ��y���� ��C����\n");
		SAFE_DELETE(m_pGame);
		m_bLoading = false;
		//m_pLoadingInterface->OnDestroy();
		//delete m_pLoadingInterface; m_pLoadingInterface = NULL;
		// �����O������I��u ��C������iAC ��e���� ��AAU ��C����CO��A ������o���� ����A��AIAo �ҡƩ�����U��A ����A��A�� ���������i ��I��n��I ��������ơ̢�U.
		ZPostStageLeave(ZGetGameClient()->GetPlayerUID());
		ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);

		ZGetInitialLoading()->Release();
		
		return false;
	}
#else
	if (!m_pGame->Create(ZApplication::GetFileSystem(), ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()) ? nullptr : &gameLoading))
	{
		mlog("Failed to create game. Could not find map.\n");
		SAFE_DELETE(m_pGame);
		m_bLoading = false;
		ZPostStageLeave(ZGetGameClient()->GetPlayerUID());
		ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);

		if(!ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()))
			ZGetInitialLoading()->Release();

		return false;
	}
#endif

	/*
	if(m_spGameClient->IsForcedEntry())
		g_pGame->SetForcedEntry();
	*/

	m_pMyCharacter=(ZMyCharacter*)ZGetGame()->m_pMyCharacter;

	

	SetFocus();

	//ZGetInitialLoading()->SetPercentage( 100.f );

	m_pGameInput = new ZGameInput();

	m_pCombatInterface = new ZCombatInterface("combatinterface", this, this);
	m_pCombatInterface->SetBounds(GetRect());
	m_pCombatInterface->OnCreate();

	
	MWidget *pWidget = m_IDLResource.FindWidget("SkillFrame");
	if(pWidget!=NULL) pWidget->Show(true);

	// Skill List
	InitSkillList(m_IDLResource.FindWidget("SkillList"));

	pWidget = m_IDLResource.FindWidget("InventoryFrame");
	if(pWidget!=NULL) pWidget->Show(true);

	// Item List
	InitItemList(m_IDLResource.FindWidget("ItemList"));

	// A������ ��c��eCIAo ��EA��
	SetCursorEnable(false);

	m_bLoading = false;

#ifndef _FASTDEBUG
#ifndef _MAP_CACHING
	ZGetInitialLoading()->SetPercentage( 100.0f) ;
	ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 , true );
#else
	if (!ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()))
	{
		ZGetInitialLoading()->SetPercentage(100.0f);
		ZGetInitialLoading()->Draw(MODE_FADEOUT, 0, true);
	}
#endif
#endif
#ifndef _MAP_CACHING
	ZGetInitialLoading()->Release();
#else
	if (!ZGetMapCache()->Find(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()))
		ZGetInitialLoading()->Release();
#endif

	if( (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_REPLAY) ||
		(ZGetGameClient()->IsLadderGame()) || 
		ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) ||
		ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT ||
		ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG)
	{
		m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, false);
	}
	else
	{
		m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, true);
	}


#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportCreateGameScreen();
#endif

	//// �좯�����o A��AA...2008.10.02
	m_Capture = new ZBandiCapturer;
	m_Capture->Init(g_hWnd, (LPDIRECT3DDEVICE9)RGetDevice());

	return true;
}

void ZGameInterface::OnGameDestroy(void)
{
	mlog( "game interface destory begin.\n" );
	MPicture* pPicture;
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "ClanResult_ClanBitmap1");
	if(pPicture) pPicture->SetBitmap(NULL);
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "ClanResult_ClanBitmap2");
	if(pPicture) pPicture->SetBitmap(NULL);

	MWidget *pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "GameResult");
	if ( pWidget) {
		MFrame *pFrame = (MFrame*)pWidget;
		pFrame->MFrame::Show( false);
	}

	ZGetGameClient()->RequestOnGameDestroyed();

	SAFE_DELETE(m_pMiniMap);

	if (m_pGameInput)
	{
		delete m_pGameInput; m_pGameInput = NULL;
//		Mint::GetInstance()->SetGlobalEvent(NULL);
	}

	if (m_pCombatInterface)
	{
		m_pCombatInterface->OnDestroy();
		delete m_pCombatInterface;
		m_pCombatInterface = NULL;
	}

	ShowWidget(CENTERMESSAGE, false);

	if(ZGetGame()!=NULL){
		ZGetGame()->Destroy();
		SAFE_DELETE(m_pGame);
	}

	SetCursorEnable(true);
	m_bLeaveBattleReserved = false;
	m_bLeaveStageReserved = false;

	// �좯�����o A��AA...2008.10.02
	SAFE_DELETE(m_Capture);

	mlog("game interface destroy finished\n");
}

void ZGameInterface::OnGreeterCreate(void)
{
	ShowWidget("Greeter", true);

	if ( m_pBackground)
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());
}

void ZGameInterface::OnGreeterDestroy(void)
{
	ShowWidget("Greeter", false);

#ifdef _CHURCH
	if (m_pBackground)
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
#else
	if (m_pBackground)
		m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
#endif
}

void ZGameInterface::OnLoginCreate(void)
{
	///TODO: uncomment these
	// WPE hacking protect
//#ifndef _DEBUG
	HMODULE hMod = GetModuleHandle( "ws2_32.dll"); 
	FARPROC RetVal = GetProcAddress( hMod, "recv"); 
	if ( (BYTE)RetVal == 0xE9)
	{
	//	mlog( "Hacking detected");

//		MessageBox(NULL, ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		//ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);

		ZPostDisconnect();
	}
//#endif

	m_bLoginTimeout = false;
	m_nLoginState = LOGINSTATE_FADEIN;
	m_dwLoginTimer = timeGetTime();

	// ��e�Ʃ� AI��IAo ��I��u
	if ( m_pLoginBG != NULL)
	{
		delete m_pLoginBG;
		m_pLoginBG = NULL;
	}
	m_pLoginBG = nullptr;
	bool bRead = false;
	
	// ��U��I ��AAIA�� A����A��U.
#ifdef _DEBUG
	//bRead = m_pLoginBG->Create( "loginbg.png", RGetDevice(), "wallpaper.jpg", false);
#else
	//MZFile::SetReadMode( MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE );
	//bRead = m_pLoginBG->Create( "loginbg.png", RGetDevice(), "wallpaper.jpg", false);
	//MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
#endif

	if ( bRead)
	{
		if ( (m_pLoginBG->GetWidth() > 1024) || (m_pLoginBG->GetHeight() > 768))
			bRead = false;
	}
	/*
	// ����A����e �������� ��AAIA�� A����A��U.
	if ( !bRead)
		bRead = m_pLoginBG->Create( "loginbg.png", RGetDevice(), "Interface/loadable/loginbg.jpg");

	// A����i��A ��n���碬E AI��IAo ����AIAI��| C����c A��A������ ��N��UAa���� C�ͨ�ACN��U
	if ( bRead && m_pLoginBG)
	{
		MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget( "Login_BackgrdImg");
		if ( pPicture)
			pPicture->SetBitmap( m_pLoginBG->GetSourceBitmap());
	}*/

	MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget("Login_BackgrdImg");
	if (pPicture)
		pPicture->SetBitmap(NULL);

    // ������I AI��IAo ��I��u
	if ( m_pLoginPanel != NULL)
	{
		delete m_pLoginPanel;
		m_pLoginPanel = NULL;
	}

	m_pLoginPanel = new MBitmapR2;
	((MBitmapR2*)m_pLoginPanel)->Create( "loginpanel.png", RGetDevice(), "Interface/loadable/loginpanel.tga");
	if ( m_pLoginPanel)
	{
		// A����i��A ��n���碬E AI��IAo ����AIAI��| C����c A��A������ ��N��UAa���� C�ͨ�ACN��U
		MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget( "Login_Panel");
		if ( pPicture)
			pPicture->SetBitmap( m_pLoginPanel->GetSourceBitmap());
	}

	MButton* pLoginOk = (MButton*)m_IDLResource.FindWidget( "LoginOK");
	if (pLoginOk)
		pLoginOk->Enable(true);

	MWidget* pLoginFrame	= m_IDLResource.FindWidget( "LoginFrame");
	MWidget* pLoginBG		= m_IDLResource.FindWidget( "Login_BackgrdImg");
	if (pLoginFrame)
	{
		if ( pLoginBG)
			pLoginFrame->Show(false);
		else
			pLoginFrame->Show(true);
	}

	pLoginFrame = m_IDLResource.FindWidget( "Login_ConnectingMsg");
	if ( pLoginFrame)
		pLoginFrame->Show( false);

	MLabel* pErrorLabel = (MLabel*)m_IDLResource.FindWidget( "LoginError");
	if ( pErrorLabel)
		pErrorLabel->SetText( "");

	//MLabel* pPasswd = (MLabel*)m_IDLResource.FindWidget( "LoginPassword");
	//if ( pPasswd)
	//	pPasswd->SetText( "");

	// ��Y���Ҩ�i��� AIA| ������o����������碬| ��������AO��a �ҡש��碯�� AO����A������
	// Netmarble �������� ��I��aCN �Ʃ���i���� Standalone LoginA�� ��a����CI��e ��OAOA����a
	//if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE) {
	//	mlog("Netmarble Logout \n");
	//	ZApplication::Exit();
	//	return;
	//}
	
	HideAllWidgets();

	ShowWidget("Login", true);
	//ShowWidget("LoginFrame", true);
	//BEGIN_WIDGETLIST("MaxHP", &m_IDLResource, MWidget*, pWidget);

	ZServerView* pServerList = (ZServerView*)m_IDLResource.FindWidget( "SelectedServer");
	if ( pServerList)
	{
		pServerList->ClearServerList();
		if ( strcmp( Z_LOCALE_DEFAULT_FONT, "Arial") == 0)
			pServerList->SetTextOffset( -2);

		// A����������
//		pServerList->AddServer( "Billcap", "192.168.0.132", 6000, 2, 0, 1000, true);


//#if defined(_DEBUG)	|| defined(_RELEASE)
//		ShowWidget("LabelServerIP", true);
//		ShowWidget("ServerAddress", true);
//		ShowWidget("ServerPort", true);
//
//		pServerList->AddServer( "Debug Server", "", 0, 7, 0, 1000, true);			// Debug server
//		pServerList->SetCurrSel( 0);
//		MWidget* pWidget = m_IDLResource.FindWidget("LoginID");
//		if(pWidget)	pWidget->SetFocus();
//#else
		if ( ZIsLaunchDevelop())
		{
			ShowWidget("LabelServerIP", true);
			ShowWidget("ServerAddress", true);
			ShowWidget("ServerPort", true);

			pServerList->AddServer( "Debug Server", "", 0, 7, 0, 1000, true);			// Debug server
			pServerList->SetCurrSel( -1);
		}
//#endif


	
		if ( ZApplication::GetInstance()->IsLaunchTest())
		{
			if( m_pTLocatorList && (m_pTLocatorList->GetSize() > 0))
				m_nLocServ = rand() % m_pTLocatorList->GetSize();			// A��A�� ��AAUCI��A A���������� ������o��| ���̢�yA����I ����AACN��U.
		}
		else
		{
			if( m_pLocatorList && (m_pLocatorList->GetSize() > 0))
				m_nLocServ = rand() % m_pLocatorList->GetSize();			// A��A�� ��AAUCI��A ������o��| ���̢�yA����I ����AACN��U.
		}

		RequestServerStatusListInfo();
	}


	MWidget* pWidget = m_IDLResource.FindWidget("LoginID");
	if(pWidget)
	{
		char buffer[256];
		if (ZGetApplication()->GetSystemValue("LoginID", buffer))
		{
			if(strlen(buffer) > 0)
				pWidget->SetText(buffer);
			else
			{
				strcpy(buffer,"Username");
				pWidget->SetText(buffer);
			}
			mlog("\nLoginID: %s\n",buffer);
		}
	}

	MEdit* pEdit = (MEdit*)m_IDLResource.FindWidget("LoginPassword");
	if (pEdit)
	{
		char buffer[64];
		if (ZGetApplication()->GetSystemValue("LoginPass", buffer))
		{
			if (strlen(buffer) > 0)
			{
				pEdit->SetText(buffer);
				MButton* pButton = (MButton*)m_IDLResource.FindWidget("LoginRememberPass");

				if (pButton)
					pButton->SetCheck(true);
			}
		}
	}

	// ������o IP
	pWidget = m_IDLResource.FindWidget("ServerAddress");
	if(pWidget)
	{
		pWidget->SetText(ZGetConfiguration()->GetServerIP());

	}
	pWidget = m_IDLResource.FindWidget("ServerPort");
	if(pWidget)
	{
		char szText[ 25];
		sprintf( szText, "%d", ZGetConfiguration()->GetServerPort());
		pWidget->SetText( szText);
	}

	if ( m_pBackground)
	{
		m_pBackground->LoadMesh();
		// Custom: Changed LOGIN_SCENE_FIXEDSKY (Default value) to LOGIN_SCENE_FALLDOWN
#ifdef _CHURCH
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
#else
		m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
#endif
	}

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

	if (m_spGameClient->IsConnected())
	{
		ZPostDisconnect();
	}

// ��OA����I ������c ��U��i�����A��A ������o����������� E����eA�� ��������A����U��A �Ƣ�A��A����I AO����A������
//#ifdef LOCALE_NHNUSA
//	m_IDLResource.FindWidget( "LoginID")->SetText( "GUNZ");
//	m_IDLResource.FindWidget( "LoginPassword")->SetText( "1234567890");
//#elif LOCALE_JAPAN


		ShowWidget("Login_Logo", true);
		ShowWidget("Label_ID", true);
		ShowWidget("LoginID", true);
		ShowWidget("Label_Password", true);
		ShowWidget("LoginPassword", true);
		ShowWidget("LabelServerIP", false);
		ShowWidget("ServerAddress", false);
		ShowWidget("ServerPort", false);
		
		// ��I����AI ��o����A�� E�ƨ���E�� CI��aA��CN �ҡ��ע�e ID, PassWord
	//	m_IDLResource.FindWidget( "LoginID")->SetText( "Username");
		/*m_IDLResource.FindWidget( "LoginPassword")->SetText( "");*/
		ShowWidget("MonsterBookCaller",false);

//#endif

#ifndef _PUBLISH
/*	// AU�좯A�˨�OCI��OCI��AA����������
	m_IDLResource.FindWidget("LoginID")->SetText("n4");
	m_IDLResource.FindWidget("LoginPassword")->SetText("4");
	//m_IDLResource.FindWidget("ServerAddress")->SetText("192.168.0.79");
	ZPostConnect("192.168.0.60", 6000);	//*/
#endif
}
void ZGameInterface::OnLoginDestroy(void)
{
	ShowWidget("Login", false);

	MWidget* pWidget = m_IDLResource.FindWidget("LoginID");
	if(pWidget)
	{
		// ��I��aAI �����Ʃ�CI��e write C������ CI����.. Ao��Y check out ��u��e��I ���ϡ�a��U -_-;
		ZGetApplication()->SetSystemValue("LoginID", pWidget->GetText());

#ifdef _CHURCH
		if (m_pBackground)
			m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
#endif
	}

	// ��e�Ʃ� AI��IAo��| �����������硤I��IAI ��eA|CN��U
	if ( m_pLoginBG != NULL)
	{
		// ��e�Ʃ� AI��IAo��| ��������AO��A A��A��AC ��n���碬E AI��IAo ����AIAI��| �����ACN��U
		MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget( "Login_BackgrdImg");
		if ( pPicture)
			pPicture->SetBitmap( NULL);
	
		delete m_pLoginBG;
		m_pLoginBG = NULL;
	}

	// ������I AI��IAo��| �����������硤I��IAI ��eA|CN��U
	if ( m_pLoginPanel != NULL)
	{
		// ������I AI��IAo��| ��������AO��A A��A��AC ��n���碬E AI��IAo ����AIAI��| �����ACN��U
		MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget( "Login_Panel");
		if ( pPicture)
			pPicture->SetBitmap( NULL);
	
		delete m_pLoginPanel;
		m_pLoginPanel = NULL;
	}

	ZGetShop()->Destroy();
}

void ZGameInterface::OnDirectLoginCreate(void)
{
	OnNetmarbleLoginCreate();
}

void ZGameInterface::OnDirectLoginDestroy(void)
{
	OnNetmarbleLoginDestroy();
}

#include "ZNetmarble.h"
void ZGameInterface::OnNetmarbleLoginCreate(void)
{
	if ( m_pBackground)
	{
		m_pBackground->LoadMesh();
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
	}

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

	if (m_spGameClient->IsConnected())
	{
		ZPostDisconnect();
	}

	HideAllWidgets();
	ShowWidget("NetmarbleLogin", true);

	ZBaseAuthInfo* pAuthInfo = ZGetLocale()->GetAuthInfo();
	if (pAuthInfo)
	{

		ZPostConnect(pAuthInfo->GetServerIP(), pAuthInfo->GetServerPort());
	}
	else _ASSERT(0);
}

void ZGameInterface::OnNetmarbleLoginDestroy(void)
{
		if ( m_pBackground)
			m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
}


void ZGameInterface::OnGameOnLoginCreate(void)
{
	if ( m_pBackground)
	{
		m_pBackground->LoadMesh();
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
	}

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

	if (m_spGameClient->IsConnected())
	{
		ZPostDisconnect();
	}

	HideAllWidgets();
	ShowWidget("NetmarbleLogin", true);

	ZBaseAuthInfo* pAuthInfo = ZGetLocale()->GetAuthInfo();
	if (pAuthInfo)
		ZPostConnect( pAuthInfo->GetServerIP(), pAuthInfo->GetServerPort());
	else
		_ASSERT(0);
}

void ZGameInterface::OnGameOnLoginDestroy(void)
{
		if ( m_pBackground)
			m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
}


void ZGameInterface::OnLobbyCreate(void)
{
	/*if( m_pAmbSound != NULL )
	{
		m_pAmbSound->Stop();
		m_pAmbSound		= NULL;
	}
	//*/
	
	// ����CA����AI EA���� ��U��i LevelPercent�ƨ�A�� ����������e��I ��������CN��U
	if ( m_bOnEndOfReplay)
	{
		m_bOnEndOfReplay = false;
		ZGetMyInfo()->SetLevelPercent( m_nLevelPercentCache);
	}

	// ����CA����AI EA���� ��U��i LevelPercent�ƨ�A�� ����������e��I ��������CN��U
	if ( m_bOnEndOfReplay)
	{
		m_bOnEndOfReplay = false;
		ZGetMyInfo()->SetLevelPercent( m_nLevelPercentCache);
	}

	if( m_pBackground != 0 )
		m_pBackground->Free();	// Free Memory...

	if (m_spGameClient)
	{
		m_spGameClient->ClearPeers();
		m_spGameClient->ClearStageSetting();
	}

	SetRoomNoLight(1);
	ZGetGameClient()->RequestOnLobbyCreated();
	

	ShowWidget("CombatMenuFrame", false);
	ShowWidget("Lobby", true);
	EnableLobbyInterface(true);

	MWidget* pWidget = m_IDLResource.FindWidget("StageName");
	if(pWidget){
		char buffer[256];
		if (ZGetApplication()->GetSystemValue("StageName", buffer))
			pWidget->SetText(buffer);
	}

 	ZRoomListBox* pRoomList = (ZRoomListBox*)m_IDLResource.FindWidget("Lobby_StageList");
	if (pRoomList) pRoomList->Clear();

	ShowWidget("Lobby_StageList", true);
	/*
	ShowWidget("ChannelFrame", true);
	ShowWidget("StageListFrame", true);
	ShowWidget("StageFrame", true);
	*/

	MPicture* pPicture = 0;
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StripBottom");
 	if(pPicture != NULL)	pPicture->SetAnimation( 0, 1000.0f);
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StripTop");
	if(pPicture != NULL)	pPicture->SetAnimation( 1, 1000.0f);

    pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_RoomListBG");
	if ( pPicture)
	{
		m_pRoomListFrame = new MBitmapR2;
		((MBitmapR2*)m_pRoomListFrame)->Create( "gamelist_panel.png", RGetDevice(), "interface/loadable/gamelist_panel.png");

		if ( m_pRoomListFrame != NULL)
			pPicture->SetBitmap( m_pRoomListFrame->GetSourceBitmap());

		m_pDuelTournamentLobbyFrame = new MBitmapR2;
		((MBitmapR2*)m_pDuelTournamentLobbyFrame)->Create( "dueltournament_lobby_panel.png", RGetDevice(), "interface/loadable/dueltournament_lobby_panel.png");
	}
    pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_BottomBG");
	if ( pPicture)
	{
		m_pBottomFrame = new MBitmapR2;
		((MBitmapR2*)m_pBottomFrame)->Create( "bottom_panel.png", RGetDevice(), "interface/loadable/bottom_panel.png");

		if ( m_pBottomFrame != NULL)
			pPicture->SetBitmap( m_pBottomFrame->GetSourceBitmap());
	}

	m_pClanInfoBg = new MBitmapR2;
	((MBitmapR2*)m_pClanInfoBg)->Create( "claninfo_panel.tga", RGetDevice(), "interface/loadable/claninfo_panel.tga");
	if ( m_pClanInfoBg != NULL)
	{
		pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_ClanInfoBG");
		if ( pPicture)	pPicture->SetBitmap( m_pClanInfoBg->GetSourceBitmap());
	}

	m_pDuelTournamentInfoBg = new MBitmapR2;
	((MBitmapR2*)m_pDuelTournamentInfoBg)->Create( "dueltournamentinfo_panel.tga", RGetDevice(), "interface/loadable/dueltournamentinfo_panel.tga");
	if ( m_pDuelTournamentInfoBg != NULL)
	{
		pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentInfoBG");
		if ( pPicture)	pPicture->SetBitmap( m_pDuelTournamentInfoBg->GetSourceBitmap());
	}

	m_pDuelTournamentRankingLabel = new MBitmapR2;
	((MBitmapR2*)m_pDuelTournamentRankingLabel)->Create( "DuelTournamentRankingLabel.png", RGetDevice(), "interface/loadable/DuelTournamentRankingLabel.png");
	if ( m_pDuelTournamentRankingLabel != NULL)
	{
		pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentRankingListLabel");
		if ( pPicture)	pPicture->SetBitmap( m_pDuelTournamentRankingLabel->GetSourceBitmap());
	}

	ZDuelTournamentRankingListBox* pDTRankingListBox = (ZDuelTournamentRankingListBox*)m_IDLResource.FindWidget("Lobby_DuelTournamentRankingList");
	if (pDTRankingListBox) pDTRankingListBox->LoadInterfaceImgs();

	// music
#ifdef _BIRDSOUND
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY);
	ZApplication::GetSoundEngine()->PlayMusic( true);
#else
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY, ZApplication::GetFileSystem());
	ZApplication::GetSoundEngine()->PlayMusic( true);
#endif

//	m_pBackground->SetScene(SCENE_TOWN);

	//ZCharacterViewList* pCharacterViewList = NULL;
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_STAGE);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->RemoveAll();
	//}
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_LOBBY);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->Assign(ZGetGameClient()->GetObjCacheMap());
	//}

//	SetupPlayerListTab();

	ZPlayerListBox *pPlayerListBox = (ZPlayerListBox*)m_IDLResource.FindWidget( "LobbyChannelPlayerList" );
	if(pPlayerListBox)
		pPlayerListBox->SetMode(ZPlayerListBox::PLAYERLISTMODE_CHANNEL);

//	InitLadderUI();

	// A����AAO��A���� ����A��������| ��AAcA����U.
	pWidget= m_IDLResource.FindWidget( "ChannelChattingInput" );
	if(pWidget) pWidget->SetFocus();

	if (m_pBackground)
#ifdef _CHURCH
		m_pBackground->Free();
#else
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDCHAR);
#endif

	// UI �������AI����
	bool bEnable = ZGetGameClient()->GetEnableInterface();

	pWidget = m_IDLResource.FindWidget( "StageJoin");
	if ( pWidget)		pWidget->Enable( bEnable);

	pWidget = m_IDLResource.FindWidget( "StageCreateFrameCaller");
	if ( pWidget)		pWidget->Enable( bEnable);
	
	pWidget = m_IDLResource.FindWidget( "QuickJoin");
	if ( pWidget)		pWidget->Enable( bEnable);
	
	pWidget = m_IDLResource.FindWidget( "QuickJoin2");
	if ( pWidget)		pWidget->Enable( bEnable);
	
	pWidget = m_IDLResource.FindWidget( "ArrangedTeamGame");
	if ( pWidget)		pWidget->Enable( bEnable);

	pWidget = m_IDLResource.FindWidget( "ChannelChattingInput");
	if ( pWidget)		pWidget->Enable( bEnable);

	pWidget = m_IDLResource.FindWidget( "Lobby_StageList");
	if ( pWidget)		pWidget->Enable( bEnable);

	InitLobbyUIByChannelType();
}

void ZGameInterface::InitLobbyUIByChannelType()
{
	bool bClanBattleUI =  ((ZGetGameClient()->GetServerMode() == MSM_CLAN) && (ZGetGameClient()->GetChannelType()==MCHANNEL_TYPE_CLAN));
	bool bDuelTournamentUI = (ZGetGameClient()->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT);

	// ���������Ƣ� A����aCI��U.. trueAIAEA�� ����A������ E��AaC��Aa���� CN��U (A��CIAo���� AIA|��I���� CO��o CN������͡�I COAAAO��a��� A�� ��I��A��i...)
	if (bClanBattleUI)
	{
		ZGetGameInterface()->InitDuelTournamentLobbyUI(false);
		ZGetGameInterface()->InitClanLobbyUI(true);
	}
	else if (bDuelTournamentUI)
	{
		ZGetGameInterface()->InitClanLobbyUI(false);
		ZGetGameInterface()->InitDuelTournamentLobbyUI(true);

		ZPostDuelTournamentRequestSideRankingInfo( ZGetMyUID());
	}
	else
	{
		ZGetGameInterface()->InitClanLobbyUI(false);
		ZGetGameInterface()->InitDuelTournamentLobbyUI(false);
	}

	if (bDuelTournamentUI)
		ZPostDuelTournamentRequestSideRankingInfo( ZGetMyUID());
}


void ZGameInterface::OnLobbyDestroy(void)
{
	ShowWidget("Lobby", false);

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_RoomListBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_BottomBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);
    
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_ClanInfoBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentInfoBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentRankingListLabel");
	if ( pPicture)
		pPicture->SetBitmap( NULL);


	SAFE_DELETE(m_pRoomListFrame);
	SAFE_DELETE(m_pDuelTournamentLobbyFrame);
	SAFE_DELETE(m_pBottomFrame);
	SAFE_DELETE(m_pClanInfoBg);
	SAFE_DELETE(m_pDuelTournamentInfoBg);
	SAFE_DELETE(m_pDuelTournamentRankingLabel);

	ZDuelTournamentRankingListBox* pDTRankingListBox = (ZDuelTournamentRankingListBox*)m_IDLResource.FindWidget("Lobby_DuelTournamentRankingList");
	if (pDTRankingListBox) {
		pDTRankingListBox->UnloadInterfaceImgs();
		pDTRankingListBox->SetVisible(false);
	}

	MWidget* pWidget = m_IDLResource.FindWidget("StageName");
	if(pWidget) ZGetApplication()->SetSystemValue("StageName", pWidget->GetText());
}

void ZGameInterface::OnStageCreate(void)
{
	///TODO: uncomment code
//	// WPE hacking protect
//#ifndef _DEBUG
	HMODULE hMod = GetModuleHandle( "ws2_32.dll"); 
	FARPROC RetVal = GetProcAddress( hMod, "recv"); 
	if ( ZCheckHackProcess() || (BYTE)RetVal == 0xE9)
	{
		//mlog( "Hacking detected");

//		MessageBox(NULL, ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		//ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);

		ZPostDisconnect();
	}
//#endif

	mlog("StageCreated\n");

	if (m_spGameClient)
	{
		m_spGameClient->ClearPeers();
	}

	ShowWidget("Shop", false);
	ShowWidget("Equipment", false);
	ShowWidget("Stage", true);
	EnableStageInterface(true);
	MButton* pObserverBtn = (MButton*)m_IDLResource.FindWidget("StageObserverBtn");
	if ( pObserverBtn)
		pObserverBtn->SetCheck( false);

	/*
	MListBox* pListBox  = (MListBox*)m_IDLResource.FindWidget("StageChattingOutput");
	if (pListBox != NULL)
	{
		pListBox->RemoveAll();		
	}
	*/

	//ZCharacterViewList* pCharacterViewList = NULL;
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_LOBBY);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->RemoveAll();
	//}
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_STAGE);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->Assign(ZGetGameClient()->GetObjCacheMap());
	//}

	ZCharacterView* pCharView = (ZCharacterView*)m_IDLResource.FindWidget("Stage_Charviewer");
	
	if( pCharView != NULL )
	{
		//MMatchObjCacheMap* pObjCacheMap = ZGetGameClient()->GetObjCacheMap();
		//for(MMatchObjCacheMap::iterator itor = pObjCacheMap->begin(); itor != pObjCacheMap->end(); ++itor)
		//{
		//	MMatchObjCache* pObj = (*itor).second;
		//	if( pObj->GetUID() == ZGetMyUID() )
		//	{
		//		pCharView->SetCharacter( pObj->GetUID() );
		//	}
		//}
		pCharView->SetCharacter( ZGetMyUID());
	}

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripBottom");
	if(pPicture != NULL)	pPicture->SetBitmapColor(0xFFFFFFFF);
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripTop");
	if(pPicture != NULL)	pPicture->SetBitmapColor(0xFFFFFFFF);

	ZPostRequestStageSetting(ZGetGameClient()->GetStageUID());
	SerializeStageInterface();

#ifdef _BIRDSOUND
		ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY);
		ZApplication::GetSoundEngine()->PlayMusic(true);
#else
		ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY, ZApplication::GetFileSystem());
		ZApplication::GetSoundEngine()->PlayMusic(true);
#endif
/*
#ifdef _BIRDSOUND
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY);
	ZApplication::GetSoundEngine()->PlayMusic();
#else
#ifndef _FASTDEBUG
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY, ZApplication::GetFileSystem());
	ZApplication::GetSoundEngine()->PlayMusic();
#endif
#endif
*/
/*
	// ����AaAI��e CoAc ��OAO ����A��A�� ��U��A ����A��CN��U.
	if ( ZGetGameClient() && ZGetGameClient()->AmIStageMaster())
	{
		MChannelRule* pRule = ZGetChannelRuleMgr()->GetRule( ZGetGameClient()->GetChannelRuleName());
		if ( pRule)
		{
			MComboBox* pCB = (MComboBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageType");				
			if ( pCB)
			{
				pCB->SetSelIndex( pRule->GetDefault());

				ZStageSetting::InitStageSettingGameFromGameType();
				ZStageSetting::PostDataToServer();
			}
		}
	}
*/
	ZApplication::GetStageInterface()->OnCreate();
}

void ZGameInterface::OnStageDestroy(void)
{
	ZApplication::GetStageInterface()->OnDestroy();
}

char* GetItemSlotName( const char* szName, int nItem)
{
	static char szTemp[256];
	strcpy( szTemp, szName);

	switch (nItem)
	{
		case MMCIP_HEAD :		strcat( szTemp, "_EquipmentSlot_Head");			break;
		case MMCIP_CHEST :		strcat( szTemp, "_EquipmentSlot_Chest");		break;
		case MMCIP_HANDS :		strcat( szTemp, "_EquipmentSlot_Hands");		break;
		case MMCIP_LEGS :		strcat( szTemp, "_EquipmentSlot_Legs");			break;
		case MMCIP_FEET :		strcat( szTemp, "_EquipmentSlot_Feet");			break;
		case MMCIP_FINGERL :	strcat( szTemp, "_EquipmentSlot_FingerL");		break;
		case MMCIP_FINGERR :	strcat( szTemp, "_EquipmentSlot_FingerR");		break;
		case MMCIP_AVATAR :		strcat( szTemp, "_EquipmentSlot_Avatar");		break;
		case MMCIP_MELEE :		strcat( szTemp, "_EquipmentSlot_Melee");		break;
		case MMCIP_PRIMARY :	strcat( szTemp, "_EquipmentSlot_Primary");		break;
		case MMCIP_SECONDARY :	strcat( szTemp, "_EquipmentSlot_Secondary");	break;
		case MMCIP_CUSTOM1 :	strcat( szTemp, "_EquipmentSlot_Custom1");		break;
		case MMCIP_CUSTOM2 :	strcat( szTemp, "_EquipmentSlot_Custom2");		break;
		case MMCIP_COMMUNITY1:	strcat( szTemp, "_EquipmentSlot_Community1");	break;
		case MMCIP_COMMUNITY2:	strcat( szTemp, "_EquipmentSlot_Community2");	break;
		case MMCIP_LONGBUFF1 :	strcat( szTemp, "_EquipmentSlot_LongBuff1");	break;
		case MMCIP_LONGBUFF2 :	strcat( szTemp, "_EquipmentSlot_LongBuff2");	break;
		case MMCIP_DASH:		strcat( szTemp, "_EquipmentSlot_Dash");			break;
			//TODO: decide if i want custom3 to be usable everywhere
		default: _ASSERT(0); break;
	}

	return szTemp;
}

bool ZGameInterface::OnCreate(ZLoadingProgress *pLoadingProgress)
{
	if (!m_sbRemainClientConnectionForResetApp)
		m_spGameClient = new ZGameClient();

	if(!m_Tips.Initialize(ZApplication::GetFileSystem(), ZGetLocale()->GetLanguage())) {
		mlog("Check tips.xml\n");
		return false;
	}
	
	ZLoadingProgress interfaceProgress("interfaceSkin",pLoadingProgress,.7f);
	if(!InitInterface(ZGetConfiguration()->GetInterfaceSkinName(),&interfaceProgress))
	{
		mlog("ZGameInterface::OnCreate: Failed InitInterface\n");
		return false;
	}

	interfaceProgress.UpdateAndDraw(1.f);

	/*
	if( !InitLocatorList(ZApplication::GetFileSystem(), "system/ZLocatorList.xml") )
	{
		mlog( "ZGameInterface::OnCreate: Fail InitLocatorList\n" );
		// ��IAEAIAI��| ���� A����i��� ��OAOA�� A��CaA�� CO. 
		// return false;
	}
	*/
	

	// mlog("ZGameInterface::OnCreate : InitInterface \n");

	m_pScreenEffectManager=new ZScreenEffectManager;
	if(!m_pScreenEffectManager->Create()) 
		return false;

	mlog("Screen effect manager create success.\n" );

	m_pEffectManager = new ZEffectManager;
	if(!m_pEffectManager->Create())
		return false;

	mlog("Effect manager create success.\n");
	
	//m_pCursorSurface=CreateImageSurface("Interface/Default/cursor.png");
	//if( m_pCursorSurface != NULL )
		//RGetDevice()->SetCursorProperties(0,0,m_pCursorSurface);

//	ResetCursor();

	SetTeenVersion(ZGetLocale()->IsTeenMode());

	if (!m_sbRemainClientConnectionForResetApp)
	{
		int nNetworkPort = RandomNumber( ZGetConfiguration()->GetEtc()->nNetworkPort1, ZGetConfiguration()->GetEtc()->nNetworkPort2);
		if (m_spGameClient->Create( nNetworkPort) == false) {
			string strMsg = "Unknown Network Error";
			if (GetLastError() == WSAEADDRINUSE)
				NotifyMessage(MATCHNOTIFY_NETWORK_PORTINUSE, &strMsg);
			else
				NotifyMessage(MATCHNOTIFY_NETWORK_CREATE_FAILED, &strMsg);
			ShowMessage(strMsg.c_str());
		}
		m_spGameClient->SetOnCommandCallback(OnCommand);
		m_spGameClient->CreateUPnP(nNetworkPort);
		m_MyPort = nNetworkPort;
			
		mlog("Client create success.\n");
	}

//	SetState(m_nInitialState);

	// ��oA�� ���� Aa��n AE��aE�� ����A��
	ZItemSlotView* itemSlot;
	for(int i = 0;  i < MMCIP_END;  i++)
	{
		if (i == MMCIP_CUSTOM3)
			continue;
		itemSlot = (ZItemSlotView*)m_IDLResource.FindWidget( GetItemSlotName( "Shop", i));
		if ( itemSlot)
		{
			strcpy( itemSlot->m_szItemSlotPlace, "Shop");

			// ����AO Au���� CI��aAU��u... -_-;
			switch ( itemSlot->GetParts())
			{
			case MMCIP_HEAD:		itemSlot->SetText( ZMsg( MSG_WORD_HEAD));				break;
			case MMCIP_CHEST:		itemSlot->SetText( ZMsg( MSG_WORD_CHEST));				break;
			case MMCIP_HANDS:		itemSlot->SetText( ZMsg( MSG_WORD_HANDS));				break;
			case MMCIP_LEGS:		itemSlot->SetText( ZMsg( MSG_WORD_LEGS));				break;
			case MMCIP_FEET:		itemSlot->SetText( ZMsg( MSG_WORD_FEET));				break;
			case MMCIP_FINGERL:		itemSlot->SetText( ZMsg( MSG_WORD_LFINGER));			break;
			case MMCIP_FINGERR:		itemSlot->SetText( ZMsg( MSG_WORD_RFINGER));			break;
			case MMCIP_AVATAR:		itemSlot->SetText( ZMsg( MSG_WORD_AVATAR));				break;
			case MMCIP_MELEE:		itemSlot->SetText( ZMsg( MSG_WORD_MELEE));				break;
			case MMCIP_PRIMARY:		itemSlot->SetText( ZMsg( MSG_WORD_WEAPON1));			break;
			case MMCIP_SECONDARY:	itemSlot->SetText( ZMsg( MSG_WORD_WEAPON2));			break;
			case MMCIP_CUSTOM1:		itemSlot->SetText( ZMsg( MSG_WORD_ITEM1));				break;
			case MMCIP_CUSTOM2:		itemSlot->SetText( ZMsg( MSG_WORD_ITEM2));				break;
			case MMCIP_COMMUNITY1:	itemSlot->SetText( ZMsg( MSG_WORD_COMMUNITY_ITEM1));	break;
			case MMCIP_COMMUNITY2:	itemSlot->SetText( ZMsg( MSG_WORD_COMMUNITY_ITEM2));	break;
			case MMCIP_LONGBUFF1:	itemSlot->SetText( ZMsg( MSG_WORD_LONGBUFF_ITEM1));		break;
			case MMCIP_LONGBUFF2:	itemSlot->SetText( ZMsg( MSG_WORD_LONGBUFF_ITEM2));		break;
			case MMCIP_DASH:		itemSlot->SetText("Dash"); break;
			default :				itemSlot->SetText("");									break;
				//todo: decide if i want to enable custom3 everywhere

			}
		}
	}
	for(int i = 0;  i < MMCIP_END;  i++)
	{
		if (i == MMCIP_CUSTOM3)
			continue;
		itemSlot = (ZItemSlotView*)m_IDLResource.FindWidget( GetItemSlotName( "Equip", i));
		if ( itemSlot)
		{
			strcpy( itemSlot->m_szItemSlotPlace, "Equip");

			// ����AO Au���� CI��aAU��u... -_-;
			switch ( itemSlot->GetParts() )
			{
			case MMCIP_HEAD:		itemSlot->SetText( ZMsg( MSG_WORD_HEAD));				break;
			case MMCIP_CHEST:		itemSlot->SetText( ZMsg( MSG_WORD_CHEST));				break;
			case MMCIP_HANDS:		itemSlot->SetText( ZMsg( MSG_WORD_HANDS));				break;
			case MMCIP_LEGS:		itemSlot->SetText( ZMsg( MSG_WORD_LEGS));				break;
			case MMCIP_FEET:		itemSlot->SetText( ZMsg( MSG_WORD_FEET));				break;
			case MMCIP_FINGERL:		itemSlot->SetText( ZMsg( MSG_WORD_LFINGER));			break;
			case MMCIP_FINGERR:		itemSlot->SetText( ZMsg( MSG_WORD_RFINGER));			break;
			case MMCIP_AVATAR:		itemSlot->SetText( ZMsg( MSG_WORD_AVATAR));				break;
			case MMCIP_MELEE:		itemSlot->SetText( ZMsg( MSG_WORD_MELEE));				break;
			case MMCIP_PRIMARY:		itemSlot->SetText( ZMsg( MSG_WORD_WEAPON1));			break;
			case MMCIP_SECONDARY:	itemSlot->SetText( ZMsg( MSG_WORD_WEAPON2));			break;
			case MMCIP_CUSTOM1:		itemSlot->SetText( ZMsg( MSG_WORD_ITEM1));				break;
			case MMCIP_CUSTOM2:		itemSlot->SetText( ZMsg( MSG_WORD_ITEM2));				break;
			case MMCIP_COMMUNITY1:	itemSlot->SetText( ZMsg( MSG_WORD_COMMUNITY_ITEM1));	break;
			case MMCIP_COMMUNITY2:	itemSlot->SetText( ZMsg( MSG_WORD_COMMUNITY_ITEM2));	break;
			case MMCIP_LONGBUFF1:	itemSlot->SetText( ZMsg( MSG_WORD_LONGBUFF_ITEM1));		break;
			case MMCIP_LONGBUFF2:	itemSlot->SetText( ZMsg( MSG_WORD_LONGBUFF_ITEM2));		break;
			case MMCIP_DASH:		itemSlot->SetText("Dash");  break;
			default :				itemSlot->SetText("");									break;
			}
		}
	}
	ZShopEquipListbox* pItemListBox = (ZShopEquipListbox*)m_IDLResource.FindWidget("EquipmentList");
	if ( pItemListBox)
	{
		//����AIAU ��iA����? �������� Ao��YA�� ��E������I��i AO����
		//pItemListBox->AttachMenu(new ZItemMenu("ItemMenu", pItemListBox, pItemListBox, MPMT_VERTICAL));	
		pItemListBox->EnableDragAndDrop( true);
	}

	// ����CA����AI ��U���� AE��aE��
	MListBox* pReplayBox = (MListBox*)m_IDLResource.FindWidget( "Replay_FileList");
	if ( pReplayBox)
	{
		pReplayBox->m_FontAlign = MAM_VCENTER;
		pReplayBox->SetVisibleHeader( false);
		pReplayBox->AddField( "NAME", 92);
		pReplayBox->AddField( "VERSION", 8);
		pReplayBox->m_bAbsoulteTabSpacing = false;
	}


	// Setting Configuration about ZGameClient
	if (Z_ETC_BOOST && RIsActive())
		m_spGameClient->PriorityBoost(true);

	m_spGameClient->SetRejectNormalChat(Z_ETC_REJECT_NORMALCHAT);
	m_spGameClient->SetRejectTeamChat(Z_ETC_REJECT_TEAMCHAT);
	m_spGameClient->SetRejectClanChat(Z_ETC_REJECT_CLANCHAT);
	m_spGameClient->SetRejectWhisper(Z_ETC_REJECT_WHISPER);
	m_spGameClient->SetRejectInvite(Z_ETC_REJECT_INVITE);


	// ��a��uE����e ������������U���� AE��aE��
	int nLineHeightTextArea = int(18/600.f * MGetWorkspaceHeight());	// 800*600 C����o��� ��aA��A����I 18CE����
	MTextArea* pTextArea = (MTextArea*)m_IDLResource.FindWidget( "CombatResult_PlayerNameList");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "CombatResult_PlayerKillList");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "CombatResult_PlayerDeathList");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "ClanResult_PlayerNameList1");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "ClanResult_PlayerKillList1");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "ClanResult_PlayerDeathList1");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "ClanResult_PlayerNameList2");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "ClanResult_PlayerKillList2");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "ClanResult_PlayerDeathList2");
	if ( pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget( "CombatResult_Header");
	if ( pPicture)
		pPicture->SetOpacity( 80);
	pPicture = (MPicture*)m_IDLResource.FindWidget( "ClanResult_Header1");
	if ( pPicture)
		pPicture->SetOpacity( 80);
	pPicture = (MPicture*)m_IDLResource.FindWidget( "ClanResult_Header2");
	if ( pPicture)
		pPicture->SetOpacity( 80);

	// ��������AI ��E
	{
		MComboBox* pCombo = (MComboBox*)m_IDLResource.FindWidget( "Stage_RelayMapType" );
		if ( pCombo)
		{
			pCombo->RemoveAll();
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_TURN));
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_RANDOM));
			pCombo->SetSelIndex(0);
		}
		pCombo = (MComboBox*)m_IDLResource.FindWidget( "Stage_RelayMapRepeatCount" );
		if ( pCombo)
		{
			pCombo->RemoveAll();
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_1ROUND));
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_2ROUND));
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_3ROUND));
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_4ROUND));
			pCombo->Add(ZMsg( MSG_GAME_RELAYMAP_5ROUND));
			pCombo->SetSelIndex(2);
		}

		// ��E ����������� ��U���� AE��aE��
		MListBox* pMapListBox = (MListBox*)m_IDLResource.FindWidget( "Stage_MapListbox" );
		if ( pMapListBox)
		{
			pMapListBox->m_FontAlign = MAM_LEFT;
			pMapListBox->AddField( "ICON", 23);
			pMapListBox->AddField( "NAME", 170);
			pMapListBox->SetItemHeight( 23);
			pMapListBox->SetVisibleHeader( false);
			pMapListBox->m_bNullFrame = true;
		}
		// ��������AI��E ����������� ��U���� AE��aE��
		MListBox* pRelayMapListBox = (MListBox*)m_IDLResource.FindWidget( "Stage_RelayMapListbox" );
		if ( pRelayMapListBox)
		{
			pRelayMapListBox->m_FontAlign = MAM_LEFT;
			pRelayMapListBox->AddField( "ICON", 23);
			pRelayMapListBox->AddField( "NAME", 170);
			pRelayMapListBox->SetItemHeight( 23);
			pRelayMapListBox->SetVisibleHeader( false);
			pRelayMapListBox->m_bNullFrame = true;
		}

		MListBox* pSpyBanMapList = (MListBox*)m_IDLResource.FindWidget("Stage_SpyBanMapList");
		if (pSpyBanMapList)
		{
			pSpyBanMapList->m_FontAlign = MAM_LEFT;
			pSpyBanMapList->AddField("ICON", 23);
			pSpyBanMapList->AddField("NAME", 170);
			pSpyBanMapList->SetItemHeight(23);
			pSpyBanMapList->SetVisibleHeader(false);
			pSpyBanMapList->m_bNullFrame = true;
		}
	}

	// En��y ����AIAU ����������� ��U���� AE��aE��
	MListBox* pListBox = (MListBox*)m_IDLResource.FindWidget( "Stage_SacrificeItemListbox");
	if ( pListBox)
	{
		pListBox->m_FontAlign = MAM_VCENTER;
		pListBox->AddField( "ICON", 32);
		pListBox->AddField( "NAME", 170);
		pListBox->SetItemHeight( 32);
		pListBox->SetVisibleHeader( false);
		pListBox->m_bNullFrame = true;
		pListBox->EnableDragAndDrop( true);
		pListBox->SetOnDropCallback( OnDropCallbackRemoveSacrificeItem);
	}

	// En��y ����AIAU ������O AE��aE��
	ZItemSlotView* questitemSlot = (ZItemSlotView*)m_IDLResource.FindWidget( "Stage_SacrificeItemButton0");
	if (questitemSlot)
	{
		questitemSlot->EnableDragAndDrop( true);
		strcpy(questitemSlot->m_szItemSlotPlace, "SACRIFICE0");
	}
	questitemSlot = (ZItemSlotView*)m_IDLResource.FindWidget( "Stage_SacrificeItemButton1");
	if (questitemSlot)
	{
		questitemSlot->EnableDragAndDrop( true);
		strcpy(questitemSlot->m_szItemSlotPlace, "SACRIFICE1");
	}

	// E���쩡 ����AIAU ����������� ��U���� AE��aE��
	pListBox = (MListBox*)m_IDLResource.FindWidget( "QuestResult_ItemListbox");
	if ( pListBox)
	{
		pListBox->m_FontAlign = MAM_VCENTER;
		pListBox->AddField( "ICON", 35);
		pListBox->AddField( "NAME", 300);
		pListBox->SetItemHeight( 32);
		pListBox->SetVisibleHeader( false);
		pListBox->SetFont( MFontManager::Get( "FONTa10b"));
		pListBox->m_FontColor = MCOLOR( 0xFFFFF794);
		pListBox->m_bNullFrame = true;
	}

	// ������UAI��u ����A�� ����������� ��U���� AE��aE��
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "SurvivalResult_MyRank");
	if (pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "SurvivalResult_MyName");
	if (pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "SurvivalResult_MyRankPoint");
	if (pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "SurvivalResult_PlayerRankList");
	if (pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "SurvivalResult_PlayerNameList");
	if (pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}
	pTextArea = (MTextArea*)m_IDLResource.FindWidget( "SurvivalResult_PlayerRankPointList");
	if (pTextArea)
	{
		pTextArea->SetFont( MFontManager::Get( "FONTa10b"));
		pTextArea->SetCustomLineHeight( nLineHeightTextArea);
	}

	// BmNumLabel AE��aE��
	int nMargin[ BMNUM_NUMOFCHARSET] = { 15,15,15,15,15,15,15,15,15,15,8,10,8 };
	int nMarginNarrow[ BMNUM_NUMOFCHARSET] = { 12,12,12,12,12,12,12,12,12,12,8,10,8 };
	ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_ClanInfoWinLose");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_ClanInfoPoints");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_ClanInfoTotalPoints");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_ClanInfoRanking");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoWinLose");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoPoints");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoWinners");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoRanking");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoWinLosePrev");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoPointsPrev");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoWinnersPrev");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoRankingPrev");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMarginNarrow);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "QuestResult_GetPlusXP");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "QuestResult_GetMinusXP");
	if ( pBmNumLabel)
	{
		pBmNumLabel->SetCharMargin( nMargin);
		pBmNumLabel->SetIndexOffset( 16);
	}
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "QuestResult_GetTotalXP");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "QuestResult_GetBounty");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "SurvivalResult_GetReachedRound");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "SurvivalResult_GetPoint");
	if ( pBmNumLabel)
		pBmNumLabel->SetCharMargin( nMargin);
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "SurvivalResult_GetXP");
	if ( pBmNumLabel)
	{
		pBmNumLabel->SetCharMargin( nMargin);
		pBmNumLabel->SetIndexOffset( 16);
	}
	pBmNumLabel = (ZBmNumLabel*)m_IDLResource.FindWidget( "SurvivalResult_GetBounty");
	if ( pBmNumLabel)
	{
		pBmNumLabel->SetCharMargin( nMargin);
		pBmNumLabel->SetIndexOffset( 16);
	}


	// ��AA�� A��AOA�� ��U��A A����i��eAI��U.
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_DEATHMATCH_SOLO, ZMsg( MSG_MT_DEATHMATCH_SOLO));
	//ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_CLASSIC_SOLO, "Deathmatch (Classic)");	
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_DEATHMATCH_TEAM, ZMsg( MSG_MT_DEATHMATCH_TEAM));
//	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_CLASSIC_TEAM, "Team Deathmatch (Classic)");	
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_GLADIATOR_SOLO, ZMsg( MSG_MT_GLADIATOR_SOLO));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_GLADIATOR_TEAM, ZMsg( MSG_MT_GLADIATOR_TEAM));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_ASSASSINATE, ZMsg( MSG_MT_ASSASSINATE));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_TRAINING, ZMsg( MSG_MT_TRAINING));

	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_SURVIVAL, ZMsg( MSG_MT_SURVIVAL));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_QUEST, ZMsg( MSG_MT_QUEST));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_BERSERKER, ZMsg( MSG_MT_BERSERKER));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_DEATHMATCH_TEAM2, "Team Deathmatch + Extreme");
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_DUEL, ZMsg( MSG_MT_DUEL));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_DUELTOURNAMENT, ZMsg( MSG_MT_DUELTOURNAMENT));
	ZGetGameTypeManager()->SetGameTypeStr( MMATCH_GAMETYPE_CTF, "Capture The Flag");

	MWidget* pWidget = m_IDLResource.FindWidget( "MonsterBookCaller");
	if ( pWidget)
		pWidget->Show( false);

	MComboBox* pCombo = (MComboBox*)m_IDLResource.FindWidget( "Shop_AllEquipmentFilter");
	if ( pCombo)
	{
		pCombo->SetAlignment( MAM_LEFT);
		pCombo->SetListboxAlignment( MAM_LEFT);
	}
	pCombo = (MComboBox*)m_IDLResource.FindWidget( "Equip_AllEquipmentFilter");
	if ( pCombo)
	{
		pCombo->SetAlignment( MAM_LEFT);
		pCombo->SetListboxAlignment( MAM_LEFT);
	}

	pCombo = (MComboBox*)m_IDLResource.FindWidget( "StageType");
	if ( pCombo)
		pCombo->SetListboxAlignment( MAM_LEFT);


	// A����?AI ��a��i
	ZCharacterView* pCharView = (ZCharacterView*)m_IDLResource.FindWidget( "EquipmentInformation");
	if ( pCharView)
		pCharView->EnableAutoRotate( true);
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget( "EquipmentInformationShop");
	if ( pCharView)
		pCharView->EnableAutoRotate( true);
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget( "EquipmentInformationLobby");
	if ( pCharView)
		pCharView->EnableAutoRotate( true);

	// ��a��oAa��E��O���� ��I��n AuAuUI���� ������i A����A A���Ƣ�
	char szTooltip[256];
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_WINLOSE_PREV);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoWinLosePrev", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_TP_PREV);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoPointsPrev", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_WINNER_PREV);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoWinnersPrev", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_RANK_PREV);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoRankingPrev", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_WINLOSE);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoWinLose", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_TP);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoPoints", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_WINNER);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoWinners", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_RANK);
	SetWidgetToolTipText("Lobby_DuelTournamentInfoRanking", szTooltip, MAM_RIGHT | MAM_TOP);
	ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_SIDERANKLIST);
	SetWidgetToolTipText("Lobby_DuelTournamentRankingList", szTooltip, MAM_RIGHT | MAM_TOP);

	SetCursorEnable(true);

	mlog( "game interface create success.\n" );

	return true;
}

void ZGameInterface::OnDestroy()
{
	mlog("Destroy interface begin \n");

	MTextArea* pTextArea = (MTextArea*)m_IDLResource.FindWidget("Shop_ItemDescription");
	if (pTextArea) pTextArea->ChangeCustomLook(NULL);

	//ZCharacterViewList* pCharViewList = (ZCharacterViewList*)m_IDLResource.FindWidget("StagePlayerList");
	//for(list<ZMeshView*>::iterator iter = ZMeshView::msMeshViewList.begin(); iter != ZMeshView::msMeshViewList.end(); ++iter )
	//{
	//	ZMeshView* pmv = *iter;
	//	if( pmv )
	//		pmv->OnInvalidate();
	//}

	ZCharacterView* pCharView = (ZCharacterView*)m_IDLResource.FindWidget("Stage_Charviewer");
	if(pCharView!= 0) pCharView->OnInvalidate();

	SetCursorEnable(false);

	SetState(GUNZ_NA);	// CoAc GunzState A����a

	SAFE_DELETE(m_pMapThumbnail);

	if (!m_sbRemainClientConnectionForResetApp)
	{
		m_spGameClient->Destroy();
		mlog("destroy client.\n");
		
		SAFE_DELETE(m_spGameClient);
		// mlog("SAFE_DELETE(m_spGameClient) : \n");
	}

	SAFE_DELETE(m_pLoginBG);			// C��A| ��E��C��A �Ʃ���i�Ƣ� AO�������� -_-;

	m_Tips.Finalize();
	FinalInterface();

	mlog("Destroy game interface done.\n");
}

void ZGameInterface::OnShutdownState()
{
	mlog("ZGameInterface::OnShutdown() : begin \n");
	
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MLabel* pLabel = (MLabel*)pResource->FindWidget("NetmarbleLoginMessage");
//			pLabel->SetText(MGetErrorString(MERR_CLIENT_DISCONNECTED));
			pLabel->SetText( ZErrStr(MERR_CLIENT_DISCONNECTED) );
			ZApplication::GetGameInterface()->ShowWidget("NetmarbleLogin", true);

	mlog("ZGameInterface::OnShutdown() : done() \n");
}

/*
void ZGameInterface::DrawLoadingScreen(const char* szMessage, float t)
{
	MDrawContext* pDC = Mint::GetInstance()->GetDrawContext();
	pDC->SetColor(0, 0, 0);
	pDC->FillRectangle(0, 0, MGetWorkspaceWidth(), MGetWorkspaceHeight());
	pDC->SetColor(255, 255, 255);
	pDC->Text(10, MGetWorkspaceHeight()-20, szMessage);
	t = min(max(0, t), 1);
	pDC->FillRectangle(0, 0, int(MGetWorkspaceWidth()*t), MGetWorkspaceHeight());

	// Update Scene
	Mint::GetInstance()->Update();
}
*/

bool ZGameInterface::SetState(GunzState nState)
{

	// AIAu A��A����I CoAc A��A���Ƣ� �ơ�A����e �����A
	if ( m_nState == nState)
		return true;

	if ( nState == GUNZ_PREVIOUS)
		nState = m_nPreviousState;


	// ��OAOA�� ��AAU(GUNZ_GAME)CI��a Au���� Au�������� ������a ��AAU ���쨬n��| ��������AU CE��a�Ƣ� AO��AAo ��E��c
	if ( nState == GUNZ_GAME)
	{
		if ( ZApplication::GetStageInterface()->IsShowStartMovieOfQuest())
		{
			ZApplication::GetStageInterface()->ChangeStageEnableReady( true);
			MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageReady");
			if ( pWidget)
				pWidget->Enable( false);
			ZApplication::GetStageInterface()->StartMovieOfQuest();
			return true;
		}
	}

	m_nPreviousState = m_nState;

	if(m_nState==GUNZ_GAME) OnGameDestroy();
	else if(m_nState==GUNZ_LOGIN)
	{
		MWidget* pWidget = m_IDLResource.FindWidget( "Login_BackgrdImg");
		if ( !pWidget)
			OnLoginDestroy();
	}
	else if(m_nState==GUNZ_DIRECTLOGIN) OnDirectLoginDestroy();
	else if(m_nState==GUNZ_LOBBY) OnLobbyDestroy();
	else if(m_nState==GUNZ_STAGE) OnStageDestroy();
	else if(m_nState==GUNZ_GREETER) OnGreeterDestroy();
	else if(m_nState==GUNZ_CHARSELECTION)
	{
		OnCharSelectionDestroy();

		// �Ƣ��iAO��A Au�������� ����AIAU ��n��I ��aA��
		if ( nState == GUNZ_LOBBY)
		{
			ZGetMyInfo()->GetItemList()->QuestItemClear();
			ZPostRequestGetCharQuestItemInfo( ZGetGameClient()->GetPlayerUID());
		}
	}
	else if(m_nState==GUNZ_CHARCREATION) OnCharCreationDestroy();

	bool bStateChanged = true;
	if(nState==GUNZ_GAME) bStateChanged = OnGameCreate();
	else if(nState==GUNZ_LOGIN) OnLoginCreate();
	else if(nState==GUNZ_DIRECTLOGIN) OnDirectLoginCreate();
	else if(nState==GUNZ_LOBBY)	OnLobbyCreate();
	else if(nState==GUNZ_STAGE) OnStageCreate();
	else if(nState==GUNZ_GREETER) OnGreeterCreate();
	else if(nState==GUNZ_CHARSELECTION)
	{
		if ( m_nPreviousState == GUNZ_LOGIN)
		{
			MWidget* pWidget = m_IDLResource.FindWidget( "Login_BackgrdImg");
			if ( !pWidget)
				OnCharSelectionCreate();
			else
			{
				m_nLoginState = LOGINSTATE_LOGINCOMPLETE;
				m_dwLoginTimer = timeGetTime() + 1000;
				return true;
			}
		}
		else
			OnCharSelectionCreate();
	}
	else if(nState==GUNZ_CHARCREATION) OnCharCreationCreate();
	else if(nState==GUNZ_SHUTDOWN) OnShutdownState();


	if(bStateChanged==false){
		m_pMsgBox->SetText("Error: Can't Create a Game!");
		m_pMsgBox->Show(true, true);
		SetState(GUNZ_PREVIOUS);
	}
	else{
		m_nState = nState;
	}

	m_nDrawCount = 0;
	return true;
}

namespace RealSpace2 {
	//extern int g_nCheckWallPolygons,g_nRealCheckWallPolygons;
	//extern int g_nCheckFloorPolygons,g_nRealCheckFloorPolygons;
	extern int g_nPoly, g_nCall;
	extern int g_nPickCheckPolygon, g_nRealPickCheckPolygon;
}

#ifndef _PUBLISH
#include "fmod.h"
#endif


/*
void ZGameInterface::OnUpdateGameMessage(void)
{
switch (ZGetGame()->GetMatch()->GetMatchState())
{
case ZMS_ROUND_READY:
{
int nRoundReadyCount = ZGetGame()->GetMatch()->GetRoundReadyCount();
if(nRoundReadyCount<-1){
ShowWidget(CENTERMESSAGE, false);
return;
}

char szReadyMessage[256] = "";
if(nRoundReadyCount>0){
sprintf(szReadyMessage, "Round %d : Start in %d", 
ZGetGame()->GetMatch()->GetNowRound()+1, nRoundReadyCount);
}
else{
strcpy(szReadyMessage, "Fight");
}

ShowWidget(CENTERMESSAGE, true);
SetTextWidget(CENTERMESSAGE, szReadyMessage);
}
break;
case ZMS_ROUND_PLAYING:
{
ShowWidget(CENTERMESSAGE, false);
}
break;
case ZMS_ROUND_FINISH:
{
char szReadyMessage[256] = "";
sprintf(szReadyMessage, "Finish!");

ShowWidget(CENTERMESSAGE, true);
SetTextWidget(CENTERMESSAGE, szReadyMessage);
}
break;
case ZMS_GAME_FINISH:
{
ShowWidget(CENTERMESSAGE, false);
}
break;
}

}
*/

void ZGameInterface::OnDrawStateGame(MDrawContext* pDC)
{
	if(m_pGame!=NULL) 
	{
		if(!IsMiniMapEnable())
			m_pGame->Draw();

		if (m_bViewUI) {

			if(m_bLeaveBattleReserved)
			{
				int nSeconds = (m_dwLeaveBattleTime - timeGetTime() + 999 ) / 1000;
				m_pCombatInterface->SetDrawLeaveBattle(m_bLeaveBattleReserved,nSeconds);
			}else
				m_pCombatInterface->SetDrawLeaveBattle(false,0);

			if(GetCamera()->GetLookMode()==ZCAMERA_MINIMAP) {
				_ASSERT(m_pMiniMap);
				m_pMiniMap->OnDraw(pDC);
			}

			// �������碥A ���������ҡש��碯�� ������c ��a��C
			m_pCombatInterface->OnDrawCustom(pDC);
		}

	}
	m_ScreenDebugger.DrawDebugInfo(pDC);
}

void ZGameInterface::OnDrawStateLogin(MDrawContext* pDC)
{
	if( m_pBackground!=0)
	{
	//	m_pBackground->SetScene(LOGIN_SCEN);
		m_pBackground->LoadMesh();
		m_pBackground->Draw();
	}


	MLabel* pConnectingLabel = (MLabel*)m_IDLResource.FindWidget( "Login_ConnectingMsg");
	if ( pConnectingLabel)
	{
		char szMsg[ 128];
		memset( szMsg, 0, sizeof( szMsg));
        int nCount = ( timeGetTime() / 800) % 4;
		for ( int i = 0;  i < nCount;  i++)
			szMsg[ i] = '<';
		sprintf( szMsg, "%s %s ", szMsg, "Connecting");
		for ( int i = 0;  i < nCount;  i++)
			strcat( szMsg, ">");

		pConnectingLabel->SetText( szMsg);
		pConnectingLabel->SetAlignment( MAM_HCENTER | MAM_VCENTER);
	}


	MWidget* pWidget = m_IDLResource.FindWidget( "LoginFrame");
	MPicture* pPicture = (MPicture*)m_IDLResource.FindWidget( "Login_BackgrdImg");
	if ( !pWidget || !pPicture)
		return;


	ZServerView* pServerList = (ZServerView*)m_IDLResource.FindWidget( "SelectedServer");
	MEdit* pPassword = (MEdit*)m_IDLResource.FindWidget( "LoginPassword");
	MWidget* pLogin = m_IDLResource.FindWidget( "LoginOK");
	if ( pServerList && pPassword && pLogin)
	{
		if ( pServerList->IsSelected() && (int)strlen( pPassword->GetText()))
			pLogin->Enable( true);
		else
			pLogin->Enable( false);
	}


	DWORD dwCurrTime = timeGetTime();

	// Check timeout
	if ( m_bLoginTimeout && (m_dwLoginTimeout <= dwCurrTime))
	{
		m_bLoginTimeout = false;

		MLabel* pLabel = (MLabel*)m_IDLResource.FindWidget( "LoginError");
		if (pLabel)
			pLabel->SetText( ZErrStr( MERR_CLIENT_CONNECT_FAILED));

		MButton* pLoginOK = (MButton*)m_IDLResource.FindWidget( "LoginOK");
		if (pLoginOK)
			pLoginOK->Enable(true);

		pWidget->Show(true);

		if ( pConnectingLabel)
			pConnectingLabel->Show( false);
	}

	// Fade in
	if ( m_nLoginState == LOGINSTATE_FADEIN)
	{
		m_bLoginTimeout = false;

		if ( dwCurrTime >= m_dwLoginTimer)
		{
			int nOpacity = pPicture->GetOpacity() + 3;
			if ( nOpacity > 255)
				nOpacity = 255;

			pPicture->SetOpacity( nOpacity);

			m_dwLoginTimer = dwCurrTime + 9;
		}

		if ( pPicture->GetOpacity() == 255)
		{
			m_dwLoginTimer = dwCurrTime + 1000;
			m_nLoginState = LOGINSTATE_SHOWLOGINFRAME;
		}
		else
			pWidget->Show( false);
	}
	// Show login frame
	else if ( m_nLoginState == LOGINSTATE_SHOWLOGINFRAME)
	{
		m_bLoginTimeout = false;

		if ( timeGetTime() > m_dwLoginTimer)
		{
			m_nLoginState = LOGINSTATE_STANDBY;
			pWidget->Show( true);
		}
	}
	// Standby
	else if ( m_nLoginState == LOGINSTATE_STANDBY)
	{
#ifdef _LOCATOR
		// Refresh server status info
		if ( timeGetTime() > m_dwRefreshTime)
			RequestServerStatusListInfo();
#endif
	}
	// Login Complete
	else if ( m_nLoginState == LOGINSTATE_LOGINCOMPLETE)
	{
		m_bLoginTimeout = false;

		if ( timeGetTime() > m_dwLoginTimer)
			m_nLoginState = LOGINSTATE_FADEOUT;

		if ( pConnectingLabel)
			pConnectingLabel->Show( false);
	}
	// Fade out
	else if ( m_nLoginState == LOGINSTATE_FADEOUT)
	{
		m_bLoginTimeout = false;
		pWidget->Show(false);

		if ( dwCurrTime >= m_dwLoginTimer)
		{
			int nOpacity = pPicture->GetOpacity() - 3;
			if ( nOpacity < 0)
				nOpacity = 0;

			pPicture->SetOpacity( nOpacity);

			m_dwLoginTimer = dwCurrTime + 9;
		}

		if ( pPicture->GetOpacity() == 0)
		{
			OnLoginDestroy();
			
			m_nLoginState = LOGINSTATE_STANDBY;
			m_nState = GUNZ_CHARSELECTION;
			OnCharSelectionCreate();
		}
	}

	if(IsErrMaxPlayer())
	{
		if(m_dErrMaxPalyerDelayTime <= dwCurrTime)
		{
			ZPostDisconnect();
			SetErrMaxPlayer(false);
			ZApplication::GetGameInterface()->ShowErrorMessage( 10003 );
		}
	}
}

void ZGameInterface::OnDrawStateLobbyNStage(MDrawContext* pDC)
{
	ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();

	DWORD dwClock = timeGetTime();
	if ( (dwClock - m_dwFrameMoveClock) < 30)
		return;
	m_dwFrameMoveClock = dwClock;


	if( GetState() == GUNZ_LOBBY )
	{
		// TODO : AI��eA�� draw A��AO���� ��e��OC������ CO CE��a��A ������i����AI��U. Au��cCN �ơ�A����I ��A��aAU

		// Lobby
		char buf[512];
		// AI����
		MLabel* pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerName");
		if (pLabel)
		{
			sprintf( buf, "%s", ZGetMyInfo()->GetCharName() );
			pLabel->SetText(buf);
		}
		// ��o����A������ (�좯E?AI�Ƣ� A���Ƣ�)
		// Clan
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecClan");
		sprintf( buf, "%s : %s", ZMsg( MSG_CHARINFO_CLAN), ZGetMyInfo()->GetClanName());
		if (pLabel) pLabel->SetText(buf);
		// LV
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecLevel");
		sprintf( buf, "%s : %d %s", ZMsg( MSG_CHARINFO_LEVEL), ZGetMyInfo()->GetLevel(), ZMsg(MSG_CHARINFO_LEVELMARKER));
		if (pLabel) pLabel->SetText(buf);
		// XP
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecXP");
		sprintf( buf, "%s : %d%%", ZMsg( MSG_CHARINFO_XP), ZGetMyInfo()->GetLevelPercent());
		if (pLabel) pLabel->SetText(buf);
		// BP
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecBP");
		sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_BOUNTY), ZGetMyInfo()->GetBP());
		if (pLabel) pLabel->SetText(buf);
		// HP
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecHP");
		sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_HP), ZGetMyInfo()->GetHP());
		if (pLabel) pLabel->SetText(buf);
		// AP
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecAP");
		sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_AP), ZGetMyInfo()->GetAP());
		if (pLabel) pLabel->SetText(buf);
		// WT
		pLabel = (MLabel*)pRes->FindWidget("Lobby_PlayerSpecWT");
		ZMyItemList* pItems= ZGetMyInfo()->GetItemList();
		sprintf( buf, "%s : %d/%d", ZMsg( MSG_CHARINFO_WEIGHT), pItems->GetEquipedTotalWeight(), pItems->GetMaxWeight());
		if (pLabel) pLabel->SetText(buf);

		// A����I A������
		pLabel = (MLabel*)pRes->FindWidget("Lobby_ChannelName");
		sprintf( buf, "%s > %s > %s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_LOBBY), ZGetGameClient()->GetChannelName());
		if (pLabel) 
			pLabel->SetText(buf);
	}

	// Stage
	else if( GetState() == GUNZ_STAGE)
	{
		// CA����AI��i A������ C�ͨ�A
		char buf[512];
		MListBox* pListBox = (MListBox*)pRes->FindWidget( "StagePlayerList_");
		bool bShowMe = true;
		if ( pListBox)
		{
			ZStagePlayerListItem* pItem = NULL;
			if ( pListBox->GetSelIndex() < pListBox->GetCount())
			{
				if ( pListBox->GetSelIndex() >= 0)
					pItem = (ZStagePlayerListItem*)pListBox->Get( pListBox->GetSelIndex());

				if ( (pListBox->GetSelIndex() != -1) && (strcmp(ZGetMyInfo()->GetCharName(), pItem->GetString( 3)) != 0))
					bShowMe = false;
			}

			ZPlayerInfo* pInfo = NULL;
			if ( bShowMe)
				pInfo = ZGetPlayerManager()->Find( ZGetMyUID());
			else if ( pItem != NULL)
				pInfo = ZGetPlayerManager()->Find( pItem->GetUID());


			// AI����
			MLabel* pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerName");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf_s( buf, "%s", ZGetMyInfo()->GetCharName() );
				else
					sprintf_s( buf, "%s", pItem->GetString( 3));
				pLabel->SetText(buf);
			}

			// A������
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecClan");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %s", ZMsg( MSG_CHARINFO_CLAN), ZGetMyInfo()->GetClanName());
				else
				{
					if ( strcmp( pItem->GetString( 5), "") == 0)
						sprintf( buf, "%s :", ZMsg( MSG_CHARINFO_CLAN));
					else
						sprintf( buf, "%s : %s", ZMsg( MSG_CHARINFO_CLAN), pItem->GetString( 5));
				}
				pLabel->SetText(buf);
			}

			// ��������
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecLevel");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d %s", ZMsg( MSG_CHARINFO_LEVEL), ZGetMyInfo()->GetLevel(), ZMsg(MSG_CHARINFO_LEVELMARKER));
				else
					sprintf( buf, "%s : %s %s", ZMsg( MSG_CHARINFO_LEVEL), pItem->GetString( 1), ZMsg(MSG_CHARINFO_LEVELMARKER));
				pLabel->SetText(buf);
			}

			// XP
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecXP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d%%", ZMsg( MSG_CHARINFO_XP), ZGetMyInfo()->GetLevelPercent());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_XP));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// BP
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecBP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_BOUNTY), ZGetMyInfo()->GetBP());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_BOUNTY));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// HP
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecHP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_HP), ZGetMyInfo()->GetHP());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_HP));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// AP
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecAP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_AP), ZGetMyInfo()->GetAP());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_AP));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// WT
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecWT");
			if ( pLabel)
			{
				ZMyItemList* pItems= ZGetMyInfo()->GetItemList();
				if ( bShowMe)
					sprintf( buf, "%s : %d/%d", ZMsg( MSG_CHARINFO_WEIGHT), pItems->GetEquipedTotalWeight(), pItems->GetMaxWeight());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_WEIGHT));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}


			

			// Ranking
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecRanking");
			if ( pLabel && pInfo)
			{
				if ( pInfo->GetRank() == 0)
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_RANKING));
				else
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_RANKING), pInfo->GetRank());
				pLabel->SetText(buf);
			}

			// Kill
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecKill");
			if ( pLabel && pInfo)
			{
				sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_KILL), pInfo->GetKill());
				pLabel->SetText(buf);
			}

			// Death
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecDeath");
			if ( pLabel && pInfo)
			{
				sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_DEATH), pInfo->GetDeath());
				pLabel->SetText(buf);
			}

			// Winning percent
			pLabel = (MLabel*)pRes->FindWidget("Stage_PlayerSpecWinning");
			if ( pLabel && pInfo)
			{
				sprintf( buf, "%s : %.1f%%", ZMsg( MSG_CHARINFO_WINNING), pInfo->GetWinningRatio());
				pLabel->SetText(buf);
			}


			// Character View
			if ( bShowMe)
			{
				ZCharacterView* pCharView = (ZCharacterView*)pRes->FindWidget( "Stage_Charviewer");
				if ( pCharView)
					pCharView->SetCharacter( ZGetMyUID());
			}
		}


		// Au�������� ������a��I ��AAUCO�ҡ� ���쨬n��| ��������AU
		ZApplication::GetStageInterface()->OnDrawStartMovieOfQuest();


		// �������� AI��IAo Opacity A��Ay
		int nOpacity = 90.0f * ( sin( timeGetTime() * 0.003f) + 1) + 75;

		MLabel* pLabel = (MLabel*)pRes->FindWidget( "Stage_SenarioName");
		MPicture* pPicture = (MPicture*)pRes->FindWidget( "Stage_Lights0");
		if ( pPicture)
		{
			pPicture->SetOpacity( nOpacity);
		}
		pPicture = (MPicture*)pRes->FindWidget( "Stage_Lights1");
		if ( pPicture)
		{
			pPicture->SetOpacity( nOpacity);
		}



		// En��y ����AIAU ����������� CA����AO ��oA��AO
		MWidget* pWidget = pRes->FindWidget( "Stage_ItemListView");
		if ( !pWidget)
			return;

		int nEndPos = ZApplication::GetStageInterface()->m_nListFramePos;
		MRECT rect = pWidget->GetRect();
		if ( rect.x != nEndPos)
		{
			int nNewPos = rect.x + ( nEndPos - rect.x) * 0.25;
			if ( nNewPos == rect.x)		// not changed
				rect.x = nEndPos;
			else						// changed
				rect.x = nNewPos;

			pWidget->SetBounds( rect);

			if ( rect.x == 0)
			{
				pWidget = pRes->FindWidget( "Stage_CharacterInfo");
				if ( pWidget)
					pWidget->Enable( false);
			}
		}

		// ��������AI��E ����������� CA����AO ��oA��AO
		pWidget = pRes->FindWidget( "Stage_RelayMapListView");
		if ( !pWidget)
			return;

		nEndPos = ZApplication::GetStageInterface()->m_nRelayMapListFramePos;
		rect = pWidget->GetRect();
		if ( rect.x != nEndPos)
		{
			int nNewPos = rect.x + ( nEndPos - rect.x) * 0.25;
			if ( nNewPos == rect.x)		// not changed
				rect.x = nEndPos;
			else						// changed
				rect.x = nNewPos;

			pWidget->SetBounds( rect);

			if ( rect.x == 0)
			{
				pWidget = pRes->FindWidget( "Stage_CharacterInfo");
				if ( pWidget)
					pWidget->Enable( false);
			}
		}

		//Custom: spymode
		pWidget = pRes->FindWidget("Stage_SpyBanMapListView");
		if (!pWidget)
			return;

		nEndPos = ZApplication::GetStageInterface()->m_spyMapListFramePos;
		rect = pWidget->GetRect();
		if (rect.x != nEndPos)
		{
			int nNewPos = rect.x + (nEndPos - rect.x) * 0.25;
			if (nNewPos == rect.x)		// not changed
				rect.x = nEndPos;
			else						// changed
				rect.x = nNewPos;

			pWidget->SetBounds(rect);

			if (rect.x == 0)
			{
				pWidget = pRes->FindWidget("Stage_CharacterInfo");
				if (pWidget)
					pWidget->Enable(false);
			}
		}

	}
}

void ZGameInterface::OnDrawStateCharSelection(MDrawContext* pDC)
{
	if ( m_pBackground && m_pCharacterSelectView)
	{
		m_pBackground->LoadMesh();
		m_pBackground->Draw();
		m_pCharacterSelectView->Draw();

		// Draw effects(smoke, cloud)
		ZGetEffectManager()->Draw( timeGetTime());

		// Draw maiet logo effect
		//ZGetScreenEffectManager()->DrawEffects();
	}
}

void ZGameInterface::OnDraw(MDrawContext *pDC)
{
	m_nDrawCount++;

	__BP(11,"ZGameInterface::OnDraw");

	if(m_bLoading) 
	{
		__EP(11);
		return;
	}	

	switch (GetState())
	{
	case GUNZ_GAME:
		{
			OnDrawStateGame(pDC);
		}
		break;
	case GUNZ_LOGIN:
	case GUNZ_DIRECTLOGIN:
		{
			OnDrawStateLogin(pDC);
		}
		break;
	case GUNZ_LOBBY:
	case GUNZ_STAGE:
		{
			OnDrawStateLobbyNStage(pDC);
		}
		break;
	case GUNZ_CHARSELECTION:
	case GUNZ_CHARCREATION:
		{
			OnDrawStateCharSelection(pDC);
		}
		break;
	}

	__EP(11);
}



// AO��A

// ����AU��A 2����A�� AU���� ��IAI..

void ZGameInterface::TestChangePartsAll() 
{
}

void ZGameInterface::TestChangeParts(int mode) {

#ifndef _PUBLISH
	// �쩣AUAI��E��e... E��AU ��E�ơͨ��� AO��A A���������� CO�ҡ� ��c��eCN��U...

	RMeshPartsType ptype = eq_parts_etc;

 		 if(mode==0)	{ ptype = eq_parts_chest;  }
	else if(mode==1)	{ ptype = eq_parts_head	;  }
	else if(mode==2)	{ ptype = eq_parts_hands;  }
	else if(mode==3)	{ ptype = eq_parts_legs	;  }
	else if(mode==4)	{ ptype = eq_parts_feet	;  }
	else if(mode==5)	{ ptype = eq_parts_face	;  }

	ZPostChangeParts(ptype,1);

#endif

}
// A����?AI ����AA A�ϥ�좯i�Ƣ� ��y��a��e ��eA|..
void ZGameInterface::TestToggleCharacter()
{
	ZPostChangeCharacter();
}

void ZGameInterface::TestChangeWeapon(ZObjectVMesh* pVMesh)
{
	static int nWeaponIndex = 0;

	int nItemID = 0;
	switch(nWeaponIndex)
	{
	case 0:
		nItemID = 1;		// katana
		break;
	case 1:
		nItemID = 5;		// dagger
		break;
	case 2:
		nItemID = 2;		// double pistol
		break;
	case 3:
		nItemID = 3;		// SMG
		break;
	case 4:
		nItemID = 6;		// shotgun
		break;
	case 5:
		nItemID = 7;		// Rocket
		break;
	case 6:
		nItemID = 4;		// grenade
		break;
	}


	if (GetState() == GUNZ_GAME)
	{
		if (m_pMyCharacter == NULL) return;


		switch(nWeaponIndex)
		{
		case 0:
		case 1:
			m_pMyCharacter->GetItems()->EquipItem(MMCIP_MELEE, nItemID);		// dagger
			m_pMyCharacter->ChangeWeapon(MMCIP_MELEE);
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			m_pMyCharacter->GetItems()->EquipItem(MMCIP_PRIMARY, nItemID);		// Rocket
			m_pMyCharacter->ChangeWeapon(MMCIP_PRIMARY);
			break;
		case 6:
			m_pMyCharacter->GetItems()->EquipItem(MMCIP_CUSTOM1, nItemID);		// grenade
			m_pMyCharacter->ChangeWeapon(MMCIP_CUSTOM1);
			break;
		}

	}
	else if (GetState() == GUNZ_CHARSELECTION)
	{
		if (pVMesh != NULL)
		{
			ZChangeCharWeaponMesh(pVMesh, nItemID);
			pVMesh->SetAnimation("login_intro");
			pVMesh->GetFrameInfo(ani_mode_lower)->m_nFrame = 0;
//			pVMesh->m_nFrame[ani_mode_lower] = 0;
		}
	}
	else if (GetState() == GUNZ_LOBBY)
	{
		if (pVMesh != NULL)
		{
			ZChangeCharWeaponMesh(pVMesh, nItemID);
		}
	}


	nWeaponIndex++;
	if (nWeaponIndex >= 7) nWeaponIndex = 0;
}

/*
bool ZGameInterface::ProcessLowLevelCommand(const char* szCommand)
{
if(_stricmp(szCommand, "FORWARD")==0){
}
else if(_stricmp(szCommand, "BACK")==0){
}
else if(_stricmp(szCommand, "LEFT")==0){
}
else if(_stricmp(szCommand, "RIGHT")==0){
}
return false;
}
*/

void ZGameInterface::RespawnMyCharacter()	// E��AUA����������CO�ҡ� A����?CI��e ��C��i����������U.
{
	if (ZGetGame() == NULL) return;

	m_pMyCharacter->Revival();
	rvector pos=rvector(0,0,0), dir=rvector(0,1,0);

	ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetSoloRandomData();
	if (pSpawnData != NULL)
	{
		pos = pSpawnData->m_Pos;
		dir = pSpawnData->m_Dir;
	}

	m_pMyCharacter->SetPosition(pos);
	m_pMyCharacter->SetDirection(dir);
}

bool ZGameInterface::OnGlobalEvent(MEvent* pEvent)
{			
	//if (ZGetMyInfo()->IsAdminGrade())
	//{
	//	if(pEvent->GetShiftState())
	//	{
	//		switch(pEvent->nKey)
	//		{
	//			case '~':
	//				{
	//					bool bShow = !ZGetConsole()->IsVisible();
	//					ZGetConsole()->Show(bShow);
	//					ZGetConsole()->SetZOrder(MZ_TOP);
	//					ZGetGameClient()->OutputMessage_Sub("%s Console.",bShow ? "Showing" : "Hiding");
	//					return true;
	//				}
	//			default:
	//				break;
	//				//ZGetGameClient()->OutputMessage_Sub("nKey: %c",pEvent->nKey);
	//		}
	//	}
	//}

	if (ZGameInterface::CheckSkipGlobalEvent() == true)
		return true;

	if ((ZGetGameInterface()->GetState() == GUNZ_GAME)) 
		return ZGameInput::OnEvent(pEvent);

	switch(pEvent->nMessage){
		case MWM_CHAR:
		{
			//switch (pEvent->nKey) {
			////case '`' :
			////	if (ZGetMyInfo()->IsAdminGrade() && pEvent->GetShiftState())
			////	{
			////		ZGetConsole()->Show(!ZGetConsole()->IsVisible());
			////		ZGetConsole()->SetZOrder(MZ_TOP);
			////		return true;
			////		//				m_pLogFrame->Show(ZGetConsole()->IsVisible());
			////	}
			////	break;
			//}
			break;
		}

		case MWM_KEYDOWN:
			{
				//// Ctrl+R UI �����O���� ���硤I��u - ����A|�Ƣ� ������U. ��U��� ������E UI �쩣AUAI��e ����A�� ��a��EAI��I��i ������E ������A��c..
				//// (800*600AI��U����������A E����eAI AI��oC��Au, A����a��A �����������碬? ��oA�� ����, ��OAOA������ ���硤I��a AU�좯��E��E)
				//if (pEvent->GetCtrlState() && pEvent->nKey == 'R') {
				//	//GunzState state = ZGetGameInterface()->GetState();
				//	//m_sbRemainClientConnectionForResetApp = true;
				//	//ZLoadingProgress progress("");
				//	//ZGetGameInterface()->OnDestroy();
				//	//ZGetGameInterface()->OnCreate(&progress);
				//	////OnRestore();
				//	//ZGetGameInterface()->SetState(state);
				//	//m_sbRemainClientConnectionForResetApp = false;
				//}
				break;
			}
	}
	return false;
}

bool ZGameInterface::OnDebugEvent(MEvent* pEvent, MListener* pListener)
{
	switch(pEvent->nMessage){
	case MWM_KEYDOWN:
		{
			switch (pEvent->nKey)
			{
			case VK_F10:
				m_pLogFrame->Show(!m_pLogFrame->IsVisible());
				return true;
			case VK_NUMPAD8:
				{
					if (GetState() == GUNZ_CHARSELECTION)
					{
						if (m_pCharacterSelectView != NULL)
						{
//							TestChangeWeapon(m_pCharacterSelectView->GetVisualMesh());
						}
					}
					else if (GetState() == GUNZ_LOBBY)
					{
						if (ZGetCharacterViewList(GUNZ_LOBBY) != NULL)
						{
							ZObjectVMesh* pVMesh = 
								ZGetCharacterViewList(GUNZ_LOBBY)->Get(ZGetGameClient()->GetPlayerUID())->GetVisualMesh();

							TestChangeWeapon(pVMesh);
						}
					}

				}
				break;
			}
		}
		break;
	}
	return false;
}

bool ZGameInterface::OnEvent(MEvent* pEvent, MListener* pListener)
{
#ifndef _PUBLISH
	if (OnDebugEvent(pEvent, pListener)) return true;
#endif

#ifdef _CHURCH
	if (pEvent->nMessage == MWM_KEYDOWN && m_nState == GUNZ_CHARSELECTION)
	{
	//	if (m_pBackground)
	//		m_pBackground->SetForcedChange(true);
	}
#endif

	return false;
}

bool ZGameInterface::OnCommand(MWidget* pWidget, const char* szMessage)
{
	if (pWidget==m_pPlayerMenu) {
		MMenuItem* pItem = (MMenuItem*)pWidget;


		OutputDebugString("PLAYERMENU");
	}
	return false;
}

void ZGameInterface::ChangeParts(int key)
{
	//1����~6����A�ƥ�i��I
	/*
	int mode = 0;
	int type = 0;		// ����A������ ���ϡ�? ��uAI��o��e Aa��nCO id AoA��...

	if(key=='1')	{ mode=0;type = 2;}
	else if(key=='2')	{ mode=1;type = 7;}
	else if(key=='3')	{ mode=2;type =12;}
	else if(key=='4')	{ mode=3;type =17;}
	else if(key=='5')	{ mode=4;type =22;}
	else if(key=='6')	{ mode=5;type =27;}
	else return;

	g_parts[mode] = !g_parts[mode];	// A����?AI A������������I ��A��a��a
	//	g_parts_change = true;

	if(!g_parts[mode])	// ��a�����E
	type = 0;	

	g_pGame->m_pMyCharacter->OnChangeParts(mode,type);

	ZPostChangeParts(mode,type);
	*/
}
/*
void ZGameInterface::UpdateReserveChangeWeapon() {

	if(!m_pMyCharacter) return;

	if(m_bisReserveChangeWeapon) {
		if( m_pMyCharacter->m_fNextShotTime < g_pGame->GetTime()) {
			ChangeWeapon(m_nReserveChangeWeapon);
			m_bisReserveChangeWeapon = false;
		}
	}
}
*/

void ZGameInterface::ChangeWeapon(ZChangeWeaponType nType)
{
	ZMyCharacter* pChar = ZGetGame()->m_pMyCharacter;

	if (pChar->m_pVMesh == NULL) return;	
	if (pChar->IsDie() ) return;						// A����uA����e �����ACN��U.	
	if (m_pGame->GetMatch()->IsRuleGladiator()) return;	// ��U�����쩣����AIAI��e A�碬����a��o
//	if (m_pGame->GetMatch()->IsRuleClassic()) return;

	int nParts = -1;

/*
	if( m_bisReserveChangeWeapon ) { // ������a��E��oAA�������� ��U��U��A��i CI��A�Ʃ���i ������a Ae��O~
		m_bisReserveChangeWeapon = false;
	}

	if( m_pMyCharacter->m_fNextShotTime > g_pGame->GetTime()) { // ����A�� ��U��U��oAO��A��A�ơ�AI ������I ��C��uA����I ������a~

		m_bisReserveChangeWeapon = true;
		m_nReserveChangeWeapon = nType;
		return;
	}
*/
	bool bWheel = false;		// EU��I AU�좯C��A��ҡע�A A�������CAo ��E��A��U
	if ((nType == ZCWT_PREV) || (nType == ZCWT_NEXT))
	{
		bWheel = true;

		int nHasItemCount = 0;
		int nPos = -1;
		int ItemQueue[MMCIP_END];

		if (m_pGame->GetMatch()->isSpecialTeamsGame())
		{
			for (int i = MMCIP_MELEE; i < MMCIP_CUSTOM3; ++i)
			{
				if (!IsWeaponCharItemParts((MMatchCharItemParts)i))
					continue;

				if (!pChar->GetItems()->GetItem((MMatchCharItemParts)i)->IsEmpty())
				{
					if (pChar->GetItems()->GetSelectedWeaponParts() == i)
						nPos = nHasItemCount;

					if (ZGetConfiguration()->GetEtc()->bDisableSW && (i == MMCIP_CUSTOM1 || i == MMCIP_CUSTOM2 ||
						i == MMCIP_CUSTOM3)) continue;

					ItemQueue[nHasItemCount++] = i;
				}
			}
		}
		else
		{
			for (int i = MMCIP_MELEE; i < MMCIP_CUSTOM2 + 1; i++)
			{
				if (!pChar->GetItems()->GetItem((MMatchCharItemParts)i)->IsEmpty())
				{
					if (pChar->GetItems()->GetSelectedWeaponParts() == i)
						nPos = nHasItemCount;

					if (ZGetConfiguration()->GetEtc()->bDisableSW && (i == MMCIP_CUSTOM1 || i == MMCIP_CUSTOM2)) continue;

					ItemQueue[nHasItemCount++] = i;
				}
			}
		}
		if (nPos < 0) return;

		if (nType == ZCWT_PREV)
		{
			if (nHasItemCount <= 1) return;

			int nNewPos = nPos - 1;
			int nCount = 0;
			if (nNewPos < 0) nNewPos = nHasItemCount-1;
			while ( nNewPos != nPos)
			{
				MMatchCharItemParts nPart = (MMatchCharItemParts)ItemQueue[ nNewPos];
				if ( (nPart == MMCIP_CUSTOM1) || (nPart == MMCIP_CUSTOM2) ||(nPart == MMCIP_CUSTOM3)) {
					ZItem* pItem = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(nPart);
					if (pItem && pItem->GetBulletCurrMagazine() > 0) { break; }
				} else { break; }

				nNewPos = nNewPos - 1;
				if (nNewPos < 0) nNewPos = nHasItemCount-1;

				nCount++;
				if ( nCount > (nHasItemCount * 2))
					return;
			}
			nPos = nNewPos;

		}
		else if (nType == ZCWT_NEXT)
		{
			if (nHasItemCount <= 1) return;

			int nNewPos = nPos + 1;
			if (nNewPos >= nHasItemCount) nNewPos = 0;
			while(nNewPos != nPos) 
			{
				MMatchCharItemParts nPart = (MMatchCharItemParts)ItemQueue[nNewPos];
				if ( (nPart == MMCIP_CUSTOM1) || (nPart == MMCIP_CUSTOM2) || (nPart == MMCIP_CUSTOM3) ) {
					ZItem* pItem = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(nPart);
					if (pItem && pItem->GetBulletCurrMagazine() > 0) {	break; }
				} else { break; }

				nNewPos = nNewPos + 1;
				if (nNewPos >= nHasItemCount) nNewPos = 0;
			}
			nPos = nNewPos;
		}

		nParts = ItemQueue[nPos];
	}
	else
	{

		switch (nType)
		{
		case ZCWT_MELEE:	nParts = (int)(MMCIP_MELEE);	break;
		case ZCWT_PRIMARY:	nParts = (int)(MMCIP_PRIMARY);	break;
		case ZCWT_SECONDARY:nParts = (int)(MMCIP_SECONDARY); break;
		case ZCWT_CUSTOM1:	nParts = (int)(MMCIP_CUSTOM1);	break;
		case ZCWT_CUSTOM2:	nParts = (int)(MMCIP_CUSTOM2);	break;
		case ZCWT_CUSTOM3: nParts = (int)(MMCIP_CUSTOM3); break;
		default: _ASSERT(0); break;
		}

		// ��U���� ��o��uA��A����I ChangeWeapon ��YAo
		if ((nParts == MMCIP_CUSTOM1) || (nParts == MMCIP_CUSTOM2) || (nParts == MMCIP_CUSTOM3))
		{
			ZItem* pItem = pChar->GetItems()->GetItem((MMatchCharItemParts)nParts);
			if (pItem->GetBulletCurrMagazine() <= 0) return;
		}
	}

	if (nParts < 0) return;
	if (pChar->GetItems()->GetSelectedWeaponParts() == nParts) return;

	// EU��I ��������ҡע�A Ae��OCIAo ��E��A��U
	if(bWheel && (pChar->GetStateUpper() == ZC_STATE_UPPER_LOAD && pChar->IsUpperPlayDone() == false ))
		return;
	
	//jintriple3 ������������ CA��I��A...��n���� ����A��..
	const ZCharaterStatusBitPacking &uStatus = pChar->m_dwStatusBitPackingValue.Ref();
	ZMyCharaterStatusBitPacking & zStatus = pChar->m_statusFlags.Ref();

	// ��A������AAOA��ҡ�/��o�ҡ�/��y��i����/����A��/����A��ҡ�/��c��oA����oAA��A ���졾a����E?AI ��E��E��U.
	if (zStatus.m_bWallHang || zStatus.m_bShot || zStatus.m_bShotReturn || uStatus.m_bTumble 
		|| zStatus.m_bSkill || zStatus.m_bGuard || uStatus.m_bBlast || 	uStatus.m_bBlastFall 
		|| uStatus.m_bBlastDrop || 	uStatus.m_bBlastStand || uStatus.m_bBlastAirmove
		|| zStatus.m_bSlash || zStatus.m_bJumpSlash || zStatus.m_bJumpSlashLanding	// �ơ����̡�a A��������� ����E? ��E��E��U
		 
		// ��|A��AI�ҡ� ��oA����O��I����AI��CA����I ��C����
		|| (pChar->GetStateUpper() == ZC_STATE_UPPER_SHOT && pChar->IsUpperPlayDone() == false )
		
		/*
		// ���������i ������cCI��i�������� ��o����AI�Ƣ� ����A�� ��E��������A��ҡ�
		||  pChar->m_fNextShotTimeType[pChar->GetItems()->GetSelectedWeaponParts()] > g_pGame->GetTime() 
		*/
		
		)
		{
			// AI�ҡע�A ������aA�� CN��U.	(EU����CO)
			m_bReservedWeapon = true;
			m_ReservedWeapon  = nType;
			return;
		}

	/*
	// ��oA����O��I����AI��CAI AOA����e A�����.. ���硤I��a/��| ��uA��
	if (pChar->GetStateUpper() != ZC_STATE_UPPER_NONE) {
		if ( (pChar->GetStateUpper() != ZC_STATE_UPPER_RELOAD) &&
			(pChar->IsUpperPlayDone() == false) ) return;
	}
	*/


	m_bReservedWeapon = false;
	ZPostChangeWeapon(nParts);

	m_pMyCharacter->m_dwStatusBitPackingValue.Ref().m_bSpMotion = false;
	// ���� ���졾a��A A��A�� ��U��U��U. command��| AeCI��e ���������Ƣ� ��U��i��A �Ʃ���i�Ƣ� AO��U.
	m_pMyCharacter->ChangeWeapon((MMatchCharItemParts)nParts);
}

/*
void ZGameInterface::ChangeWeapon(int key)
{
//7����8����A�ƥ�i��I
int mode = 0;

if(key=='7') mode = 0;
else if(key=='8') mode = 1;
else if(key=='9') mode = 2;
else if(key=='0') mode = 3;
else return;

//	g_select_weapon = mode;
//	g_weapon_change = true;

// -1 remove ..

g_pGame->m_pMyCharacter->OnChangeWeapon(mode);

ZPostChangeWeapon(mode);
}
*/

void ZGameInterface::OnGameUpdate(float fElapsed)
{
	__BP(12,"ZGameInterface::OnGameUpdate");
	if(m_pGame==NULL) return;
	if (m_pGameInput) m_pGameInput->Update(fElapsed);

//	UpdateReserveChangeWeapon();

	m_pGame->Update(fElapsed);

	if (m_pCombatInterface) m_pCombatInterface->Update(fElapsed);

	if(m_bReservedWeapon)
		ChangeWeapon(m_ReservedWeapon);

	__EP(12);
}


extern bool g_bTestFromReplay;

void ZGameInterface::OnReplay()
{
	ShowWidget( "ReplayConfirm", false);

	CreateReplayGame(NULL);
}

bool ZGameInterface::Update(float fElapsed)
{
	// ������a��E A����a��| E��AICN��U.
	if ( m_bReservedQuit) {
		if ( timeGetTime() > m_dwReservedQuitTimer) {
			ZGetApplication()->Exit();
		}
	}

	if ( m_pBackground && ( ( GetState() == GUNZ_CHARSELECTION) || ( GetState() == GUNZ_CHARCREATION)) )
		m_pBackground->OnUpdate( fElapsed);


	// ����AI���� ��c��i��a �������AI����
	OnVoiceSound();


	// A���������硤I ����CA����AI��A ��U��A A��A����IAI ��AAUCI��a A��C�� AI������O ������e��u��U.
	if (GetState() == GUNZ_LOBBY)
	{
		if (g_bTestFromReplay == true) 
		{
			ShowWidget( "Lobby", false);
			ShowWidget( "ReplayConfirm", true);
			return false;
		}
	}

	__BP(13,"ZGameInterface::Update");

	ZGetOptionInterface()->Update();

	GetShopEquipInterface()->Update();

	__BP(14,"ZGameInterface::GameClient Run & update");
	if (m_spGameClient != NULL) m_spGameClient->Run();
	m_spGameClient->Tick();
	__EP(14);

 	if(!m_bLoading) {
		if(GetState()==GUNZ_GAME){
			OnGameUpdate(fElapsed);
		}
		else{
		}
	}

	if(GetState()==GUNZ_LOBBY && m_bWaitingArrangedGame) {
        // AO��A��I
		MLabel *pLabel = (MLabel*)m_IDLResource.FindWidget("LobbyWaitingArrangedGameLabel");
		if(pLabel) {
			int nCount = (timeGetTime()/500)%5;
			char dots[10];
			for(int i=0;i<nCount;i++) {
				dots[i]='.';
			}
			dots[nCount]=0;

			char szBuffer[256];
			sprintf(szBuffer,"%s%s", ZMsg( MSG_WORD_FINDTEAM), dots);
			pLabel->SetText(szBuffer);
		}
	}

	// ��I��n�Ƣ� ������N��� Aa��E��O���� Au�Ƣ�A��AI �ҡ�AOA����e A�˨�O ������U���� ��I����AI E����eA����I ��������ơ� �Ʃ���i ��iAI��C��I Au�Ƣ�A��A�� ��Y����A����U
	MWidget* pDTWaitMatchDlg = m_IDLResource.FindWidget("DuelTournamentWaitMatchDialog");
	if(pDTWaitMatchDlg && pDTWaitMatchDlg->IsVisible() && GetState()!=GUNZ_LOBBY )
		OnDuelTournamentGameUI(false);

	UpdateDuelTournamentWaitMsgDots();

	UpdateCursorEnable();

	// ��CA| ��OAO�������� �����Ƣ���A ��a��CA�� ��I���͢�U
	if(ZGetGame()!=NULL && m_bLeaveBattleReserved && (m_dwLeaveBattleTime < timeGetTime()))
		LeaveBattle();

	__EP(13);

	return true;
}

void ZGameInterface::OnResetCursor()
{
	SetCursorEnable(m_bCursor);
}

void ZGameInterface::SetCursorEnable(bool bEnable)
{
	//	_RPT1(_CRT_WARN,"cursor %d\n",bEnable);
	
	if(m_bCursor==bEnable) return;

	m_bCursor = bEnable;
	MCursorSystem::Show(bEnable);

	/*
	if(!bEnable)
	{
	ShowCursor(FALSE);
	}else
	ShowCursor(TRUE);
	*/

/*	if(m_pCursorSurface)	// RAONHAJE Mouse Cursor HardwareDraw
	{
		RGetDevice()->SetCursorProperties(0,0,m_pCursorSurface);
		RGetDevice()->ShowCursor(bEnable);
	}	*/
}

void ZGameInterface::UpdateCursorEnable()
{
	
	if( GetState()!=GUNZ_GAME ||
		(GetCombatInterface() && GetCombatInterface()->IsShowResult()) ||
		IsMenuVisible() || 
		m_pMsgBox->IsVisible() ||
		m_pConfirmMsgBox->IsVisible() )
		SetCursorEnable(true);
	else
	{
		MWidget* pWidget = m_IDLResource.FindWidget( "112Confirm");
		if ( pWidget && pWidget->IsVisible())
			SetCursorEnable( true);
		else
			SetCursorEnable( false);
		pWidget = m_IDLResource.FindWidget("BlitzClassSelect");
		if (pWidget && pWidget->IsVisible())
			SetCursorEnable(true);
		else
			SetCursorEnable(false);
	}
}

void ZGameInterface::UpdateDuelTournamentWaitMsgDots()
{
	// '��a��oAa��E��O���� Au�Ƣ�A��' ��UAI��o��I���� ������AAoAC ... ����AI��| ��A�ơ̢��� ��u��o A��A��CN��U
	if(GetState()!=GUNZ_LOBBY) return;
    
	MLabel *pLabel = (MLabel*)m_IDLResource.FindWidget("DuelTournamentWaitMatchLabel");
	if(pLabel == NULL || !pLabel->IsVisible()) return;

	const char* szText = pLabel->GetText();
	size_t len = strlen(szText);
	if (len == 0) return;

	int nCount = (timeGetTime()/500)%5 + 1;
	char dots[10];
	for(int i=0;i<nCount;i++) {
		dots[i]='.';
	}
	dots[nCount]=0;

	// ��aA������ ��U��iAO���� ...A�� A|��UCN �������� ����AU����AC ����AI��| ��E����������U
	int msgEndPosExceptDots = 0;
	for (size_t pos=len-1; pos>=0; --pos) {
		if (szText[pos] != '.') {
			msgEndPosExceptDots = (int)pos;
			break;
		}
	}

	// �������� ����AU�������� ��o ...A�� ��UAI��U
	char szBuffer[256];
	strncpy(szBuffer, szText, msgEndPosExceptDots+1);
	szBuffer[msgEndPosExceptDots+1] = 0;
	strcat(szBuffer, dots);
	pLabel->SetText(szBuffer);
}

void ZGameInterface::SetMapThumbnail(const char* szMapName)
{
	SAFE_DELETE(m_pMapThumbnail);

	char szThumbnail[256];
	sprintf(szThumbnail, "maps/%s/%s.rs.bmp", szMapName,szMapName);

	m_pMapThumbnail=Mint::GetInstance()->OpenBitmap(szThumbnail);
	if(!m_pMapThumbnail)
	{
		sprintf(szThumbnail, "maps/%s/%s.bmp", szMapName,szMapName);
		m_pMapThumbnail=Mint::GetInstance()->OpenBitmap(szThumbnail);
	}
}

void ZGameInterface::ClearMapThumbnail()
{
	SAFE_DELETE(m_pMapThumbnail);
}


void ZGameInterface::Reload()
{
	if(!ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeapon()) return;
	MMatchItemDesc* pSelectedItemDesc = ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeapon()->GetDesc();

	if (pSelectedItemDesc == NULL) return;

	if (pSelectedItemDesc->m_nType.Ref() != MMIT_RANGE)  return;

	if(ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeapon()->isReloadable()==false) return;

	ZMyCharacter* pChar = ZGetGame()->m_pMyCharacter;

	//jintriple3 ������������ CA��I��A...��n���� ����A��..
	ZCharaterStatusBitPacking &uStatus = pChar->m_dwStatusBitPackingValue.Ref();

	// ��c��iA�� ��oAA����������A AaAu����CO..
	if(uStatus.m_bBlast || 
		uStatus.m_bBlastFall || 
		uStatus.m_bBlastDrop || 
		uStatus.m_bBlastStand || 
		uStatus.m_bBlastAirmove )
		return;

	if( pChar->GetStateUpper()==ZC_STATE_UPPER_RELOAD) return ;

	uStatus.m_bSpMotion = false;

	ZPostReload();
}

#include <thread>

void ZGameInterface::SaveScreenShot()
{
	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();
#define SCREENSHOT_DELAY		2000

	// 2AE ��o����AI
	if ((nNowTime-st_nLastTime) < SCREENSHOT_DELAY)	return;
	st_nLastTime = nNowTime;


	char szPath[_MAX_PATH];
	char szFilename[_MAX_PATH];
	char szFilenameSafe[_MAX_PATH];

	TCHAR szMyDocPath[MAX_PATH];
	if(GetMyDocumentsPath(szMyDocPath)) {
		strcpy(szPath,szMyDocPath);
		strcat(szPath,GUNZ_FOLDER);
		CreatePath( szPath );
		strcat(szPath,SCREENSHOT_FOLDER);
		CreatePath( szPath );
		strcat(szPath, "/");
	}

	// CoAc ��OAO A��������I ��AAI��iA�� ��������
	SYSTEMTIME t;
	GetLocalTime( &t );
	char szCharName[MATCHOBJECT_NAME_LENGTH];
	ValidateFilename(szCharName, ZGetMyInfo()->GetCharName(), '_');

	sprintf(szFilename,"%s_%4d%02d%02d_%02d%02d%02d",  
		szCharName, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	sprintf(szFilenameSafe,"nocharname_%4d%02d%02d_%02d%02d%02d",  
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	LPBYTE data = NULL;
	LPDIRECT3DSURFACE9 frontbuffer = NULL;

	//if(nsscount<1000)
	{

		HRESULT hr;

		D3DDISPLAYMODE d3ddm;
		RGetDevice()->GetDisplayMode(0, &d3ddm);

		hr = RGetDevice()->CreateOffscreenPlainSurface(d3ddm.Width, d3ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &frontbuffer, NULL);
		hr = RGetDevice()->GetFrontBufferData(0, frontbuffer);

		_ASSERT(hr == D3D_OK);

		RECT rt;
		::GetClientRect(g_hWnd, &rt);

		D3DLOCKED_RECT LRECT;
		if (frontbuffer->LockRect(&LRECT, &rt, NULL) == D3D_OK)
		{
			int nWidth = rt.right - rt.left;
			int nHeight = rt.bottom - rt.top;

			int pitch = LRECT.Pitch;
			LPBYTE source = (LPBYTE)LRECT.pBits;

			data = new BYTE[nWidth*nHeight * 4];
			for (int i = 0; i<nHeight; i++)
			{
				memcpy(data + i*nWidth * 4, source + pitch*i, nWidth * 4);
			}

			char szFullPath[_MAX_PATH];	// E��AaAU��A ��E��e��iAOA��
			char szFullPathToNotify[_MAX_PATH];

			sprintf(szFullPath, "%s%s", szPath, szFilename);
			sprintf(szFullPathToNotify, GUNZ_FOLDER SCREENSHOT_FOLDER"/""%s.jpg", szFilename);
			bool bSuccess = RScreenShot(nWidth, nHeight, data, szFullPath);

			if (!bSuccess) {	// A����?��i�ҡש��碯�� ��C����C��A�� ��o AOA����I A����?��iA�� ��y����CN ��AAI��iA����I Ac��A���

				sprintf(szFullPath, "%s%s", szPath, szFilenameSafe);
				sprintf(szFullPathToNotify, GUNZ_FOLDER SCREENSHOT_FOLDER"/""%s.jpg", szFilenameSafe);
				bSuccess = RScreenShot(nWidth, nHeight, data, szFullPath);

				if (!bSuccess) goto SCREENSHOTERROR;
			}

			char szOutput[_MAX_PATH * 2];
			ZTransMsg(szOutput, MSG_SCREENSHOT_SAVED, 1, szFullPathToNotify);
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);

			//SAFE_DELETE(data); ��iAIAi
			delete[] data;
		}
		SAFE_RELEASE(frontbuffer);
	}
	return;

SCREENSHOTERROR:
	//SAFE_DELETE(data);
	delete[] data;
	SAFE_RELEASE(frontbuffer);

	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM),
		ZMsg(MSG_SCREENSHOT_CANT_SAVE));

	return;

}

void ZGameInterface::ShowMessage(const char* szText, MListener* pCustomListenter, int nMessageID)
{
	if (pCustomListenter)
		m_pMsgBox->SetCustomListener(pCustomListenter);

	char text[1024] ="";

	// nMessageID�Ƣ� 0AI ������I��e ��������Ao ��U���� ��������Ao ����E�̥�� CO���� Aa��AC��A����U.(��U���� ������o ����AI�ҡ� E��AICI��a A��CO)
	if (nMessageID != 0)
	{
		sprintf(text, "%s (M%d)", szText, nMessageID);
	}
	else
	{
		strcpy(text, szText);
	}

	m_pMsgBox->SetText(text);
	m_pMsgBox->Show(true, true);
}

void ZGameInterface::ShowConfirmMessage(const char* szText, MListener* pCustomListenter)
{
	if (pCustomListenter)
		m_pConfirmMsgBox->SetCustomListener(pCustomListenter);

	m_pConfirmMsgBox->SetText(szText);
	m_pConfirmMsgBox->Show(true, true);
}

void ZGameInterface::ShowMessage(int nMessageID)
{
	const char *str = ZMsg( nMessageID );
	if(str)
	{
		char text[1024];
		sprintf(text, "%s (M%d)", str, nMessageID);
		ShowMessage(text);
	}
}

void ZGameInterface::ShowErrorMessage(int nErrorID)
{
	const char *str = ZErrStr( nErrorID );
	if(str)
	{
		char text[1024];
		sprintf(text, "%s (E%d)", str, nErrorID);
		ShowMessage(text);
	}
}

void ZGameInterface::ShowErrorMessage(const char* szErrorMsg, int nErrorID)
{
	char szErrorID[64];
	sprintf(szErrorID, " (E%d)", nErrorID);

	char text[1024];
	strcpy(text, szErrorMsg);
	strcat(text, szErrorID);
	
	ShowMessage(text);
}

void ZGameInterface::ChangeSelectedChar(int nNum)
{
	bool bRequested = false;
	// ������a A����?AI A��������| ��E������O��uA����e ������o���� ������o��i ��aA��CN��U.
	if ((!ZCharacterSelectView::m_CharInfo[nNum].m_bLoaded) && (!ZCharacterSelectView::m_CharInfo[nNum].m_bRequested))
	{
		ZPostAccountCharInfo(nNum);
		GetCharacterSelectView()->UpdateInterface(nNum);
		//ZCharacterSelectView::SetSelectedCharacter(nNum);
		ZCharacterSelectView::m_CharInfo[nNum].m_bRequested = true;
		bRequested = true;
	}


	// A����?AI ����AI��a
	if ((!bRequested) && (GetCharacterSelectView() != NULL))
	{
		GetCharacterSelectView()->SelectChar(nNum);
	}



}

void ZGameInterface::OnCharSelectionCreate(void)
{
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

	EnableCharSelectionInterface(true);

	if (m_pCharacterSelectView!=NULL) SAFE_DELETE(m_pCharacterSelectView);
	m_pCharacterSelectView = new ZCharacterSelectView();
	m_pCharacterSelectView->SetBackground(m_pBackground);
	m_pCharacterSelectView->SelectChar(ZCharacterSelectView::GetSelectedCharacter());
#ifdef _CHURCH
	if (m_nPreviousState == GUNZ_LOBBY)
	{
		//m_pBackground->SetScene(GUNZ_LO)
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
		m_pBackground->ReverseCam();
		m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
	}
	else {

		m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);

	}
#endif
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MWidget* pWidget = pResource->FindWidget( "CS_SelectCharDefKey");
	if ( pWidget)
		pWidget->Enable( true);

	pWidget = pResource->FindWidget( "CharSel_GameRoomUser");
	if ( pWidget)
	{
		if ( ZGetMyInfo()->GetPGradeID() == MMPG_PREMIUM_IP)
			pWidget->Show( true);
		else
			pWidget->Show( false);
	}

#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportDisplaySelCharacter();
#endif
}

void ZGameInterface::OnCharSelect(void)
{
	if( m_pCharacterSelectView->SelectMyCharacter() == false )
		return;

	EnableCharSelectionInterface( false);

#ifndef _CHURCH
	if ( m_pBackground)
		m_pBackground->SetScene( LOGIN_SCENE_SELECTCHAR);
#else
		m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
#endif



#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportSelectCharacter();
#endif
}

void ZGameInterface::OnCharSelectionDestroy(void)
{
	ShowWidget("CharSelection", false);
	if (m_pCharacterSelectView!=NULL) SAFE_DELETE(m_pCharacterSelectView);
}

void ZGameInterface::OnCharCreationCreate(void)
{
	ShowWidget("CharSelection", false);
	ShowWidget("CharCreation", true);

	if (m_pCharacterSelectView!=NULL) SAFE_DELETE(m_pCharacterSelectView);
	m_pCharacterSelectView = new ZCharacterSelectView();
	m_pCharacterSelectView->SetBackground(m_pBackground);
	m_pCharacterSelectView->SetState(ZCharacterSelectView::ZCSVS_CREATE);
	m_pCharacterSelectView->OnChangedCharCostume();

#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportDisplayCreateCharacter();
#endif
}

void ZGameInterface::OnCharCreationDestroy(void)
{
	ShowWidget("CharCreation", false);
	ShowWidget("CharSelection", true);

	if (m_pCharacterSelectView!=NULL) SAFE_DELETE(m_pCharacterSelectView);
}

void ZGameInterface::ChangeToCharSelection(void)
{
	ZCharacterSelectView::ClearCharInfo();
	ZPostAccountCharList(ZGetMyInfo()->GetSystemInfo()->pbyGuidAckMsg);		// update sgk 0409 // A����?AI ����������� ��aA��
}

void ZGameInterface::OnInvalidate()
{
	ZGetWorldManager()->OnInvalidate();
	if(m_pGame)
		m_pGame->OnInvalidate();
	if(m_pBackground)
		m_pBackground->OnInvalidate();
	if( m_pCharacterSelectView)
		m_pCharacterSelectView->OnInvalidate();
	//ZCharacterViewList* pCharViewList = (ZCharacterViewList*)m_IDLResource.FindWidget("StagePlayerList");
	//for(list<ZMeshView*>::iterator iter = ZMeshView::msMeshViewList.begin(); iter != ZMeshView::msMeshViewList.end(); ++iter )
	//{
	//	ZMeshView* pmv = *iter;
	//	if( pmv )
	//		pmv->OnInvalidate();
	//}

	if(ZGetEffectManager())
		ZGetEffectManager()->OnInvalidate();	

	ZCharacterView* pCharView = (ZCharacterView*)m_IDLResource.FindWidget("Stage_Charviewer");
	if(pCharView!= 0) pCharView->OnInvalidate();
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget("EquipmentInformation");
	if(pCharView!= 0) pCharView->OnInvalidate();
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget("EquipmentInformationShop");
	if(pCharView!= 0) pCharView->OnInvalidate();
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget("EquipmentInformationLobby");
	if(pCharView!= 0) pCharView->OnInvalidate();

	if (m_pCombatInterface)
		m_pCombatInterface->OnInvalidate();
}

void ZGameInterface::OnRestore()
{
	ZGetWorldManager()->OnRestore();
	if(m_pGame)
		m_pGame->OnRestore();
	if(m_pBackground)
		m_pBackground->OnRestore();
//	ResetCursor();	
	if( m_pCharacterSelectView)
		m_pCharacterSelectView->OnRestore();
	//ZCharacterViewList* pCharViewList = (ZCharacterViewList*)m_IDLResource.FindWidget("StagePlayerList");
	//for(list<ZMeshView*>::iterator iter = ZMeshView::msMeshViewList.begin(); iter != ZMeshView::msMeshViewList.end(); ++iter )
	//{
	//	ZMeshView* pmv = *iter;
	//	if( pmv )
	//		pmv->OnRestore();
	//}

	if(ZGetEffectManager())
		ZGetEffectManager()->OnRestore();	

	ZCharacterView* pCharView = (ZCharacterView*)m_IDLResource.FindWidget("Stage_Charviewer");
	if(pCharView!= 0) pCharView->OnRestore();
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget("EquipmentInformation");
	if(pCharView!= 0) pCharView->OnRestore();
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget("EquipmentInformationShop");
	if(pCharView!= 0) pCharView->OnRestore();
	pCharView = (ZCharacterView*)m_IDLResource.FindWidget("EquipmentInformationLobby");
	if(pCharView!= 0) pCharView->OnRestore();

	if (m_pCombatInterface)
		m_pCombatInterface->OnRestore();
}


void ZGameInterface::UpdateBlueRedTeam( void)
{
	MButton* pBlueTeamBtn  = (MButton*)m_IDLResource.FindWidget("StageTeamBlue");
	MButton* pBlueTeamBtn2 = (MButton*)m_IDLResource.FindWidget("StageTeamBlue2");
	MButton* pRedTeamBtn  = (MButton*)m_IDLResource.FindWidget("StageTeamRed");
	MButton* pRedTeamBtn2 = (MButton*)m_IDLResource.FindWidget("StageTeamRed2");
	if ((pRedTeamBtn == NULL) || (pBlueTeamBtn == NULL) || (pRedTeamBtn2 == NULL) || (pBlueTeamBtn2 == NULL))
		return;

	if (GetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_INFECTION ||
		GetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_SPYMODE)
	{
		pRedTeamBtn->Show(false);
		pRedTeamBtn2->Show(false);
		pBlueTeamBtn->Show(false);
		pBlueTeamBtn2->Show(false);
		return;
	}

	// ��ACA(��AAu)
	if ( m_bTeamPlay)
	{
		pRedTeamBtn->Show(true);
		pRedTeamBtn2->Show(true);
		pBlueTeamBtn->Show(true);
		pBlueTeamBtn2->Show(true);

		ZPlayerListBox* pListBox = (ZPlayerListBox*)m_IDLResource.FindWidget("StagePlayerList_");

		if(pListBox)
		{
			int nR=0,nB=0;

			// A��A���������� ��yAU��| ������A��U. AAAo��E��U.
			for( int i = 0;  i < pListBox->GetCount(); i++)
			{
				ZStagePlayerListItem *pSItem = (ZStagePlayerListItem*)pListBox->Get(i);
				if(pSItem->m_nTeam == 1)		nR++;
				else if(pSItem->m_nTeam == 2)	nB++;
			}

			char buffer[64];
			ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
			ZBmNumLabel *pNumLabel;

//			sprintf(buffer,"%d",nB);
			sprintf(buffer,"%s:%d", ZMsg( MSG_WORD_BLUETEAM), nB);
			pNumLabel = (ZBmNumLabel*)pRes->FindWidget("StageNumOfBlueTeam");
			if ( pNumLabel)
			{
				pNumLabel->SetText(buffer);
				pNumLabel->Show( true);
			}
			MButton* pButton = (MButton*)pRes->FindWidget( "StageTeamBlue");
			if ( pButton)
				pButton->SetText( buffer);

//			sprintf(buffer,"%d",nR);
			sprintf(buffer,"%s:%d", ZMsg( MSG_WORD_REDTEAM), nR);
			pNumLabel = (ZBmNumLabel*)pRes->FindWidget("StageNumOfRedTeam");
			if ( pNumLabel)
			{
				pNumLabel->SetIndexOffset(16);
				pNumLabel->SetText(buffer);
				pNumLabel->Show( true);
			}
			pButton = (MButton*)pRes->FindWidget( "StageTeamRed");
			if ( pButton)
				pButton->SetText( buffer);

			sprintf( buffer, "%s : %d", ZMsg( MSG_WORD_PLAYERS), nB+nR);
			MLabel* pLabel = (MLabel*)pRes->FindWidget( "Stage_NumOfPerson");
			if ( pLabel)
				pLabel->SetText( buffer);
		}
	}
	else
	{
		pRedTeamBtn->Show(false);
		pRedTeamBtn2->Show(false);
		pBlueTeamBtn->Show(false);
		pBlueTeamBtn2->Show(false);

		ZPlayerListBox* pListBox = (ZPlayerListBox*)m_IDLResource.FindWidget("StagePlayerList_");

		if(pListBox)
		{
			int nPlayerNum=0;

			// A��A���������� ��yAU��| ������A��U. AAAo��E��U.
			for( int i = 0;  i < pListBox->GetCount(); i++)
			{
				ZStagePlayerListItem *pSItem = (ZStagePlayerListItem*)pListBox->Get(i);
				if( pSItem->m_nTeam == 0)
					nPlayerNum++;
			}

			ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
			ZBmNumLabel *pNumLabel;

			char buffer[64];

			pNumLabel = (ZBmNumLabel*)pRes->FindWidget("StageNumOfBlueTeam");
			if ( pNumLabel)
				pNumLabel->Show( false);

			pNumLabel = (ZBmNumLabel*)pRes->FindWidget("StageNumOfRedTeam");
			if ( pNumLabel)
				pNumLabel->Show( false);

			sprintf( buffer, "%s : %d", ZMsg( MSG_WORD_PLAYERS), nPlayerNum);
			MLabel* pLabel = (MLabel*)pRes->FindWidget( "Stage_NumOfPerson");
			if ( pLabel)
				pLabel->SetText( buffer);
		}
	}
}

void ZGameInterface::ShowInterface(bool bShowInterface)
{
	m_bShowInterface = bShowInterface;

	if (m_nState != GUNZ_GAME)
	{
		SetCursorEnable(bShowInterface);
	}

	// Login
	if (m_nState == GUNZ_LOGIN)
	{
		ShowWidget("Login", m_bShowInterface);
	}
	else if (m_nState == GUNZ_CHARSELECTION)
	{
		ShowWidget("CharSelection", m_bShowInterface);
	}
	else if (m_nState == GUNZ_GAME)
	{
		bool bConsole=ZGetConsole()->IsVisible();
		bool bLogFrame=m_pLogFrame->IsVisible();

		//		m_pCombatInterface->Show(m_bShowInterface);
		m_pLogFrame->Show(m_bShowInterface);
		ZGetConsole()->Show(m_bShowInterface);
		ShowWidget("CombatInfo1",m_bShowInterface);
		ShowWidget("CombatInfo2",m_bShowInterface);
		ShowWidget("Time", m_bShowInterface);
		ZGetConsole()->Show(bConsole);
		m_pLogFrame->Show(bLogFrame);
	}
}

/*
void ZGameInterface::OnMoveMouse(int iDeltaX,int iDeltaY)
{
	bool bRotateEnable=false;
	if(!m_pMyCharacter->m_bWallJump && !m_pMyCharacter->m_bWallJump2 && !m_pMyCharacter->m_bWallHang && 
		!m_pMyCharacter->m_bTumble && !m_pMyCharacter->m_bBlast && !m_pMyCharacter->m_bBlastStand && !m_pMyCharacter->m_bBlastDrop )
		bRotateEnable=true;


	if (RIsActive())
	{
		static float lastanglex,lastanglez;
		if(bRotateEnable)		{
			float fRotateStep = 0.0005f * Z_MOUSE_SENSITIVITY*10.0f;
			m_Camera.m_fAngleX += (iDeltaY * fRotateStep);
			m_Camera.m_fAngleZ += (iDeltaX * fRotateStep);

			m_Camera.m_fAngleX=min(CAMERA_ANGLEX_MAX,
								max(CAMERA_ANGLEX_MIN,m_Camera.m_fAngleX));

			lastanglex=m_Camera.m_fAngleX;
			lastanglez=m_Camera.m_fAngleZ;
		}else
		{
			// �ơ˥��A|CNAI CE��aCI��U

			float fRotateStep = 0.0005f * Z_MOUSE_SENSITIVITY*10.0f;
			m_Camera.m_fAngleX += (iDeltaY * fRotateStep);
			m_Camera.m_fAngleZ += (iDeltaX * fRotateStep);

			m_Camera.m_fAngleX=min(CAMERA_ANGLEX_MAX,max(CAMERA_ANGLEX_MIN,
								min(lastanglex+pi/4.f,max(lastanglex-pi/4.f,m_Camera.m_fAngleX))));

			_ASSERT(m_Camera.m_fAngleX>=CAMERA_ANGLEX_MIN && m_Camera.m_fAngleX<=CAMERA_ANGLEX_MAX );

			m_Camera.m_fAngleZ=min(lastanglez+pi/4.f,
								max(lastanglez-pi/4.f,m_Camera.m_fAngleZ));



		}
	}
}
*/

void ZGameInterface::OnResponseShopItemList( const vector< MTD_ShopItemInfo*> &vShopItemList , const vector<MTD_GambleItemNode*>& vGItemList )
{
	ZGetShop()->SetItemsAll( vShopItemList );
	ZGetShop()->SetItemsGamble( vGItemList );
	ZGetShop()->Serialize();
}

void ZGameInterface::OnResponseCharacterItemList(MUID* puidEquipItem
												 , MTD_ItemNode* pItemNodes
												 , int nItemCount
												 , MTD_GambleItemNode* pGItemNodes
												 , int nGItemCount )
{
	ZGetMyInfo()->GetItemList()->SetItemsAll(pItemNodes, nItemCount);
	ZGetMyInfo()->GetItemList()->SetGambleItemAll( pGItemNodes, nGItemCount );
	ZGetMyInfo()->GetItemList()->SetEquipItemsAll(puidEquipItem);
	ZGetMyInfo()->GetItemList()->Serialize();

	// ��oA���������� A����?AI UI Text A��������| �������AI���� C��A����U (��������, ��U��i����, A����A, ������i, �����O)
	GetShopEquipInterface()->DrawCharInfoText();
}



void ZGameInterface::FinishGame()
{
	ZGetGame()->StopRecording();
	ZGetGameInterface()->GetCombatInterface()->Finish();
	m_bGameFinishLeaveBattle = true;
}

void ZGameInterface::SerializeStageInterface()
{
	// Au�������� ������a...  ��OCI��A AU��aAIAo �������̡�UA��...  -_-;
	// ����A������ CE��a������O ��C��e AI CO��o AUA����| ��eA|CI��i ��U��I OnStageInterfaceSettupA����I ��N��i�Ƣ���O...
/*
#ifdef _QUEST
	ZStageSetting::InitStageSettingGameType();

	if ( ZGetGameClient()->GetServerMode() == MSM_CLAN)
	{
		// ����A������ ���̢�y��E ����CoCO�ҡס�iAo ������E ��E CI�������� ��c��e
		MComboBox* pCBMapSelection = (MComboBox*)m_IDLResource.FindWidget( "MapSelection");
		if ( pCBMapSelection)
		{
			int nSelected = pCBMapSelection->GetSelIndex();
			pCBMapSelection->RemoveAll();

			InitMaps( pCBMapSelection);

			if ( nSelected >= pCBMapSelection->GetCount())
				nSelected = pCBMapSelection->GetCount() - 1;

			pCBMapSelection->SetSelIndex( nSelected);
			pCBMapSelection->SetText( "asdfasdfa");
		}
	}
#endif
*/
	MComboBox* pCombo = (MComboBox*)m_IDLResource.FindWidget( "MapSelection");
	if ( pCombo)
		InitMaps( pCombo);

	ZApplication::GetStageInterface()->OnStageInterfaceSettup();

}


void ZGameInterface::HideAllWidgets()
{
	ShowWidget("Login", false);
	ShowWidget("Lobby", false);
	ShowWidget("Stage", false);
	ShowWidget("Game", false);
	ShowWidget("Option", false);
	ShowWidget("CharSelection", false);
	ShowWidget("CharCreation", false);
	ShowWidget("Shop", false);
//	ShowWidget("LobbyChannelPlayerList", false);

	// dialog
	ShowWidget("StageSettingFrame", false);
	ShowWidget("BuyConfirm", false);
	ShowWidget("Equipment", false);
	ShowWidget("StageCreateFrame", false);
	ShowWidget("PrivateStageJoinFrame", false);
}

bool SetWidgetToolTipText(char* szWidget,const char* szToolTipText, MAlignmentMode mam) {

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if(pResource==NULL)		return false;
	if(!szToolTipText)		return false;


	MWidget* pWidget = pResource->FindWidget(szWidget);

	if(pWidget) {

		
		if(!szToolTipText[0]) {
			pWidget->DetachToolTip();
		}
		else {
//			pWidget->AttachToolTip(szToolTipText);
			pWidget->AttachToolTip(new ZToolTip(szToolTipText, pWidget, mam));
		}
/*
		MToolTip* pTT =	pWidget->GetToolTip();
		if(pTT) {
			if(!szToolTipText[0]) {
				pTT->AttachToolTip()
			}
			else {

			}
//			pTT->SetText(szToolTipText);
			return true;
		}
*/
	}
	return false;
}

// #define CheckLine(str) 
// AO��e 200 A����IAa��i -14 ��iAo A��A�� ��UAU�Ƣ� A������Ao ��E��졤I..

 // zmaplistbox.cpp ������� ����������C��iAO��U..

bool GetItemDescName(string& str,DWORD nItemID)
{
	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

	if ( pItemDesc == NULL)
	{
		str.clear();
		return false;
	}

	str = (string)pItemDesc->m_pMItemName->Ref().m_szItemName;
	return false;
}

bool GetItemDescStr(string& str,DWORD nItemID) {

	static char temp[1024];

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

	if(pItemDesc==NULL) {
		str.clear();
//		str += " ";
		return false;
	}

	bool bAdd = false;
	bool bLine = false;
	int	 nLine = 0;
	int  nLen = 0;

	if(pItemDesc->m_pMItemName->Ref().m_szItemName) {
		str += pItemDesc->m_pMItemName->Ref().m_szItemName;
		bAdd = true;
	}

	if(pItemDesc->m_nResLevel.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"Res Level : %d",pItemDesc->m_nResLevel.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nWeight.Ref()){
		if(bAdd) str += " / ";
		sprintf(temp,"+Weight : %d",pItemDesc->m_nWeight.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nMaxBullet.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"Max Bullets : %d",pItemDesc->m_nMaxBullet.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	
	if(pItemDesc->m_nMagazine.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+Magazine : %d",pItemDesc->m_nMagazine.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nDamage.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+Damage : %d",pItemDesc->m_nDamage.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nDelay.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+Delay : %d",pItemDesc->m_nDelay.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nReloadTime.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+Reload Time : %d",pItemDesc->m_nReloadTime.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nHP.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+HP : %d",pItemDesc->m_nHP.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nAP.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+AP : %d",pItemDesc->m_nAP.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	if(pItemDesc->m_nMaxWT.Ref()) {
		if(bAdd) str += " / ";
		sprintf(temp,"+MaxWT : %d",pItemDesc->m_nMaxWT.Ref());

		nLen = (int)strlen(temp);
		if((int)str.size() + nLen > (nLine+1) * MAX_TOOLTIP_LINE_STRING+3) { str += "\n"; nLine++; }

		str += temp;
		bAdd = true;
	}

	return true;
}

void ZGameInterface::ShowEquipmentDialog(bool bShow)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if (bShow)
	{	// ����A��AIAo ��E�������� Aa��n ��o���� ����AA
		ShowWidget( "Lobby", false);
		ShowWidget( "Stage", false);
		ShowWidget( "Shop",  false);

		MButton* pButton = (MButton*)pResource->FindWidget("Equipment_to_Shop");
		MLabel* pLabel = (MLabel*)pResource->FindWidget("Equip_Message");
		if ( pButton && pLabel)
		{
			char buf[ 256];
			if ( ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
			{
				pButton->Show( true);
				sprintf( buf, "%s > %s > %s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_STAGE), ZMsg( MSG_WORD_EQUIPMENT));
				pLabel->SetText( buf);
			}
			else
			{
				pButton->Show( true);
				sprintf( buf, "%s > %s > %s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_LOBBY), ZMsg( MSG_WORD_EQUIPMENT));
				pLabel->SetText( buf);
			}
		}

		MWidget* pWidget = pResource->FindWidget("Equipment");
		if(pWidget!=NULL) pWidget->Show(true, true);

		BEGIN_WIDGETLIST( "EquipmentInformation", pResource, ZCharacterView*, pCharacterView);
		ZMyInfo* pmi = ZGetMyInfo();
		ZMyItemList* pil = ZGetMyInfo()->GetItemList();

		unsigned long int nEquipedItemID[MMCIP_END];

		for (int i = 0; i < MMCIP_END; i++)
		{
			nEquipedItemID[i] = pil->GetEquipedItemID(MMatchCharItemParts(i));
		}
		pCharacterView->InitCharParts(pmi->GetSex(), pmi->GetHair(), pmi->GetFace(), nEquipedItemID);
		END_WIDGETLIST();

		GetShopEquipInterface()->SelectEquipmentTab(0);

//		ZPostRequestGetCharQuestItemInfo( ZGetGameClient()->GetPlayerUID());
		ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());
		ZPostStageState( ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MOSS_EQUIPMENT);

		// Animation sprite
		MPicture* pPicture = 0;
		pPicture = (MPicture*)pResource->FindWidget("Equip_StripBottom");
 		if( pPicture != NULL)
			pPicture->SetAnimation( 0, 1000.0f);
		pPicture = (MPicture*)pResource->FindWidget("Equip_StripTop");
		if( pPicture != NULL)
			pPicture->SetAnimation( 1, 1000.0f);

		pWidget = pResource->FindWidget("Equip");
		if ( pWidget != NULL)
			pWidget->Enable( false);
		pWidget = pResource->FindWidget("SendAccountItemBtn");
		if ( pWidget != NULL)
			pWidget->Enable( false);
		pWidget = pResource->FindWidget("BringAccountItemBtn");
		if ( pWidget != NULL)
			pWidget->Enable( false);
		pWidget = pResource->FindWidget("BringAccountSpendableItemConfirmOpen");
		if ( pWidget != NULL)
			pWidget->Enable( false);

		MTextArea* pTextArea = (MTextArea*)pResource->FindWidget("Equip_ItemDescription");
		if(pTextArea)	pTextArea->SetText("");

		/*pTextArea = (MTextArea*)pResource->FindWidget("Equip_ItemDescription3");
		if ( pTextArea)
		{
			pTextArea->SetTextColor( MCOLOR( 180, 180, 180));
			pTextArea->SetText( ZMsg( MSG_SHOPMSG));
		}*/

		/*pPicture = (MPicture*)pResource->FindWidget("Equip_ItemIcon");
		pPicture->SetBitmap( NULL);*/
		GetShopEquipInterface()->DrawCharInfoText();
	}
	else
	{	// ����A��AIAo ��E Aa��n��a��i�������� �����Ƣ���a ��o���� ����AA
		ZPostStageState( ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MOSS_NONREADY);

		MWidget* pWidget = pResource->FindWidget("Equipment");
		if ( pWidget!=NULL)
			pWidget->Show(false);

		if ( ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
			ShowWidget( "Stage", true);
		else
			ShowWidget( "Lobby", true);
		GetShopEquipInterface()->DrawCharInfoText();

		// ½æ³×ÀÏ ÀÌ¹ÌÁö ¾ð·ÎµåÇØ¼­ ¸Þ¸ð¸® È®º¸
		GetItemThumbnailMgr()->UnloadTextureTemporarily();
	}
}

void ZGameInterface::ShowShopDialog(bool bShow)
{
	if (bShow)
	{	// ��oA�� ����AA
		ShowWidget( "Lobby", false);
		ShowWidget( "Stage", false);
		ShowWidget( "Equipment", false);

		ZGetShop()->Create();
		ZGetShop()->Serialize();

		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

		MPicture* pPicture = 0;
		pPicture = (MPicture*)pResource->FindWidget("Shop_StripBottom");
 		if( pPicture != NULL)
			pPicture->SetAnimation( 0, 1000.0f);
		pPicture = (MPicture*)pResource->FindWidget("Shop_StripTop");
		if( pPicture != NULL)
			pPicture->SetAnimation( 1, 1000.0f);


		MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Shop_ItemDescription");
		if(pTextArea)	pTextArea->SetText("");

		//pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Shop_ItemIcon");
		//pPicture->SetBitmap( NULL);
		MButton* pButton = (MButton*)pResource->FindWidget("Shop_to_Equipment");
		MLabel* pLabel = (MLabel*)pResource->FindWidget("Shop_Message");
		if ( pButton && pLabel)
		{
		char buf[256];
		if ( ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
			{
		pButton->Show( true);
		sprintf( buf, "%s > %s > %s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_STAGE), ZMsg( MSG_WORD_SHOP));
		pLabel->SetText( buf);
			}
			else
			{
				pButton->Show( true);
				sprintf( buf, "%s > %s > %s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_LOBBY), ZMsg( MSG_WORD_SHOP));
				pLabel->SetText( buf);
			}
		}


		MWidget* pWidget = pResource->FindWidget("Shop");
		if(pWidget!=NULL)
			pWidget->Show(true, true);
		pWidget = pResource->FindWidget("BuyConfirmCaller");
		if (pWidget != NULL)
			pWidget->Enable(false);
		pWidget = pResource->FindWidget("SellConfirmCaller");
		if (pWidget != NULL)
			pWidget->Enable(false);
		pWidget = pResource->FindWidget("BuyCashConfirmCaller");
		if (pWidget != NULL)
			pWidget->Enable(false);

		GetShopEquipInterface()->SelectShopTab(0);

//		ZPostRequestGetCharQuestItemInfo( ZGetGameClient()->GetPlayerUID());
		ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());

		GetShopEquipInterface()->DrawCharInfoText();
	}
	else
	{	// Aa��n ����AA
		ZPostStageState(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MOSS_NONREADY);

		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("Shop");
		if(pWidget!=NULL) pWidget->Show(false);

		if ( ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
			ShowWidget( "Stage", true);
		else
			ShowWidget( "Lobby", true);

		GetShopEquipInterface()->DrawCharInfoText();

		// ��������AI AI��IAo ������I��aC������ ������������ E�稬��
		GetItemThumbnailMgr()->UnloadTextureTemporarily();
	}
}








void ZGameInterface::EnableCharSelectionInterface(bool bEnable)
{
	MWidget* pWidget;

	pWidget = m_IDLResource.FindWidget("CS_SelectChar");
	if (pWidget) pWidget->Enable(bEnable);
	pWidget = m_IDLResource.FindWidget("CS_DeleteChar");
	if (pWidget) pWidget->Enable(bEnable);
	pWidget = m_IDLResource.FindWidget("CS_CreateChar");
	if (pWidget) pWidget->Enable(bEnable);
	pWidget = m_IDLResource.FindWidget("CS_Prev");
	if (pWidget) pWidget->Enable(bEnable);
	pWidget = m_IDLResource.FindWidget("CS_Name_Pre");
	if (pWidget) pWidget->Enable(bEnable);
	pWidget = m_IDLResource.FindWidget("CS_Name_Next");
	if (pWidget) pWidget->Enable(bEnable);
	pWidget = m_IDLResource.FindWidget("CS_Name");
	if (pWidget) pWidget->Enable(bEnable);

	if ( !bEnable)
	{
		for ( int i = 0;  i < MAX_CHAR_COUNT;  i++)
		{
			char szName[ 256];
			sprintf( szName, "CharSel_SelectBtn%d", i);
			pWidget = m_IDLResource.FindWidget( szName);
			if ( pWidget)
				pWidget->Show( false);
		}
	}

	if ( bEnable && (ZCharacterSelectView::GetNumOfCharacter() == 0))
	{
		pWidget = m_IDLResource.FindWidget( "CS_SelectChar");
		if (pWidget) pWidget->Enable( false);
		pWidget = m_IDLResource.FindWidget( "CS_DeleteChar");
		if (pWidget) pWidget->Enable( false);
	}

	if ( bEnable && (ZCharacterSelectView::GetNumOfCharacter() >= MAX_CHAR_COUNT))
	{
		pWidget = m_IDLResource.FindWidget( "CS_CreateChar");
		if (pWidget) pWidget->Enable( false);
	}
}

void ZGameInterface::EnableLobbyInterface(bool bEnable)
{

	EnableWidget("LobbyOptionFrame", bEnable);			// ��E��C ��o����
	EnableWidget("Lobby_Charviewer_info", bEnable);		// A������������a ��o����
	EnableWidget("StageJoin", bEnable);					// ��OAOAu�Ƣ� ��o����
	EnableWidget("StageCreateFrameCaller", bEnable);	// ��OAO��y���� ��o����
	EnableWidget("ShopCaller", bEnable);				// ��oA�� ��o����
	EnableWidget("EquipmentCaller", bEnable);			// Aa��n ��o����
	EnableWidget("ReplayCaller", bEnable);				// ����CA����AI ��o����
	EnableWidget("CharSelectionCaller", bEnable);		// A����?AI ��?�Ʃ� ��o����
	EnableWidget("Logout", bEnable);					// ��I����������o ��o����
	EnableWidget("QuickJoin", bEnable);					// AuA��AI ��o����
	EnableWidget("QuickJoin2", bEnable);				// AuA��AI ��o����
	EnableWidget("ChannelListFrameCaller", bEnable);	// A����I��?�Ʃ� ��o����
	EnableWidget("StageList", bEnable);					// ���������������
	EnableWidget("Lobby_StageList",bEnable);
	EnableWidget("LobbyChannelPlayerList", bEnable);
	EnableWidget("ChannelChattingOutput", bEnable);
	EnableWidget("ChannelChattingInput", bEnable);
	EnableWidget("AchievementCaller", bEnable);

	if (bEnable)
	{
		MMatchServerMode nCurrentServerMode = ZGetGameClient()->GetServerMode();
		MCHANNEL_TYPE nCurrentChannelType = ZGetGameClient()->GetChannelType();
		bool bClanBattleUI = (nCurrentServerMode== MSM_CLAN) && (nCurrentChannelType==MCHANNEL_TYPE_CLAN);
		ZGetGameInterface()->InitClanLobbyUI(bClanBattleUI);
	}

	// ����A��AIAo�Ƣ� �������碬e ��OAO ������A��u��A A��A�� ������A(��OAO��AAU��o���� ����CO)
	ZApplication::GetStageInterface()->SetEnableWidgetByRelayMap(!bEnable);
}

void ZGameInterface::EnableStageInterface(bool bEnable)
{
	EnableWidget("Stage_Charviewer_info", bEnable);		// A������������a ��o����
	EnableWidget("StagePlayerNameInput_combo", bEnable);
	EnableWidget("GameStart", bEnable);					// ��OAO��AAU ��o����
	MButton* pButton = (MButton*)m_IDLResource.FindWidget("StageReady");				// �����쩣 ��o����
	if ( pButton)
	{
		pButton->Enable( bEnable);
		pButton->SetCheck( false);
	}
	EnableWidget("ForcedEntryToGame", bEnable);			// ����AO ��o����
	EnableWidget("ForcedEntryToGame2", bEnable);		// ����AO ��o����
	EnableWidget("StageTeamBlue",  bEnable);			// blue��A ����A�� ��o����
	EnableWidget("StageTeamBlue2", bEnable);			// blue��A ����A�� ��o����
	EnableWidget("StageTeamRed",  bEnable);				// red��A ����A�� ��o����
	EnableWidget("StageTeamRed2", bEnable);				// red��A ����A�� ��o����
	EnableWidget("ShopCaller", bEnable);				// ��oA�� ��o����
	EnableWidget("EquipmentCaller", bEnable);			// Aa��n ��o����
	EnableWidget("StageSettingCaller", bEnable);		// ��������A�� ��o����
	EnableWidget("StageObserverBtn", bEnable);			// ��uAu A��A�� ��o����
	EnableWidget("Lobby_StageExit", bEnable);			// �����Ƣ���a ��o����

	EnableWidget("MapSelection", bEnable);				// ��E����AA A��������U����

	if( ZGetGameClient()->AmIStageMaster() == true )
		EnableWidget("StageType", true);					// ��OAO������A A��������U����
	else
		EnableWidget("StageType", false);					// ��OAO������A A��������U����

	EnableWidget("StageMaxPlayer", bEnable);			// AO��eAI���� A��������U����
	EnableWidget("StageRoundCount", bEnable);			// �Ʃ���aE����o A��������U����

	EnableWidget("StagePlayerList_", bEnable);
}

void ZGameInterface::SetRoomNoLight( int d )
{
	/*
	MTabCtrl *pTab = (MTabCtrl*)m_IDLResource.FindWidget("Lobby_RoomNoControl");
	if( pTab ) 
		pTab->SetSelIndex(d-1);
	*/
    
	/*
	for(int i=1;i<=6;i++)
	{
		char szBuffer[64];
		sprintf(szBuffer, "Lobby_RoomNo%d", i);
		MButton* pButton = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szBuffer);
		if(pButton)
		{
			bool bCheck = (i==d);
			pButton->SetCheck(bCheck);
		}
	}
	*/

	char szBuffer[64];
	sprintf(szBuffer, "Lobby_RoomNo%d", d);
	MButton* pButton = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szBuffer);
	if(pButton) 
		pButton->SetCheck(true);

}

void ZGameInterface::ShowPrivateStageJoinFrame(const char* szStageName)
{
	MEdit* pStageNameEdit = (MEdit*)m_IDLResource.FindWidget("PrivateStageName");
	if (pStageNameEdit)
	{
		pStageNameEdit->SetText(szStageName);
		pStageNameEdit->Enable(false);
	}
	MEdit* pPassEdit = (MEdit*)m_IDLResource.FindWidget("PrivateStagePassword");
	if (pPassEdit!=NULL)
	{
		pPassEdit->SetMaxLength(STAGEPASSWD_LENGTH);
		pPassEdit->SetText("");
		pPassEdit->SetFocus();
	}

	MWidget* pPrivateStageJoinFrame = m_IDLResource.FindWidget("PrivateStageJoinFrame");
	if (pPrivateStageJoinFrame)
	{
		pPrivateStageJoinFrame->Show(true, true);
	}
}

void ZGameInterface::LeaveBattle()
{
	ShowMenu(false);
	m_bGameFinishLeaveBattle = false;

	ZPostStageLeaveBattle(ZGetGameClient()->GetPlayerUID(), m_bGameFinishLeaveBattle);//, ZGetGameClient()->GetStageUID());
	if (m_bLeaveStageReserved) {
		ZPostStageLeave(ZGetGameClient()->GetPlayerUID());//, ZGetGameClient()->GetStageUID());
		ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);
	} else {
		ZApplication::GetGameInterface()->SetState(GUNZ_STAGE);
	}

	ZGetGameInterface()->SetCursorEnable(true);
	m_bLeaveBattleReserved = false;
	m_bLeaveStageReserved = false;
}

void ZGameInterface::ReserveLeaveStage()
{
	m_bLeaveStageReserved = true;
	ReserveLeaveBattle();
}

void ZGameInterface::ReserveLeaveBattle()
{
	bool bLeaveImmediately = false;
#ifndef _PUBLISH
	bLeaveImmediately = true;
#endif

	if(!m_pGame) return;

	// ��EAu��oAI�ҡש���..
	if (m_pCombatInterface->GetObserver()->IsVisible()) 
		bLeaveImmediately = true;

	// ��o����������yA����I AO����A������..... 20090616 by kammir
	// A��AU����AU ����A��AIAo��I �����ơ�EA ��U��I ��e������I ��e��i��A�Ʃ���i(6.5AE��E���� AcCo��E�Ʃ���i)
	// ������o�������� A����?AI��| ������uC��AOAo ��E��A��U....������IA����I AIC�� ��o���� ������y
	// A����i���� ������u��C��a��| ��a��U�����ҡע�A
	//ZMatch* pMatch = ZGetGame()->GetMatch();
	//int nRemainTime = pMatch->GetRemainedSpawnTime();
	//if ((nRemainTime > 0) && (nRemainTime <= 5))
	//	bLeaveImmediately = true;

	// ��U��I ��OAO�������� �����ơ̢�U
	if(bLeaveImmediately) 
	{
		LeaveBattle();
		return;
	}

	m_bLeaveBattleReserved = true;
	m_dwLeaveBattleTime = timeGetTime() + 5000;
}

void ZGameInterface::ShowMenu(bool bEnable)
{
	if(!GetCombatInterface()->IsShowResult())
		m_CombatMenu.ShowModal(bEnable);
	ZGetGameInterface()->SetCursorEnable(bEnable);
}

bool ZGameInterface::IsMenuVisible()
{
	return m_CombatMenu.IsVisible();
}

void ZGameInterface::Show112Dialog(bool bShow)
{
/*
	MWidget* pWidget = m_IDLResource.FindWidget("112Confirm");
	if(pWidget==NULL) return;

	if (pWidget->IsVisible() == bShow) return;
	pWidget->Show(bShow);
	
	if (bShow)
	{
		SetCursorEnable(true);

		MEdit* pReasonEdit = (MEdit*)m_IDLResource.FindWidget("112_ConfirmEdit");
		if (pReasonEdit)
		{
			pReasonEdit->SetText("");
			pReasonEdit->SetFocus();
		}
	}
	else
	{
		if (GetState() == GUNZ_GAME)
		{
			if (!IsMenuVisible()) SetCursorEnable(false);
		}
	}
*/
	MWidget* pWidget = m_IDLResource.FindWidget( "112Confirm");
	if ( !pWidget)
		return;

	if ( pWidget->IsVisible() == bShow)
		return;

	pWidget->Show( bShow, true);
	pWidget->SetFocus();

	if ( !bShow)
		return;


	MComboBox* pCombo1 = (MComboBox*)m_IDLResource.FindWidget( "112_ConfirmID");
	MComboBox* pCombo2 = (MComboBox*)m_IDLResource.FindWidget( "112_ConfirmReason");

	if ( !pCombo1 || !pCombo2)
		return;

	pCombo1->RemoveAll();
	pCombo2->SetSelIndex( 0);


	switch ( m_nState)
	{
		case GUNZ_LOBBY:
		{
			ZPlayerListBox *pPlayerListBox = (ZPlayerListBox*)m_IDLResource.FindWidget( "LobbyChannelPlayerList");
			if ( pPlayerListBox)
			{
				for ( int i = 0;  i < pPlayerListBox->GetCount();  i++)
					pCombo1->Add( pPlayerListBox->GetPlayerName( i));
			}
		}
		break;

		case GUNZ_STAGE:
		{
			ZPlayerListBox *pPlayerListBox = (ZPlayerListBox*)m_IDLResource.FindWidget( "StagePlayerList_");
			if ( pPlayerListBox)
			{
				for ( int i = 0;  i < pPlayerListBox->GetCount();  i++)
					pCombo1->Add( pPlayerListBox->GetPlayerName( i));
			}
		}
		break;

		case GUNZ_GAME:
		{
			for ( ZCharacterManager::iterator itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); itor++)
			{
				ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
				pCombo1->Add(pCharacter->GetCharName());
			}
		}
		break;
	}

	pCombo1->SetSelIndex( 0);
}


void ZGameInterface::RequestQuickJoin()
{
	MTD_QuickJoinParam	quick_join_param;

	quick_join_param.nMapEnum = 0xFFFFFFFF;

	// ���硤��AI����, A��AuA�� ��좥U.
	quick_join_param.nModeEnum = 0;
	SetBitSet(quick_join_param.nModeEnum, MMATCH_GAMETYPE_DEATHMATCH_SOLO);
	SetBitSet(quick_join_param.nModeEnum, MMATCH_GAMETYPE_DEATHMATCH_TEAM);
	/*SetBitSet(quick_join_param.nModeEnum, MMATCH_GAMETYPE_CLASSIC_SOLO);
	SetBitSet(quick_join_param.nModeEnum, MMATCH_GAMETYPE_CLASSIC_TEAM);*/
	SetBitSet(quick_join_param.nModeEnum, MMATCH_GAMETYPE_ASSASSINATE);
	SetBitSet(quick_join_param.nModeEnum, MMATCH_GAMETYPE_GUNGAME);




	ZPostRequestQuickJoin(ZGetGameClient()->GetPlayerUID(), &quick_join_param);
}

/*
// 0 = A����I , 1 = A�̡��� , 2 = A������
void ZGameInterface::SetupPlayerListButton(int index)
{
	if(index<0 || index>=3) return;

	ZIDLResource *pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MWidget *pWidget;

	pWidget = pResource->FindWidget("LobbyPlayerListTabChannel");
	if(pWidget) pWidget->Show(index==0);

	pWidget = pResource->FindWidget("LobbyPlayerListTabFriend");
	if(pWidget) pWidget->Show(index==1);

	pWidget = pResource->FindWidget("LobbyPlayerListTabClan");
	if(pWidget) pWidget->Show(index==2);

	// A������AI��� A�����̢��� �Ƣ�AOAI ��E��C��iAOA����e ��y���� A��A�� ����AI��U
	bool bShowCreateFrame = (index==2 && !ZGetMyInfo()->IsClanJoined());

	pWidget = m_IDLResource.FindWidget("LobbyPlayerListClanCreateFrame");
	if(pWidget) pWidget->Show(bShowCreateFrame);
	pWidget = m_IDLResource.FindWidget("LobbyChannelPlayerList");
	if(pWidget) pWidget->Show(!bShowCreateFrame);
}

void ZGameInterface::SetupPlayerListTab()
{
	ZIDLResource *pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget *pWidget;
	pWidget = pResource->FindWidget("LobbyPlayerListTabClan");
	if(!pWidget || !pWidget->IsVisible()) return;

	// A������AI��� A�����̢��� �Ƣ�AOAI ��E��C��iAOA����e ��y���� A��A�� ����AI��U
	bool bShowCreateFrame = !ZGetMyInfo()->IsClanJoined();

	pWidget = m_IDLResource.FindWidget("LobbyPlayerListClanCreateFrame");
	if(pWidget) pWidget->Show(bShowCreateFrame);
	pWidget = m_IDLResource.FindWidget("LobbyChannelPlayerList");
	if(pWidget) pWidget->Show(!bShowCreateFrame);
}
*/

void ZGameInterface::InitClanLobbyUI(bool bClanBattleEnable)
{
	OnArrangedTeamGameUI(false);

	MWidget *pWidget;

	pWidget= m_IDLResource.FindWidget( "StageJoin" );
	if(pWidget) pWidget->Show(!bClanBattleEnable);
	pWidget= m_IDLResource.FindWidget( "StageCreateFrameCaller" );
	if(pWidget) pWidget->Show(!bClanBattleEnable);

	pWidget= m_IDLResource.FindWidget( "ArrangedTeamGame" );
	if(pWidget) pWidget->Show(bClanBattleEnable);

	m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, !bClanBattleEnable);
//	pWidget= m_IDLResource.FindWidget( "BattleExit" );
//	if(pWidget) pWidget->Enable(!bClanBattleEnable);

	pWidget= m_IDLResource.FindWidget( "QuickJoin" );
	if(pWidget) pWidget->Show(!bClanBattleEnable);

	pWidget= m_IDLResource.FindWidget( "QuickJoin2" );
	if(pWidget) pWidget->Show(!bClanBattleEnable);

	bool bClanServer = ZGetGameClient()->GetServerMode()==MSM_CLAN;

	pWidget= m_IDLResource.FindWidget( "PrivateChannelInput" );
	if(pWidget) pWidget->Show(bClanServer);
	
	pWidget= m_IDLResource.FindWidget( "PrivateChannelEnter" );
	if(pWidget) pWidget->Show(bClanServer);

	pWidget= m_IDLResource.FindWidget( "Lobby_ClanInfoBG" );
	if(pWidget) pWidget->Show(bClanBattleEnable);

	pWidget= m_IDLResource.FindWidget( "Lobby_ClanList" );
	if(pWidget) pWidget->Show(bClanBattleEnable);
}

void ZGameInterface::InitDuelTournamentLobbyUI(bool bEnableDuelTournamentUI)
{
	MWidget *pWidget;

	pWidget= m_IDLResource.FindWidget( "Lobby_RoomListBG" );
	if(pWidget) {
		if (!bEnableDuelTournamentUI) ((MPicture*)pWidget)->SetBitmap( m_pRoomListFrame);
		else ((MPicture*)pWidget)->SetBitmap( m_pDuelTournamentLobbyFrame);
	}

	pWidget= m_IDLResource.FindWidget( "StageBeforeBtn");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "StageAfterBtn");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_RoomNo1");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_RoomNo2");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_RoomNo3");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_RoomNo4");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_RoomNo5");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_RoomNo6");
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);

	pWidget= m_IDLResource.FindWidget( "StageJoin" );
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "StageCreateFrameCaller" );
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "QuickJoin" );
	if(pWidget) pWidget->Show(!bEnableDuelTournamentUI);

	pWidget= m_IDLResource.FindWidget( "DuelTournamentGame" );
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);

#ifdef _PUBLISH
	pWidget= m_IDLResource.FindWidget( "DuelTournamentGame_2Test" );
	if(pWidget) pWidget->Show(false);
	pWidget= m_IDLResource.FindWidget( "DuelTournamentGame_4Test" );
	if(pWidget) pWidget->Show(false);
#else	// A���������碯e 2,4AI ��a��oAa��E��O���� Au�Ƣ� ��o����
	pWidget= m_IDLResource.FindWidget( "DuelTournamentGame_2Test" );
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "DuelTournamentGame_4Test" );
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);
#endif

	pWidget= m_IDLResource.FindWidget( "Lobby_DuelTournamentInfoBG" );
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);
	pWidget= m_IDLResource.FindWidget( "Lobby_DuelTournamentRankingList" );
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);

	if (bEnableDuelTournamentUI) {
		pWidget = m_IDLResource.FindWidget("Lobby_DuelTournamentInfoName");
		if(pWidget) pWidget->SetText(ZGetMyInfo()->GetCharName());
	}
	
	pWidget= m_IDLResource.FindWidget( "Lobby_DuelTournamentRankingListLabel" );
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);

	pWidget = m_IDLResource.FindWidget("Lobby_DuelTournamentNeedPointNextRank");
	if(pWidget) pWidget->Show(bEnableDuelTournamentUI);

	//Aa��AA���������碯e
/*	if (bEnableDuelTournamentUI)
	{
		pWidget= m_IDLResource.FindWidget( "Lobby_DuelTournamentRankingList" );
		ZDuelTournamentRankingListBox* pRankingList = (ZDuelTournamentRankingListBox*)pWidget;

		ZDUELTOURNAMENTRANKINGITEM tempItem;
		tempItem.bEmptyItem = false;
		tempItem.nFluctuation = 1;
		tempItem.nLosses = 30;
		tempItem.nWins = 193;
		tempItem.nWinners = 42;
		tempItem.nPoint = 1832;
		tempItem.nRank = 35;
		strcpy(tempItem.szCharName, "��������AI");
		pRankingList->SetRankInfo(0, tempItem);

		tempItem.nFluctuation = -2;
		tempItem.nLosses = 25;
		tempItem.nWins = 43;
		tempItem.nWinners = 22;
		tempItem.nPoint = 1532;
		tempItem.nRank = 36;
		strcpy(tempItem.szCharName, "��e�좯Co��C");
		pRankingList->SetRankInfo(1, tempItem);

		tempItem.nFluctuation = 534528;
		tempItem.nLosses = 823458;
		tempItem.nWins = 823458;
		tempItem.nWinners = 820098;
		tempItem.nPoint = 8518;
		tempItem.nRank = 823528;
		strcpy(tempItem.szCharName, "A|AI��aAI����AO");
		pRankingList->SetRankInfo(2, tempItem);

		tempItem.nFluctuation = -1;
		tempItem.nLosses = 34;
		tempItem.nWins = 120;
		tempItem.nWinners = 30;
		tempItem.nPoint = 1300;
		tempItem.nRank = 38;
		strcpy(tempItem.szCharName, "^1����^2����^3��?");
		pRankingList->SetRankInfo(3, tempItem);

		tempItem.nFluctuation = 0;
		tempItem.nLosses = 22;
		tempItem.nWins = 110;
		tempItem.nWinners = 30;
		tempItem.nPoint = 980;
		tempItem.nRank = 39;
		strcpy(tempItem.szCharName, "iiiiiiiiiiii");
		pRankingList->SetRankInfo(4, tempItem);

		ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();

		pWidget = pRes->FindWidget("Lobby_DuelTournamentNeedPointNextRank");
		if(pWidget) { 
			if (bEnableDuelTournamentUI) {
				char szBuffer[256] = "";
				ZTransMsg(szBuffer, MSG_LOBBY_DUELTOURNAMENT_NEEDPOINT_TONEXTRANK, 1, "32");
				pWidget->SetText(szBuffer);
				((MLabel*)pWidget)->SetAlignment(MAM_RIGHT);
			}
		}		
		//
		//char szOutput[256];
		//sprintf(szOutput,"%d/%d",823458,823458);
		//ZBmNumLabel *pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoWinLose");
		//pNumLabel->SetText(szOutput);
		//sprintf(szOutput,"%d",8518);
		//pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoPoints");
		//pNumLabel->SetText(szOutput);
		//sprintf(szOutput,"%d",820098);
		//pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoWinners");
		//pNumLabel->SetText(szOutput);
		//sprintf(szOutput,"%d",823528);
		//pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoRanking");
		//pNumLabel->SetText(szOutput);
	}
*/
}

void ZGameInterface::InitChannelFrame(MCHANNEL_TYPE nChannelType)
{
	MWidget* pWidget;

	pWidget = m_IDLResource.FindWidget("PrivateChannelInput");
	if(pWidget) pWidget->Show( nChannelType == MCHANNEL_TYPE_USER );
	pWidget = m_IDLResource.FindWidget("PrivateChannelEnter");
	if(pWidget) pWidget->Show( nChannelType == MCHANNEL_TYPE_USER );
	pWidget = m_IDLResource.FindWidget("MyClanChannel");
	if(pWidget) pWidget->Show( nChannelType == MCHANNEL_TYPE_CLAN );

	MListBox* pListBox = (MListBox*)m_IDLResource.FindWidget("ChannelList");
	if (pListBox) pListBox->RemoveAll();
}

void ZGameInterface::InitLadderUI(bool bLadderEnable)
{
	OnArrangedTeamGameUI(false);

	MWidget *pWidget;

	pWidget= m_IDLResource.FindWidget( "StageJoin" );
	if(pWidget) pWidget->Show(!bLadderEnable);
	pWidget= m_IDLResource.FindWidget( "StageCreateFrameCaller" );
	if(pWidget) pWidget->Show(!bLadderEnable);

	pWidget= m_IDLResource.FindWidget( "ArrangedTeamGame" );
	if(pWidget) pWidget->Show(bLadderEnable);

	m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, !bLadderEnable);
	//pWidget= m_IDLResource.FindWidget( "BattleExit" );
	//if(pWidget) pWidget->Enable(!bLadderEnable);

	bool bLadderServer = 
		ZGetGameClient()->GetServerMode()==MSM_CLAN ||
		ZGetGameClient()->GetServerMode()==MSM_LADDER ||
		ZGetGameClient()->GetServerMode()==MSM_EVENT;

	pWidget= m_IDLResource.FindWidget( "PrivateChannelInput" );
	if(pWidget) pWidget->Show(bLadderServer);
	
	pWidget= m_IDLResource.FindWidget( "PrivateChannelEnter" );
	if(pWidget) pWidget->Show(bLadderServer);

}

void ZGameInterface::OnArrangedTeamGameUI(bool bFinding)
{
	MWidget *pWidget;

	pWidget= m_IDLResource.FindWidget( "ArrangedTeamGame" );
	if(pWidget) pWidget->Show(!bFinding);

	pWidget = m_IDLResource.FindWidget("LobbyFindClanTeam");
	if(pWidget!=NULL) pWidget->Show(bFinding);


	// ��UA�� A��A����eA�� enable/disable C��A����U
#define SAFE_ENABLE(x,b) { pWidget= m_IDLResource.FindWidget( x ); if(pWidget) pWidget->Enable(!b); }

	SAFE_ENABLE("LobbyChannelPlayerList", bFinding);
//	SAFE_ENABLE("LobbyPlayerListTabClanCreateButton");
	SAFE_ENABLE("ShopCaller", bFinding);
	SAFE_ENABLE("EquipmentCaller", bFinding);
	SAFE_ENABLE("ChannelListFrameCaller", bFinding);
	SAFE_ENABLE("LobbyOptionFrame", bFinding);
	SAFE_ENABLE("Logout", bFinding);
	SAFE_ENABLE("ReplayCaller", bFinding);
	SAFE_ENABLE("CharSelectionCaller", bFinding);
	SAFE_ENABLE("QuickJoin", bFinding);
	SAFE_ENABLE("QuickJoin2", bFinding);

	m_bWaitingArrangedGame = bFinding;
}

// ��a��oAa��E��O���� ��AA��C��A�� �ҡ� E����e��oAC ��o���ƥ�eA�� ��nE�ƨ���E��CN��U( E��A�� ������Y��e)
void ZGameInterface::OnDuelTournamentGameUI(bool bWaiting)
{
	MWidget *pWidget;

	pWidget = m_IDLResource.FindWidget("DuelTournamentWaitMatchDialog");
	if(pWidget) pWidget->Show(bWaiting);

	// ��UA�� A��A����eA�� enable/disable C��A����U
	SAFE_ENABLE("LobbyChannelPlayerList", bWaiting);
	SAFE_ENABLE("ShopCaller", bWaiting);
	SAFE_ENABLE("EquipmentCaller", bWaiting);
	SAFE_ENABLE("ChannelListFrameCaller", bWaiting);
	SAFE_ENABLE("LobbyOptionFrame", bWaiting);
	SAFE_ENABLE("Logout", bWaiting);
	SAFE_ENABLE("ReplayCaller", bWaiting);
	SAFE_ENABLE("CharSelectionCaller", bWaiting);
	SAFE_ENABLE("DuelTournamentGame", bWaiting);
	SAFE_ENABLE("DuelTournamentGame_2Test", bWaiting);
	SAFE_ENABLE("DuelTournamentGame_4Test", bWaiting);
}

bool ZGameInterface::IsReadyToPropose()
{
	if(GetState() != GUNZ_LOBBY)
		return false;

	if(m_bWaitingArrangedGame)
		return false;

	if(GetLatestExclusive()!=NULL)
		return false;

	if(m_pMsgBox->IsVisible())
		return false;

	// TODO : ��O���� modal A��AI �ҡ�AOA����e return false CIAU..
	return true;
}

bool ZGameInterface::IsMiniMapEnable()
{
	return GetCamera()->GetLookMode()==ZCAMERA_MINIMAP;
}

bool ZGameInterface::OpenMiniMap()
{
	if(!m_pMiniMap) {
		m_pMiniMap = new ZMiniMap;
		if(!m_pMiniMap->Create(ZGetGameClient()->GetMatchStageSetting()->GetMapName()))
		{
			SAFE_DELETE(m_pMiniMap);
			return false;
		}
	}

	return true;
}



class AchievementListBoxItem : public MListItem
{
protected:
	char m_Title[32];
	char m_Desc[128];
public:
	AchievementListBoxItem(const char* title, const char* desc) noexcept
	{
		strcpy_s(m_Title, sizeof(m_Title), title);
		strcpy_s(m_Desc, sizeof(m_Desc), desc);
	}

	virtual const char* GetString(void)
	{
		return m_Title;
	}
	virtual const char* GetString(int i)
	{
		if (i == 0)
			return m_Title;
		else if (i == 1)
			return m_Desc;

		return NULL;
	}
	virtual MBitmap* GetBitmap(int i)
	{
		return NULL;
	}
};

void ZGameInterface::ShowAchievementDialog(bool bShow)
{
	if (bShow)			// ����AI��aAI��e...
	{
		// ����CA����AI E����e ����AI��a
		MWidget* pWidget;
		pWidget = (MWidget*)m_IDLResource.FindWidget("AchievementUI");
		if (pWidget)
			pWidget->Show(true, true);



		// ��AAI ����������� AE��aE��
		MListBox* pListBox = (MListBox*)m_IDLResource.FindWidget("AchievementListBox");
		if (pListBox)
		{
			pListBox->RemoveAll();

			///TODO: send achievements to client

			for (auto const& itor : ZGetMyInfo()->GetAchievements())
			{
				mlog("%s, %s", itor.title, itor.desc);
				pListBox->Add(new AchievementListBoxItem(itor.title, itor.desc));
			}

			pListBox->Sort();		// Sorting
		}
	}
	else				// �ơ�A����aAI��e...
	{
		// ����CA����AI E����e �ơ�A����a
		ShowWidget("AchievementUI", false);
	}
}






// ����CA����AI ��u��A CO��o��e(�좯E?AI�Ƣ� A���Ƣ�)

/***********************************************************************
  ShowReplayDialog : public
  
  desc : ����CA����AI E����e ����AI��a
  arg  : true(=show) or false(=hide)
  ret  : none
************************************************************************/
class ReplayListBoxItem : public MListItem
{
protected:
	char		m_szName[ _MAX_PATH];
	char		m_szVersion[ 10];

public:
	ReplayListBoxItem( const char* szName, const char* szVersion)
	{
		strcpy(m_szName, szName);
		strcpy(m_szVersion, szVersion);
	}

	virtual const char* GetString( void)
	{
		return m_szName;
	}
	virtual const char* GetString( int i)
	{
		if ( i == 0)
			return m_szName;
		else if ( i == 1)
			return m_szVersion;

		return NULL;
	}
	virtual MBitmap* GetBitmap( int i)
	{
		return NULL;
	}
};



void ZGameInterface::ShowReplayDialog( bool bShow)
{
	if ( bShow)			// ����AI��aAI��e...
	{
		// ����CA����AI E����e ����AI��a
		MWidget* pWidget;
		pWidget = (MWidget*)m_IDLResource.FindWidget( "Replay");
		if ( pWidget)
			pWidget->Show( true, true);

		// '������a'��o���� ��nE�ƨ���E��
		pWidget = (MWidget*)m_IDLResource.FindWidget( "Replay_View");
		if ( pWidget)
			pWidget->Enable( false);


		// ��AAI ����������� AE��aE��
		MListBox* pListBox = (MListBox*)m_IDLResource.FindWidget( "Replay_FileList");
		if ( pListBox)
		{
			pListBox->RemoveAll();

			// Get path name
			TCHAR szPath[ MAX_PATH];
			if ( GetMyDocumentsPath( szPath))		// ���������� ��u��o �Ʃ���I
			{
				strcat( szPath, GUNZ_FOLDER);		// Gunz ��u��o �Ʃ���I
				strcat( szPath, REPLAY_FOLDER);		// Replay ��u��o �Ʃ���I
				CreatePath( szPath );
			}
			TCHAR szFullPath[ MAX_PATH];
			strcpy( szFullPath, szPath);

			strcat( szPath, "/*.gzr");			// E��Aa��i CEAI

			// Get file list
			struct _finddata_t c_file;
			intptr_t hFile;
			char szName[ _MAX_PATH];
			char szFullPathName[ _MAX_PATH];
			if ( (hFile = _findfirst( szPath, &c_file)) != -1L)
			{
				do
				{
					strcpy( szName, c_file.name);

					strcpy( szFullPathName, szFullPath);
					strcat( szFullPathName, "/");
					strcat( szFullPathName, szName);

					DWORD dwFileVersion = 0;
					char szVersion[10];
					int nRead;
					DWORD header;
					ZFile *file = zfopen( szFullPathName);
					if ( file)
					{
						nRead = zfread( &header, sizeof( header), 1, file);
						if( (nRead != 0) && (header == GUNZ_REC_FILE_ID))
						{
							zfread( &dwFileVersion, sizeof( dwFileVersion), 1, file);
							sprintf( szVersion, "v%d.0", dwFileVersion);
						}
						else
							strcpy( szVersion, "--");

						zfclose( file);
					}
					else
						strcpy( szVersion, "--");
				
					pListBox->Add( new ReplayListBoxItem( szName, szVersion));			// Add to listbox
				} while ( _findnext( hFile, &c_file) == 0);

				_findclose( hFile);
			}

			pListBox->Sort();		// Sorting
		}
	}
	else				// �ơ�A����aAI��e...
	{
		// ����CA����AI E����e �ơ�A����a
		ShowWidget( "Replay", false);
	}
}

/***********************************************************************
  ViewReplay : public
  
  desc : ����CA����AI ��CCa
  arg  : none
  ret  : none
************************************************************************/
void ZGameInterface::ViewReplay( void)
{
	// ����CA����AI ��UAI��o��I���� ��Y��a
	ShowReplayDialog( false);


	// ��u��o �Ʃ���I-E��AI��i ����CO
	MListBox* pListBox = (MListBox*)m_IDLResource.FindWidget( "Replay_FileList");
	if ( !pListBox)
		return ;

	if ( pListBox->GetSelItemString() == NULL)
		return ;

	TCHAR szName[ MAX_PATH];
	if ( GetMyDocumentsPath( szName))				// ���������� ��u��o �Ʃ���I
	{
		strcat( szName, GUNZ_FOLDER);				// Gunz ��u��o �Ʃ���I
		strcat( szName, REPLAY_FOLDER);				// Replay ��u��o �Ʃ���I
		strcat( szName, "/");
		strcat( szName, pListBox->GetSelItemString());
	}


	m_bOnEndOfReplay = true;
	m_nLevelPercentCache = ZGetMyInfo()->GetLevelPercent();

	// ����CA����AI ��CCa
	if ( !CreateReplayGame( szName))
	{
		ZApplication::GetGameInterface()->ShowMessage( "Can't Open Replay File" );
	}

	// ��E��C ��������AC AI��I ��o���� ��nE�ƨ���E��
	m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, false);
}



////////////////////////////////////////////////////////////////
#ifdef _QUEST_ITEM
void ZGameInterface::OnResponseCharacterItemList_QuestItem( MTD_QuestItemNode* pQuestItemNode, int nQuestItemCount )
{
	if( 0 == pQuestItemNode)
		return;

	ZGetMyInfo()->GetItemList()->SetQuestItemsAll( pQuestItemNode, nQuestItemCount );

	ZApplication::GetStageInterface()->SerializeSacrificeItemListBox();	
	ZGetMyInfo()->GetItemList()->Serialize();

	// ��oA���������� A����?AI UI Text A��������| �������AI���� C��A����U (��������, ��U��i����, A����A, ������i, �����O)
	GetShopEquipInterface()->DrawCharInfoText();
}

void ZGameInterface::OnResponseBuyQuestItem( const int nResult, const int nBP)
{
	if( MOK == nResult )
	{
		// ÀÓ½Ã
		//ZApplication::GetGameInterface()->ShowMessage( MSG_GAME_BUYITEM );

		ZGetMyInfo()->SetBP( nBP );
	}
	else if( MERR_TOO_MANY_ITEM == nResult )
	{
		ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
	}
	else if( MERR_TOO_EXPENSIVE_BOUNTY == nResult )
	{
		ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
	}
	else
	{
		mlog( "ZGameInterface::OnCommand::MC_MATCH_RESPONSE_BUY_QUEST_ITEM UNKNOWN ERROR.\n" );				
	}
	// ��oA���������� A����?AI UI Text A��������| �������AI���� C��A����U (��������, ��U��i����, A����A, ������i, �����O)
	GetShopEquipInterface()->DrawCharInfoText();
}

void ZGameInterface::OnResponseSellQuestItem( const int nResult, const int nBP)
{
	if( MOK == nResult )
	{
		// ÀÓ½Ã.
		//ZApplication::GetGameInterface()->ShowMessage(MSG_GAME_SELLITEM);

		ZGetMyInfo()->SetBP( nBP );
	}
	else
	{
	}

	// »óÁ¡¿¡¼­ Ä³¸¯ÅÍ UI Text Á¤º¸¸¦ ¾÷µ¥ÀÌÆ® ÇØÁØ´Ù (·¹º§, ¹Ù¿îÆ¼, Ã¼·Â, ¹æ¾î, ¹«°Ô)
	GetShopEquipInterface()->DrawCharInfoText();
}
#endif


// Äù½ºÆ®¿ë ¾ÆÀÌÅÛ ¾ÆÀÌÄÜ È­ÀÏ¸í(ÇÏµå ÄÚµù µ¢¾î¸®~  -_-;)
// ³õÀ»µ¥°¡ ¾ø¾î¼­ ÀÏ´Ü ¿©±â´Ù...  -_-;
MBitmap* ZGameInterface::GetQuestItemIcon( int nItemID, bool bSmallIcon)
{
	char szFileName[ 64] = "";
	switch ( nItemID)
	{
		// Page
		case 200001 :		// Ao��UA�� 13��aAIAo
		case 200002 :		// Ao��UA�� 25��aAIAo
		case 200003 :		// Ao��UA�� 41��aAIAo
		case 200004 :		// Ao��UA�� 65��aAIAo
			strcpy( szFileName, "slot_icon_page");
			break;

		// Skull
		case 200005 :		// AUA����I��n
		case 200006 :		// A���I��n
		case 200007 :		// ��o��o������aAC ��I��n
		case 200008 :		// ��i��i����AC ��I��n
		case 200009 :		// ��i��i���� A��AC ��I��n
		case 200010 :		// ��A��eCN A?��n
			strcpy( szFileName, "slot_icon_skull");
			break;

		// Fresh
		case 200011 :		// ��?��i��a
		case 200012 :		// ��O��i��a
		case 200013 :		// ����A��AIA��
			strcpy( szFileName, "slot_icon_fresh");
			break;

		// Ring
		case 200014 :		// A�ҡ�I��EAI
		case 200015 :		// A����I��EAI
		case 200016 :		// ��Y��I��EAI
		case 200017 :		// CA����������N ��I��EAI
			strcpy( szFileName, "slot_icon_ring");
			break;

		// Necklace
		case 200018 :		// A�Ϣ�������AC ��n��EAI
			strcpy( szFileName, "slot_icon_neck");
			break;

		// Doll
		case 200019 :		// ����AI����A�� AICu
		case 200020 :		// AU������a AICu
		case 200021 :		// ��i��i���� AICu
		case 200022 :		// ��E������i AICu
		case 200023 :		// Aa����AICu
		case 200024 :		// ��oAICu
		case 200025 :		// AuAO ����A�� ��oAICu
		case 200026 :		// ��E������i ����AI���
		case 200027 :		// AuAO ����A�� ��E������i ����AI���
			strcpy( szFileName, "slot_icon_doll");
			break;

		// Book
		case 200028 :		// ��C����AC ��cAu
		case 200029 :		// ����A�Ϣ�oAI��oAC ��i��I ��o��i
		case 200030 :		// ����A�Ϣ�oAI��oAC ��i��I CI��i
			strcpy( szFileName, "slot_icon_book");
			break;

		// Object
		case 200031 :		// Aa��������A�� ��EAU�Ƣ�
		case 200032 :		// AuAO����A�� ��EAU�Ƣ�
		case 200033 :		// ��i����AI
		case 200034 :		// ����CI��A �����IAI
		case 200035 :		// ��oA��AC ��aA��
		case 200036 :		// ��I����A�� ��a��i
		case 200037 :		// ��i���� ��a��i
			strcpy( szFileName, "slot_icon_object");
			break;

		// Sword
		case 200038 :		// ��i��i���� A��AC ������
		case 200039 :		// AU������a ��OAC ������
		case 200040 :		// ��A��e ����AI����A��AC A��
		case 200041 :		// AuAO����A�� ��A��AAC A��
		case 200042 :		// ����A��AC ��������
		case 200043 :		// ��E������iAC ��oA�� A��
		case 200044 :		// ����A�Ϣ�oAI��oAC A��
			strcpy( szFileName, "slot_icon_qsword");
			break;
	}

//	if ( bSmallIcon)
//		strcat( szFileName, "_s");

	strcat(szFileName, ".tga");


	return MBitmapManager::Get( szFileName);
}


void ZGameInterface::OnResponseServerStatusInfoList( const int nListCount, void* pBlob )
{
	ZServerView* pServerList = (ZServerView*)m_IDLResource.FindWidget( "SelectedServer");
	if ( !pServerList)
		return;

	int nCurrSel = pServerList->GetCurrSel2();
    pServerList->ClearServerList();



		if ( ZIsLaunchDevelop())
		{
			pServerList->AddServer( (char*)ZMsg(MSG_SERVER_DEBUG), "", 0, 7, 0, 1000, true);			// Debug server
//			pServerList->AddServer( "", "", 0, 0, 0, 1000, false);						// Null
		}



	if( (0 < nListCount) && (0 != pBlob) )
	{
		/*
		 * Au��U��C��A ServerList��A CoA| E�ƨ���E����C��iAO��A DBAC A���������� ��������AU.
		 *  Au��U��CAo ��EA�� ServerID��A Server�Ƣ� CoA| �좯AUA��AIAo ��E��A��IAO.
		 */

		for( int i = 0; i < nListCount; ++i )
		{
			MTD_ServerStatusInfo* pss = (MTD_ServerStatusInfo*)MGetBlobArrayElement( pBlob, i );
			if( 0 == pss )
			{
				mlog( "ZGameInterface::OnResponseServerStatusInfoList SERVER NOT FOUND IN LIST.", i );
				continue;
			}

			in_addr inaddr;
			inaddr.S_un.S_addr = pss->m_dwIP;
			
			char* pszAddr = inet_ntoa( inaddr );
			char strAddrBuf[32];
			strcpy(strAddrBuf, pszAddr);

			// pss->m_nCurPlayer;	// CoA| A�˨�OA?Au ��o.
			// pss->m_nMaxPlayer;	// ������o AO��e A�˨�O �Ƣ���E ��o.
			// pss->m_nPort;		// ������o Port.
			// pss->m_nServerID;	// ������o ID.
			// pss->m_nType;		// ������o A��AO.
			// pss->m_bIsLive;		// CoA| ������o�Ƣ� �좯AUCI��i AO��A�Ƣ�.

			char szServName[ 128 ] = {0,};
//				if( pss->m_bIsLive )
//					_snprintf( szServName, 127, "server%d", pss->m_nServerID );
//				else
//					_snprintf( szServName, 127, "server%d(dead)", pss->m_nServerID );

			if ( pss->m_nType == 1)					// Normal server (�������� "type==1"AI Debug server����A������ ��Y���Ҩ�i�������� AI��Y������o��I ��c��eCI��i AO��i���� ��?�Ʃ�)
				sprintf( szServName, "%s %d", ZMsg(MSG_SERVER_NORMAL), pss->m_nServerID );
			else if ( pss->m_nType == 2)			// Match server
				sprintf( szServName, "%s %d", ZMsg(MSG_SERVER_MATCH), pss->m_nServerID );
			else if ( pss->m_nType == 3)			// Clan server
				sprintf( szServName, "%s %d", ZMsg(MSG_SERVER_CLAN), pss->m_nServerID );
			else if ( pss->m_nType == 4)			// Quest server
				sprintf( szServName, "%s %d", ZMsg(MSG_SERVER_QUEST), pss->m_nServerID );
			else if ( pss->m_nType == 5)			// Event server
				sprintf( szServName, "%s %d", ZMsg(MSG_SERVER_EVENT), pss->m_nServerID );
			else if ( pss->m_nType == 6)			// Test server
				sprintf( szServName, "Test Server %d", pss->m_nServerID );
			else if ( pss->m_nType == 7)			// Debug server
				sprintf( szServName, "%s %d", ZMsg(MSG_SERVER_DEBUG), pss->m_nServerID );
			else 
				continue;


			int nRow = 0;
			int nCol = 0;
			char* pszAgentIP = "";

			// ������o AI����A�� ������������ A��A�� C�ͨ�AC��AU
			sprintf( szServName, "%s", pss->m_szServerName);

			if ( (pss->m_nServerID % 50) >= 25)
				nRow = 1;

#ifdef LOCALE_NHNUSA
			//	��I���� ������o ����������� ��e����
			//	A�����̨�����o  (Type 2) ������oID: ������IA������50~74,		�좯��IA������75~99		(A�����̨�����o��A 50~99 ��cAI)
			//	��AA��������o  (Type 3) ������oID: ������I��AA��100~124,		�좯��I��AA��125~149		(��AA��������o��A 100~149 ��cAI)
			//	Au�������������o(Type 4) ������oID: ������IAu��������150~174,	�좯��IAu��������175~199	(Au�������������o��A 150~200 ��cAI)
			//	A�����̨�����o  (Type 2) ������oID: XXXXA������250~74,		A?������IA������275~299		(A�����̨�����o��A 250~299 ��cAI)
			//	��AA��������o  (Type 3) ������oID: XXXX��AA��300~324,		A?������AA��325~349		(��AA��������o��A 300~349 ��cAI)
			//	Au�������������o(Type 4) ������oID: XXXX��IAu��������350~374,	A?����Au��������375~399	(Au�������������o��A 350~400 ��cAI)
			//	AI������o��eA�� AA��i��I ��������AO��i��IA����e  25��aA��A����I  25������U AUA����e ����AE, A�Ϣ�e ��A����AE
			//	AI������o��eA�� ��oCI��I ��������AO��i��IA����e 200��aA��A����I 200������U AUA����e A��AE, A�Ϣ�e ��������AE

			// A?����������o��| CNA�� ����A����I C��A����U. 
			// ����A������ 4�Ʃ���I �������ϡ�O ��E��U��e ���ϡ�a���� A������C��AO��e��E��U.
			//		-------------------------
			//		|	������I	|	�좯��I	|
			//		|	nRow=0	|	nRow=1	|
			//		|	nCol=0 	|	nCol=0	|
			//		-------------------------
			//		|	XXXX	|	A?����	|
			//		|	nRow=0	|	nRow=1	|
			//		|	nCol=1 	|	nCol=1	|
			//		-------------------------
 			if ( pss->m_nServerID > 200)
			{ // 200 AI��o��IAI A?���� ������o��I, XXXX������oAI��U.
				nCol = 1;
			}

			// ������o A���Ƣ�
			sockaddr_in Addr;
			Addr.sin_addr.S_un.S_addr = pss->m_dwAgentIP;
			pszAgentIP = inet_ntoa(Addr.sin_addr);

#else if defined(LOCALE_KOREA) || defined(_DEBUG)
			//		-------------------------
			//		|			|	A������Au	|
			//		|	AI��Y������o|	nRow=1	|
			//		|	nRow=0 	|	nCol=0	|
			//		|	nCol=0	|------------
			//		|			|	Au��������	|
			//		------------|	nRow=1	|
			//		|			|	nCol=1	|
			//		|	�������ר�����o|------------
			//		|	nRow=0	|	AUA?������o|
			//		|	nCol=1 	|	nRow=1	|
			//		|			|	nCol=2	|
			//		-------------------------
			// ������a��iCI��O CI��aAU��u~~
			// AI��Y������o 1~50, �������ר�����o 51~100, A������Au 101~150, Au�������������o 151~200, AUA?������o 201~250
				 if (							pss->m_nServerID <  51)	{	nCol = 0; nRow = 0;	}
			else if (  50 < pss->m_nServerID && pss->m_nServerID < 101)	{	nCol = 1; nRow = 0;	}
			else if ( 100 < pss->m_nServerID && pss->m_nServerID < 151)	{	nCol = 0; nRow = 1;	}
			else if ( 150 < pss->m_nServerID && pss->m_nServerID < 201)	{	nCol = 1; nRow = 1;	}
			else if ( 200 < pss->m_nServerID && pss->m_nServerID < 251)	{	nCol = 2; nRow = 1;	}
#endif
			pServerList->AddServer( szServName, strAddrBuf, pss->m_nPort, pss->m_nType, pss->m_nCurPlayer, pss->m_nMaxPlayer, pss->m_bIsLive, pszAgentIP, nRow, nCol);

		}
	}
	pServerList->SetCurrSel( nCurrSel);

	m_dwRefreshTime = timeGetTime() + 10000;
}


void ZGameInterface::OnResponseBlockCountryCodeIP( const char* pszBlockCountryCode, const char* pszRoutingURL )
{
	if( 0 != pszBlockCountryCode )
	{
		// ��i���ƥ�E IPAC �����Ƣ���| A����oAI�������碯�� Ae����C��Aa���� CO.
		// Message���� Aa��A ����AaA�� A���Ƣ�C������ CO.

		ShowMessage( pszRoutingURL );
	}
}


void ZGameInterface::RequestServerStatusListInfo()
{
	ZLocatorList*	pLocatorList;

	if ( ZApplication::GetInstance()->IsLaunchTest())				// A���������� ����AAAI �Ʃ���i
		pLocatorList = m_pTLocatorList;
	else
		pLocatorList = m_pLocatorList;


	if( 0 == pLocatorList ) 
		return;

	if ( pLocatorList->GetSize() < 1)
		return;

	MCommand* pCmd = ZNewCmd( MC_REQUEST_SERVER_LIST_INFO );
	if( 0 != pCmd )
	{
		const string strIP = pLocatorList->GetIPByPos( m_nLocServ++);
		m_nLocServ %= pLocatorList->GetSize();

		GetGameClient()->SendCommandByUDP( pCmd, const_cast<char*>(strIP.c_str()), LOCATOR_PORT);
		delete pCmd;
	}

	m_dwRefreshTime = timeGetTime() + 1500;
}


/*
bool ZGameInterface::InitLocatorList( MZFileSystem* pFileSystem, const char* pszLocatorList )
{
	if( (0 == pszLocatorList) || (0 == strlen(pszLocatorList)) ) 
		return false;

	if( 0 == m_pLocatorList )
	{
		m_pLocatorList = new ZLocatorList;
		if( 0 == m_pLocatorList )
			return false;
	}
	else
		m_pLocatorList->Clear();
	
	return m_pLocatorList->Init( pFileSystem, pszLocatorList );

	return false;
}
*/

void ZGameInterface::OnDisconnectMsg( const DWORD dwMsgID )
{
	MListBox* pListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "QuestResult_ItemListbox");
	if( 0 != pListBox )
	{
		// C��A�� ��u��A ������AAoAI �Ʃ���i���� A����a ������aA�� CN��U.
		if ( (dwMsgID == MERR_FIND_HACKER) || (dwMsgID == MERR_BLOCK_HACKER) 
			|| (dwMsgID == MERR_BLOCK_BADUSER) || (dwMsgID == MERR_FIND_INVALIDFILE) 
			|| (dwMsgID == MERR_FIND_FLOODING) || (dwMsgID == MERR_BLOCK_BY_ADMIN)
			|| (dwMsgID == MERR_BLOCK_SLEEP_ACCOUNT) )
		{
			m_bReservedQuit = true;
			m_dwReservedQuitTimer = timeGetTime() + 5000;
		}

		ShowMessage( ZErrStr(dwMsgID) );
	}
}

void ZGameInterface::ShowDisconnectMsg( DWORD errStrID, DWORD delayTime )
{
	m_bReservedQuit = true;
	m_dwReservedQuitTimer = timeGetTime() + delayTime;

	ShowMessage( ZErrStr(errStrID) );
}


void ZGameInterface::OnAnnounceDeleteClan( const string& strAnnounce )
{
	char szMsg[ 128 ];
	
	sprintf( szMsg, MGetStringResManager()->GetErrorStr(MERR_CLAN_ANNOUNCE_DELETE), strAnnounce.c_str() );
	ShowMessage( szMsg );
}



// ����AI���� ��c��i��a ��u��A
void ZGameInterface::OnVoiceSound()
{
	DWORD dwCurrTime = timeGetTime();

	// AoA����E ��A�ơ�AI �쨡��AAo ��E��c
	if ( dwCurrTime < m_dwVoiceTime)
		return;

	m_szCurrVoice[ 0] = 0;

	// ������a��E ��c��i��a Aa��A
	if ( m_szNextVoice[ 0] == 0)
		return;

	ZApplication::GetSoundEngine()->PlaySound( m_szNextVoice);
	m_dwVoiceTime = dwCurrTime + m_dwNextVoiceTime;

	strcpy( m_szCurrVoice, m_szNextVoice);
	m_szNextVoice[ 0] = 0;
	m_dwNextVoiceTime = 0;
}

void ZGameInterface::PlayVoiceSound( const char* pszSoundName, DWORD time)
{
	if ( !Z_AUDIO_NARRATIONSOUND)
		return;


	if ( strcmp( pszSoundName, m_szCurrVoice) != 0)
	{
		sprintf( m_szNextVoice, pszSoundName);
		m_dwNextVoiceTime = time;
	}


	// CA����AI A��AI ��c��i��a�Ƣ� ����A����e ��U��I CA����AICN��U.
	if ( timeGetTime() > m_dwVoiceTime)
		OnVoiceSound();
}

void ZGameInterface::OnSendGambleItemList( void* pGItemArray, const DWORD dwCount )
{
	ZGetGambleItemDefineMgr().Release();

	MTD_DBGambleItmeNode*	pGItem;
	ZGambleItemDefine*		pZGItem;


	for( DWORD i = 0; i < dwCount; ++i )
	{
		pGItem = (MTD_DBGambleItmeNode*)MGetBlobArrayElement( pGItemArray, i );
		if( NULL ==  pGItem )
			return;

		pZGItem = new ZGambleItemDefine( pGItem->nItemID
			, pGItem->szName
			, pGItem->szDesc
			, pGItem->nBuyPrice
			, pGItem->bIsCash);
		if( NULL == pZGItem )
			return;

		if( !ZGetGambleItemDefineMgr().AddGambleItemDefine(pZGItem) )
		{
			_ASSERT( 0 );
		}
	}
}

void ZGameInterface::SetAgentPing(DWORD nIP, DWORD nTimeStamp)
{
	ZServerView* pServerList = (ZServerView*)m_IDLResource.FindWidget( "SelectedServer");
	pServerList->SetAgentPing(nIP, nTimeStamp);
}

void ZGameInterface::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	MWidget::MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);

	if (m_pMsgBox)
		m_pMsgBox->MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);
	if (m_pConfirmMsgBox)
		m_pConfirmMsgBox->MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);
}

void ZGameInterface::SetDuelTournamentCharacterList(MDUELTOURNAMENTTYPE nType, const vector<DTPlayerInfo>& vecDTPlayerInfo)
{
	m_eDuelTournamentType = nType;

	m_vecDTPlayerInfo.clear();
	m_vecDTPlayerInfo = vecDTPlayerInfo;

}

void ZGameInterface::UpdateDuelTournamantMyCharInfoUI()
{
	ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
	if (!pRes) return;

	const ZGameClient::DTCHARINFO* pCharInfo = GetGameClient()->GetMyDuelTournamentCharInfo();
	if (!pCharInfo) return;

	char szOutput[256];
	sprintf(szOutput,"%d/%d",pCharInfo->wins,pCharInfo->losses);
	ZBmNumLabel *pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoWinLose");
	if (pNumLabel)
		pNumLabel->SetText(szOutput);
	sprintf(szOutput,"%d",pCharInfo->tournamentPoint);
	pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoPoints");
	if (pNumLabel)
		pNumLabel->SetText(szOutput);
	sprintf(szOutput,"%d",pCharInfo->winners);
	pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoWinners");
	if (pNumLabel)
		pNumLabel->SetText(szOutput);

	int ranking = (pCharInfo->ranking == -1) ? 0 : pCharInfo->ranking;
	sprintf(szOutput,"%d",ranking);
	pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoRanking");
	if (pNumLabel)
		pNumLabel->SetText(szOutput);

	GetDuelTournamentGradeIconFileName(szOutput, pCharInfo->lastWeekGrade);
	MPicture* pPicture= (MPicture*)pRes->FindWidget("Lobby_DuelTournamentInfoEmblem");
	if (pPicture) {
		pPicture->SetBitmap(MBitmapManager::Get(szOutput));

		char sz[32];
		char szTooltip[256];
		sprintf(sz, "%d", pCharInfo->lastWeekGrade);
		ZTransMsg(szTooltip, MSG_LOBBY_DUELTOURNAMENT_RECORD_TOOPTIP_GRADE, 1, sz);
		SetWidgetToolTipText("Lobby_DuelTournamentInfoEmblem", szTooltip, MAM_VCENTER);
	}
}

void ZGameInterface::UpdateDuelTournamantMyCharInfoPreviousUI()
{
	ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
	if (!pRes) return;

	const ZGameClient::DTCHARINFO* pCharInfo = GetGameClient()->GetMyDuelTournamentCharInfoPrev();
	if (!pCharInfo) return;

	char szOutput[256];
	sprintf(szOutput,"%d/%d",pCharInfo->wins,pCharInfo->losses);
	ZBmNumLabel *pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoWinLosePrev");
	if (pNumLabel)
	{
		pNumLabel->SetIndexOffset(16);		// ��o��U���� ��U������I Ai��A��U
		pNumLabel->SetText(szOutput);
	}
	sprintf(szOutput,"%d",pCharInfo->tournamentPoint);
	pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoPointsPrev");
	if (pNumLabel)
	{
		pNumLabel->SetIndexOffset(16);
		pNumLabel->SetText(szOutput);
	}
	sprintf(szOutput,"%d",pCharInfo->winners);
	pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoWinnersPrev");
	if (pNumLabel)
	{
		pNumLabel->SetIndexOffset(16);
		pNumLabel->SetText(szOutput);
	}
	
	int ranking = (pCharInfo->ranking == -1) ? 0 : pCharInfo->ranking;
	sprintf(szOutput,"%d",ranking);
	pNumLabel = (ZBmNumLabel*)pRes->FindWidget("Lobby_DuelTournamentInfoRankingPrev");
	if (pNumLabel)
	{
		pNumLabel->SetIndexOffset(16);
		pNumLabel->SetText(szOutput);
	}
}


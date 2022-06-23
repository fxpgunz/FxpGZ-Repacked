#include "StdAfx.h"
#include "ZOptionInterface.h"
#include "MSlider.h"
#include "ZConfiguration.h"
#include "ZActionKey.h"
#include "FileInfo.h"
#include "ZCanvas.h"
#include "ZInput.h"
#include "ZRoomListBox.h"
#include "ZShopEquipListbox.h"
#include "ZShopEquipInterface.h"
#include "ZScreenEffectManager.h"
#include <experimental/filesystem>
#define DEFAULT_SLIDER_MAX			10000

#define DEFAULT_GAMMA_SLIDER_MIN	50
#define DEFAULT_GAMMA_SLIDER_MAX	800

#define LISTBOX_CELL_ARRANGE(pList, feldsize, flsize)   { pList->GetField(1)->nTabSize += (int)( (float)feldsize * flsize) - (int)( (float)feldsize);  }  // 그림칸 크기는 고정, 그부분만큼 2번열에 더함

static	map< int, D3DDISPLAYMODE> gDisplayMode;
#define DEFAULT_REFRESHRATE			0

template< class F, class S>
class value_equals
{
private:
	S second;
public:
	value_equals(const S& s) : second(s) {}
	bool operator() (pair<const F, S> elem)
	{ return elem.second == second; }
};

bool operator == ( D3DDISPLAYMODE lhs, D3DDISPLAYMODE rhs )
{
	return( lhs.Width == rhs.Width && lhs.Height == rhs.Height && lhs.Format == rhs.Format );
}

static int widths[]={ 640,800,1024,1280,1600,1280,1440, 1650, 1920, 2048,2560,2560,3840};
static int heights[]={ 480,600,768,960,1200,800,900, 1050, 1200, 1536,1440,1600,2160};


ZOptionInterface::ZOptionInterface(void)
{
	mbTimer = false;

//	mOldScreenWidth = 0;
//	mOldScreenHeight = 0;
	mnOldBpp = D3DFMT_A8R8G8B8;
	mTimerTime = 0;

	mOldScreenWidth = 800;
	mOldScreenHeight = 600;

}

ZOptionInterface::~ZOptionInterface(void)
{
	gDisplayMode.clear();
}

void ZOptionInterface::InitInterfaceOption(void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	mlog("start InitInterface option\n");
	/*
	// Mouse Sensitivity Min/Max (Z_MOUSE_SENSITIVITY_MIN ~ Z_MOUSE_SENSITIVITY_MAX)
	BEGIN_WIDGETLIST("MouseSensitivitySlider", pResource, MSlider*, pWidget);
	pWidget->SetMinMax(Z_MOUSE_SENSITIVITY_MIN, Z_MOUSE_SENSITIVITY_MAX);
	pWidget->SetValue(Z_MOUSE_SENSITIVITY);
	END_WIDGETLIST();
	*/
	MSlider* pWidget = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if(pWidget)
	{
		pWidget->SetMinMax(MOUSE_SENSITIVITY_MIN, MOUSE_SENSITIVITY_MAX);
		pWidget->SetValue( ZGetConfiguration()->GetMouseSensitivityInInt());
	}

	pWidget = (MSlider*)pResource->FindWidget("JoystickSensitivitySlider");
	if(pWidget)
	{
		pWidget->SetMinMax(0, DEFAULT_SLIDER_MAX);
		pWidget->SetValue(ZGetConfiguration()->GetJoystick()->fSensitivity * DEFAULT_SLIDER_MAX);
	}

	pWidget = (MSlider*)pResource->FindWidget("BGMVolumeSlider");
	if(pWidget)
	{
		pWidget->SetMinMax(0, DEFAULT_SLIDER_MAX);
		pWidget->SetValue(Z_AUDIO_BGM_VOLUME*DEFAULT_SLIDER_MAX);
	}

	pWidget = (MSlider*)pResource->FindWidget("EffectVolumeSlider");
	if(pWidget)
	{
		pWidget->SetMinMax(0, DEFAULT_SLIDER_MAX);
		pWidget->SetValue(Z_AUDIO_EFFECT_VOLUME*DEFAULT_SLIDER_MAX);
	}

	pWidget = (MSlider*)pResource->FindWidget("VideoGamma");
	if(pWidget)
	{
		pWidget->SetMinMax(DEFAULT_GAMMA_SLIDER_MIN, DEFAULT_GAMMA_SLIDER_MAX);
		pWidget->SetValue(Z_VIDEO_GAMMA_VALUE*DEFAULT_GAMMA_SLIDER_MAX);
		pWidget->SetValue(Z_VIDEO_GAMMA_VALUE);
	}

	// Action Key
	for(int i=0; i<ZACTION_COUNT; i++){
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);

		BEGIN_WIDGETLIST(szItemName, pResource, ZActionKey*, pWidget);
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey);
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKeyAlt);
		END_WIDGETLIST();
	}

	//	ComboBox
	{
		MComboBox *pWidget = (MComboBox*)pResource->FindWidget("ScreenResolution");
		if (pWidget)
		{
			pWidget->RemoveAll();
			gDisplayMode.clear();

			int dmIndex = 0;
			char szBuf[256];

			D3DDISPLAYMODE ddm;
			int nDM = RGetAdapterModeCount(D3DFMT_X8R8G8B8, D3DADAPTER_DEFAULT);

			mlog("Number of Display mode : %d\n", nDM);

			for (int idm = 0; idm < nDM; ++idm)
			{
				if (REnumAdapterMode(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, idm, &ddm))
				{
					if (ddm.Width < 640 || ddm.Height < 480)
						continue;

					ddm.RefreshRate = DEFAULT_REFRESHRATE;

					if (ddm.Format == D3DFMT_X8R8G8B8)
					{
						map<int, D3DDISPLAYMODE>::iterator iter_ = find_if(gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
						if (iter_ == gDisplayMode.end())
						{
							gDisplayMode.insert(map<int, D3DDISPLAYMODE>::value_type(dmIndex++, ddm));
							sprintf(szBuf, "%d x %d %dbpp", ddm.Width, ddm.Height, 32);
							pWidget->Add(szBuf);
						}
					}
				}
			}
			// 만약 등록된 해상도가 하나도 없을경우 강제로 등록
			if (gDisplayMode.size() == 0)
			{
				int i = 0;
				for (auto dDMItr : gDisplayMode)
				{
					ddm.Width = dDMItr.second.Width;
					ddm.Height = dDMItr.second.Height;
					ddm.RefreshRate = dDMItr.second.RefreshRate;
					ddm.Format = ((dDMItr.second.Format % 2 == 1) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5);

					int bpp = 32;
					gDisplayMode.insert(map<int, D3DDISPLAYMODE>::value_type(i, ddm));
					sprintf(szBuf, "%dx%d  %d bpp", ddm.Width, ddm.Height, bpp);
					pWidget->Add(szBuf);
					i++;
				}
			}
			ddm.Width = RGetScreenWidth();
			ddm.Height = RGetScreenHeight();
			ddm.RefreshRate = DEFAULT_REFRESHRATE;
			ddm.Format = RGetPixelFormat();
			map< int, D3DDISPLAYMODE>::iterator iter = find_if(gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
			
			if(iter != gDisplayMode.end())
				pWidget->SetSelIndex(iter->first);
		}

		pWidget = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if (pWidget) {
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nCharTexLevel);
		}

		pWidget = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if (pWidget) {
			if (ZGetConfiguration()->GetVideo()->nMapTexLevel > 7) {
				pWidget->SetSelIndex(3);
			}
			else if (ZGetConfiguration()->GetVideo()->nMapTexLevel > 3) {
				pWidget->SetSelIndex(4);
			}
			else {
				pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nMapTexLevel);
			}
		}

		pWidget = (MComboBox*)pResource->FindWidget("EffectLevel");
		if (pWidget) {
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nEffectLevel);
		}

		pWidget = (MComboBox*)pResource->FindWidget("TextureFormat");
		if (pWidget) {
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nTextureFormat);
		}

		// 동영상 캡쳐 20081017... by kam
		pWidget = (MComboBox*)pResource->FindWidget("MovingPictureResolution");
		if (pWidget) {
			pWidget->SetSelIndex(ZGetConfiguration()->GetMovingPicture()->iResolution); // 동영상 캡쳐 해상도 세팅
		}
		// 동영상 캡쳐 20081028... by kam
		pWidget = (MComboBox*)pResource->FindWidget("MovingPictureFileSize");
		if (pWidget) {
			pWidget->SetSelIndex(ZGetConfiguration()->GetMovingPicture()->iFileSize); // 동영상 캡쳐 파일크기(용량제한)
		}

		// 언어 선택
		pWidget = (MComboBox*)pResource->FindWidget("LanguageSelectComboBox");
		if (pWidget)
		{
			pWidget->RemoveAll();
			size_t size = ZGetConfiguration()->GetLocale()->vecSelectableLanguage.size();

			for (unsigned int i = 0; i < size; ++i) {
				pWidget->Add(ZGetConfiguration()->GetLocale()->vecSelectableLanguage[i].strLanguageName.c_str());
			}

			pWidget->SetSelIndex(ZGetConfiguration()->GetSelectedLanguageIndex());

			GunzState state = ZApplication::GetGameInterface()->GetState();
			if (state == GUNZ_GAME || state == GUNZ_STAGE)
				pWidget->Enable(false);
			else
				pWidget->Enable(true);
		}

		pWidget = (MComboBox*)pResource->FindWidget("CustomMusic");
		if (pWidget)
		{
			pWidget->RemoveAll();

			if (ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
			{
				if (CustomMusic::m_customMusic.size() > 0)
				{

					if (CustomMusic::folderName.size() <= 0)
						pWidget->Enable(false);

					for (auto itor = CustomMusic::folderName.begin(); itor != CustomMusic::folderName.end(); ++itor)
					{
						pWidget->Add(itor->c_str());
					}
					pWidget->SetSelIndex(ZGetConfiguration()->GetAudio()->nCustomMusicFolders);
				}
			}
			else
				pWidget->SetSelIndex(0);
		}
	

		pWidget = (MComboBox*)pResource->FindWidget("CustomMusicList");
		if (pWidget)
		{
			pWidget->RemoveAll();

			if (ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
			{
				if (CustomMusic::m_customMusic.size() > 0)
				{
					for (int i = 0; i < CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].size(); ++i)
					{
						string songName, baseText;

						songName = CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].at(i).c_str();
						baseText.append(songName.substr(songName.find_last_of("\\") + 1));
						pWidget->Add(baseText.c_str());
					}
					if (CustomMusic::g_NextMusicIndex != -1)
						pWidget->SetSelIndex(CustomMusic::g_NextMusicIndex);
					else
						pWidget->SetSelIndex(0);
				}
			}
			else
				pWidget->SetSelIndex(0);
		}

	}

	//	Button
	{
		MButton* pWidget = (MButton*)pResource->FindWidget("Reflection");
		if( pWidget )
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bReflection);
		}

		pWidget = (MButton*)pResource->FindWidget("LightMap");
		if( pWidget )
		{
			//if(ZGetConfiguration()->GetVideo()->bTerrible && !RIsHardwareTNL() )
			{
				pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bLightMap);

				if(ZGetGame()) {
					ZGetGame()->GetWorld()->GetBsp()->LightMapOnOff(ZGetConfiguration()->GetVideo()->bLightMap);
				}
				else {
					RBspObject::SetDrawLightMap(ZGetConfiguration()->GetVideo()->bLightMap);
				}
			}

		}

		pWidget = (MButton*)pResource->FindWidget("Archetype");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bArchetype);
		}

		pWidget = (MButton*)pResource->FindWidget("DynamicLight");
		if( pWidget )
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bDynamicLight);
		}

		pWidget = (MButton*)pResource->FindWidget("Shader");
		if( pWidget )
		{
			//if( !RShaderMgr::shader_enabled )
			if(!RIsSupportVS())
			{
				pWidget->SetCheck(false);
				pWidget->Enable(false);
			}
			else
			{
				pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bShader);
			}
		}
		
		pWidget = (MButton*)pResource->FindWidget("NewRenderer");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bNewRenderer);
		}

		pWidget = (MButton*)pResource->FindWidget("D3D9EX");
		if (pWidget) {
			OSVERSIONINFOEX os;
			os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			GetVersionEx((OSVERSIONINFO*)&os);
			if (os.dwMajorVersion < 6)
				pWidget->Enable(false);

			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->enableD3D9Ex);
		}


		pWidget = (MButton*)pResource->FindWidget("BGMMute");
		if(pWidget)
		{
			pWidget->SetCheck( !ZGetConfiguration()->GetAudio()->bBGMMute );
		}

		pWidget = (MButton*)pResource->FindWidget("EffectMute");
		if(pWidget)
		{
			pWidget->SetCheck( !ZGetConfiguration()->GetAudio()->bEffectMute );
		}

		pWidget	= (MButton*)pResource->FindWidget("Effect3D");
		if(pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetAudio()->b3DSound);
		}

		pWidget = (MButton*)pResource->FindWidget("EnableCustomMusic");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetAudio()->bCustomMusicEnabled);
		}

		pWidget = (MButton*)pResource->FindWidget("8BitSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_8BITSOUND);
			pWidget = (MButton*)pResource->FindWidget("16BitSound");
			if(pWidget) pWidget->SetCheck(!Z_AUDIO_8BITSOUND);
		}

		pWidget = (MButton*)pResource->FindWidget("BackGroundMusic");
		if (pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_BACKGROUNDAUDIO);
		}

		//Trail Option
		pWidget = (MButton*)pResource->FindWidget("TrailOption");
		if ( pWidget )
		{
			pWidget->SetCheck(ZGetConfiguration()->GetEtc()->bTrailOption);
		}

		pWidget =(MButton*)pResource->FindWidget("FpsOverlay");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetEtc()->bFpsOverlay);
		}
		pWidget = (MButton*)pResource->FindWidget("InverseSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_INVERSE);
		}
		pWidget = (MButton*)pResource->FindWidget("HWMixing");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_HWMIXING);
		}
		pWidget = (MButton*)pResource->FindWidget("HitSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_HITSOUND);
		}
		pWidget = (MButton*)pResource->FindWidget("NarrationSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_NARRATIONSOUND);
		}
		pWidget = (MButton*)pResource->FindWidget("InvertMouse");
		if(pWidget)
		{
			pWidget->SetCheck(Z_MOUSE_INVERT);
		}
		pWidget = (MButton*)pResource->FindWidget("InvertJoystick");
		if(pWidget)
		{
			pWidget->SetCheck(Z_JOYSTICK_INVERT);
		}

	}

	// Etc
	{	
		MEdit* pEdit = (MEdit*)pResource->FindWidget("NetworkPort1");
		if (pEdit)
		{
			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_NETWORKPORT1);
			pEdit->SetText(szBuf);
		}

		pEdit = (MEdit*)pResource->FindWidget("NetworkPort2");
		if (pEdit)
		{
			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_NETWORKPORT2);
			pEdit->SetText(szBuf);
		}

		pEdit = (MEdit*)pResource->FindWidget("MouseSensitivityEdit");
		if (pEdit)
		{
			char szBuf[1024];
			sprintf(szBuf, "%d", ZGetConfiguration()->GetMouseSensitivityInInt());
			pEdit->SetText(szBuf);
			pEdit->SetMaxLength(16);
		}

		MButton* pBtnBoost = (MButton*)pResource->FindWidget("BoostOption");
		if (pBtnBoost)
		{
			pBtnBoost->SetCheck(Z_ETC_BOOST);
		}
		

		MButton* pBtnNormalChat = (MButton*)pResource->FindWidget("NormalChatOption");
		if (pBtnNormalChat)
		{
			pBtnNormalChat->SetCheck( Z_ETC_REJECT_NORMALCHAT);
		}

		MButton* pBtnTeamChat = (MButton*)pResource->FindWidget("TeamChatOption");
		if (pBtnTeamChat)
		{
			pBtnTeamChat->SetCheck(Z_ETC_REJECT_TEAMCHAT);
		}

		MButton* pBtnClanChat = (MButton*)pResource->FindWidget("ClanChatOption");
		if (pBtnClanChat)
		{
			pBtnClanChat->SetCheck(Z_ETC_REJECT_CLANCHAT);
		}

		MButton* pBtnWhisper = (MButton*)pResource->FindWidget("WhisperOption");
		if (pBtnWhisper)
		{
			pBtnWhisper->SetCheck(Z_ETC_REJECT_WHISPER);
		}


		MButton* pHPAP = (MButton*)pResource->FindWidget("HPAP");
		if( pHPAP )
		{
			pHPAP->SetCheck(ZGetConfiguration()->GetVideo()->bHPAP);
		}

		MButton* pBtnInvite = (MButton*)pResource->FindWidget("InviteOption");
		if (pBtnInvite)
		{
			pBtnInvite->SetCheck(Z_ETC_REJECT_INVITE);
		}

		MButton* pScroll = (MButton*)pResource->FindWidget("DisableSWScroll");
		if (pScroll)
		{
			pScroll->SetCheck(Z_ETC_SW_SCROLL);
		}

		MComboBox *pComboBox = (MComboBox*)pResource->FindWidget("CrossHairComboBox");
		if(pComboBox)
		{
			pComboBox->RemoveAll();

			// 기본 크로스 헤어 하드코딩으로 입력

			for (int i = 0; i < ZCSP_CUSTOM; i++)
			{
				char szText[256];
				sprintf(szText, "%s %d", ZMsg(MSG_WORD_TYPE), i+1);
				pComboBox->Add(szText);
			}
			char szCustomFile[256];
			sprintf(szCustomFile, "%s%s%s", PATH_CUSTOM_CROSSHAIR, FN_CROSSHAIR_HEADER,  FN_CROSSHAIR_TAILER);
			if (IsExist(szCustomFile)) pComboBox->Add("Custom");
			if (Z_ETC_CROSSHAIR >= pComboBox->GetCount())	// 사용자지정이였는데 사용자지정이 없어졌을 경우
			{
				Z_ETC_CROSSHAIR = 0;
			}
			pComboBox->SetSelIndex(Z_ETC_CROSSHAIR);			
		}

		ZCanvas* pCrossHairPreview = (ZCanvas*)pResource->FindWidget("CrossHairPreviewCanvas");
		if (pCrossHairPreview)
		{
			pCrossHairPreview->SetOnDrawCallback(ZCrossHair::OnDrawOptionCrossHairPreview);
		}

		// 1초당 프레임 제한
		pComboBox = (MComboBox*)pResource->FindWidget("FrameLimit_PerSecond");
		if(pComboBox)	{
			if (Z_ETC_FRAMELIMIT_PERSECOND >= pComboBox->GetCount())	// 사용자지정이였는데 사용자지정이 없어졌을 경우
			{
				Z_ETC_FRAMELIMIT_PERSECOND = 0;
			}
			pComboBox->SetSelIndex(Z_ETC_FRAMELIMIT_PERSECOND); // 1초당 프레임 제한
			RSetFrameLimitPerSeceond(Z_ETC_FRAMELIMIT_PERSECOND);
		}

		pComboBox = (MComboBox*)pResource->FindWidget("AntiAlias");
		if (pComboBox != nullptr)
		{
			{
				if (Z_VIDEO_ANTIALIAS >= pComboBox->GetCount())
					Z_VIDEO_ANTIALIAS = 0;

				if (pComboBox)
					pComboBox->SetSelIndex(Z_VIDEO_ANTIALIAS);


			}
		}

		MButton* pButton = (MButton*)pResource->FindWidget("FullScreen");
		if (pButton)
		{
			pButton->SetCheck(ZGetConfiguration()->GetVideo()->bFullScreen);
		}

	}

	//Macro
	{
		static char stemp_str[ZCONFIG_MACRO_MAX][80] = {
			"MacroF1",
			"MacroF2",
			"MacroF3",
			"MacroF4",
			"MacroF5",
			"MacroF6",
			"MacroF7",
			"MacroF8",
		};

		ZCONFIG_MACRO* pMacro = ZGetConfiguration()->GetMacro();

		if(pMacro) {
			MEdit* pEdit = NULL;
			for(int i=0;i<ZCONFIG_MACRO_MAX;i++) {

				pEdit = (MEdit*) pResource->FindWidget(stemp_str[i]);

				if (pEdit) {
					pEdit->SetText( pMacro->GetString(i) );
				}
			}
		}
	}

	mlog("end of InitInterface option ok\n");
}

bool ZOptionInterface::SaveInterfaceOption(void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	bool bResetDevice = false;
	{ // 슬라이더
		MSlider* pWidget = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
		Z_MOUSE_SENSITIVITY = (float) ((MSlider*)pWidget)->GetValue() / (float)MOUSE_SENSITIVITY_MAX;

		pWidget = (MSlider*)pResource->FindWidget("JoystickSensitivitySlider");
		Z_JOYSTICK_SENSITIVITY = (float) ((MSlider*)pWidget)->GetValue() / (float)DEFAULT_SLIDER_MAX;

		pWidget = (MSlider*)pResource->FindWidget("BGMVolumeSlider");
		if(pWidget)
		{
			Z_AUDIO_BGM_VOLUME = (float) ((MSlider*)pWidget)->GetValue() / (float)DEFAULT_SLIDER_MAX;
			ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME) ;
		}

		pWidget = (MSlider*)pResource->FindWidget("EffectVolumeSlider");
		if(pWidget)
		{
			Z_AUDIO_EFFECT_VOLUME = (float) ((MSlider*)pWidget)->GetValue() / (float)DEFAULT_SLIDER_MAX;

			ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
		}

	}


	int i=0;

	//for(i=0; i<ZACTION_COUNT; i++) {
	//	ZGetInput()->UnregisterActionKey(i);
	//}

	// 모두 클리어후 재등록
	ZGetInput()->ClearActionKey();

	for(i=0; i<ZACTION_COUNT; i++){
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);
		ZActionKey* pWidget = (ZActionKey*)pResource->FindWidget(szItemName);
		if(pWidget==NULL) continue;
		int nKey = 0;
		pWidget->GetActionKey(&nKey);
		//		Mint::GetInstance()->UnregisterActionKey(i);
		//		Mint::GetInstance()->RegisterActionKey(i, nKey);	// 키 등록

//		ZGetInput()->UnregisterActionKey(i);
		ZGetInput()->RegisterActionKey(i,nKey);

		ZVIRTUALKEY altKey;
		pWidget->GetActionAltKey(&altKey);
		if(altKey!=-1)
			ZGetInput()->RegisterActionKey(i,altKey);

		// ZConfiguration으로 옵션 저장
		ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey = nKey;
		ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKeyAlt = altKey;
	}

	// 새 키맵후 모든 키상태를 release로 셋팅
	ZGetInput()->OffActionKeys();

	/*
	int nCnt[ZACTION_COUNT];

	for(i=0; i<ZACTION_COUNT; i++)
	{
		nCnt[i] = ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey;
	}
	*/

	{

		Z_VIDEO_WIDTH = RGetScreenWidth();
		Z_VIDEO_HEIGHT = RGetScreenHeight();
		Z_VIDEO_FULLSCREEN	= RIsFullScreen();
		Z_VIDEO_BPP	= RGetPixelFormat()==D3DFMT_X8R8G8B8 ? 32:16 ;

		MComboBox*	pWidget = (MComboBox*)pResource->FindWidget("CharTexLevel");

		int TexLevel = 0;
		DWORD flag = 0;
		int EffectLevel = 0;
		int nTextureFormat = 0;

		if(pWidget)	{

			TexLevel = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->bTerrible ){
				ZGetConfiguration()->GetVideo()->nCharTexLevel = TexLevel;
				if( TexLevel == 2 )
					SetObjectTextureLevel(TexLevel+2);
				else
					SetObjectTextureLevel(TexLevel);

				flag |= RTextureType_Object;
			}
			else if( ZGetConfiguration()->GetVideo()->nCharTexLevel != TexLevel ) {
				ZGetConfiguration()->GetVideo()->nCharTexLevel = TexLevel;
				SetObjectTextureLevel(TexLevel);
				flag |= RTextureType_Object;
			}
		}

        pWidget = (MComboBox*)pResource->FindWidget("MapTexLevel");

        if(pWidget)    {

			TexLevel = pWidget->GetSelIndex();

			if (ZGetConfiguration()->GetVideo()->bTerrible) {
				ZGetConfiguration()->GetVideo()->nCharTexLevel = TexLevel;
				if (TexLevel == 2)
					SetObjectTextureLevel(TexLevel + 2);
				else
					SetObjectTextureLevel(TexLevel);

				flag |= RTextureType_Object;
			}
			if (ZGetConfiguration()->GetVideo()->nMapTexLevel != TexLevel)
			{
				ZGetConfiguration()->GetVideo()->nMapTexLevel = TexLevel;
				SetMapTextureLevel(TexLevel);
				flag |= RTextureType_Map;
			}
        }

		pWidget = (MComboBox*)pResource->FindWidget("EffectLevel");

		if(pWidget)	{

			EffectLevel = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->nEffectLevel != EffectLevel ) {
				ZGetConfiguration()->GetVideo()->nEffectLevel = EffectLevel;
				SetEffectLevel(EffectLevel);
				//				flag |= RTextureType_Map;
			}
		}

		pWidget = (MComboBox*)pResource->FindWidget("TextureFormat");

		if(pWidget)	{

			nTextureFormat = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->nTextureFormat != nTextureFormat ) {
				ZGetConfiguration()->GetVideo()->nTextureFormat = nTextureFormat;
				SetTextureFormat(nTextureFormat);
				flag = RTextureType_All;
			}
		}

		if(flag) {
			RChangeBaseTextureLevel(flag);
		}
	}
	{ // 동영상 캡쳐 20081017... by kam
		MComboBox*	pWidget = (MComboBox*)pResource->FindWidget("MovingPictureResolution");
		int iResolution = 0;
		if(pWidget)	{
			iResolution = pWidget->GetSelIndex();
			ZGetConfiguration()->GetMovingPicture()->iResolution = iResolution;	// configuration에 세팅한다.
			SetBandiCaptureConfig(iResolution);									// 반디캡처 동영상 해상도에 적용한다
		}
		pWidget = (MComboBox*)pResource->FindWidget("MovingPictureFileSize");
		int iFileSize = 0;
		if(pWidget)	{
			iFileSize = pWidget->GetSelIndex();
			ZGetConfiguration()->GetMovingPicture()->iFileSize = iFileSize;	// configuration에 세팅한다.
			SetBandiCaptureFileSize(iFileSize);								// 반디캡처 동영상 파일크기 세팅에 적용한다
		}
	}
	{	// 언어선택
		MComboBox*	pWidget = (MComboBox*)pResource->FindWidget("LanguageSelectComboBox");
		if(pWidget)	
		{
			ZGetConfiguration()->SetSelectedLanguageIndex( pWidget->GetSelIndex());
		}

		pWidget = (MComboBox*)pResource->FindWidget("CustomMusic");
		if (pWidget)
		{
			ZGetConfiguration()->GetAudio()->nCustomMusicFolders = pWidget->GetSelIndex();
		}

	}
	{


		MButton* pWidget = (MButton*)pResource->FindWidget("Reflection");
		if(pWidget)
		{
			Z_VIDEO_REFLECTION = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("LightMap");
		if(pWidget)
		{
			Z_VIDEO_LIGHTMAP = pWidget->GetCheck();

			if(ZGetGame()) {
				ZGetGame()->GetWorld()->GetBsp()->LightMapOnOff(Z_VIDEO_LIGHTMAP);
			}
			else {
				RBspObject::SetDrawLightMap(Z_VIDEO_LIGHTMAP);
			}
		}

		pWidget = (MButton*)pResource->FindWidget("DynamicLight");
		if(pWidget)
		{
			Z_VIDEO_DYNAMICLIGHT = pWidget->GetCheck();
		}


		pWidget = (MButton*)pResource->FindWidget("HPAP");
		if(pWidget)
		{
			Z_VIDEO_HPAP = pWidget->GetCheck();
		
				if (Z_VIDEO_HPAP)
					ZGetConfiguration()->GetVideo()->bHPAP = true;
				else
					ZGetConfiguration()->GetVideo()->bHPAP = false;
		}



		pWidget = (MButton*)pResource->FindWidget("Shader");
		if(pWidget)
		{
			Z_VIDEO_SHADER = pWidget->GetCheck();

			if( Z_VIDEO_SHADER )
			{
				RGetShaderMgr()->SetEnable();
			}
			else
			{
				RGetShaderMgr()->SetDisable();
			}
			//*/
		}

		pWidget = (MButton*)pResource->FindWidget("NewRenderer");
		if (pWidget)
		{
			ZGetConfiguration()->GetVideo()->bNewRenderer = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("D3D9EX");
		if (pWidget) {
			bool isEnabledD3D9Ex = pWidget->GetCheck();
			if (Z_VIDEO_D3D9EX != isEnabledD3D9Ex) {
				ZApplication::GetGameInterface()->ShowMessage("Please restart the client for the changes to take effect.");
				Z_VIDEO_D3D9EX = pWidget->GetCheck();
			}
		}

		pWidget	= (MButton*)pResource->FindWidget("BGMMute");
		if( pWidget )
		{
			Z_AUDIO_BGM_MUTE	= !(pWidget->GetCheck());

			ZGetSoundEngine()->SetMusicMute( Z_AUDIO_BGM_MUTE );
		}
		pWidget	= (MButton*)pResource->FindWidget("EffectMute");
		if(pWidget)
		{
			Z_AUDIO_EFFECT_MUTE = !(pWidget->GetCheck());
			ZGetSoundEngine()->SetEffectMute( Z_AUDIO_EFFECT_MUTE );

		}
		pWidget	= (MButton*)pResource->FindWidget("Effect3D");
		if(pWidget)
		{
			Z_AUDIO_3D_SOUND = pWidget->GetCheck();
			ZGetSoundEngine()->Set3DSound( Z_AUDIO_3D_SOUND );
		}

		pWidget = (MButton*)pResource->FindWidget("EnableCustomMusic");
		if (pWidget)
		{
			ZGetConfiguration()->GetAudio()->bCustomMusicEnabled = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("8BitSound");
		if(pWidget)
		{
			Z_AUDIO_8BITSOUND = pWidget->GetCheck();
#ifdef _BIRDSOUND

#else
			ZGetSoundEngine()->SetSamplingBits(Z_AUDIO_8BITSOUND);
#endif
		}
		pWidget = (MButton*)pResource->FindWidget("InverseSound");
		if(pWidget)
		{
			Z_AUDIO_INVERSE = pWidget->GetCheck();
#ifdef _BIRDSOUND

#else
			ZGetSoundEngine()->SetInverseSound( Z_AUDIO_INVERSE );
#endif
		}
		pWidget = (MButton*)pResource->FindWidget("HWMixing");
		if(pWidget)
		{
			Z_AUDIO_HWMIXING = pWidget->GetCheck();
			if (ZGetSoundEngine()->m_bHWMixing != Z_AUDIO_HWMIXING)
#ifdef _BIRDSOUND

#else
				ZGetSoundEngine()->Reset(g_hWnd, Z_AUDIO_HWMIXING, Z_AUDIO_BACKGROUNDAUDIO);
#endif
		}
		pWidget = (MButton*)pResource->FindWidget("HitSound");
		if(pWidget)
		{
			Z_AUDIO_HITSOUND = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("NarrationSound");
		if(pWidget)
		{
			Z_AUDIO_NARRATIONSOUND = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("BackGroundMusic");
		if (pWidget)
		{
			Z_AUDIO_BACKGROUNDAUDIO = pWidget->GetCheck();
			ZGetSoundEngine()->Reset(g_hWnd, Z_AUDIO_HWMIXING, Z_AUDIO_BACKGROUNDAUDIO);
		}
		pWidget = (MButton*)pResource->FindWidget("InvertMouse");
		if(pWidget)
		{
			Z_MOUSE_INVERT = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("InvertJoystick");
		if(pWidget)
		{
			Z_JOYSTICK_INVERT = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("TrailOption");
		if(pWidget)
		{
			Z_ETC_TRAIL_OPTION = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("HPAP");
		if( pWidget )
		{
			Z_VIDEO_HPAP = pWidget->GetCheck();
		}
	}
	{	// Etc
		MEdit* pEdit = (MEdit*)pResource->FindWidget("NetworkPort1");
		if (pEdit)
		{
			int nPreviousPort = Z_ETC_NETWORKPORT1;
			Z_ETC_NETWORKPORT1 = atoi(pEdit->GetText());

		}

		pEdit = (MEdit*)pResource->FindWidget("NetworkPort2");
		if (pEdit)
		{
			int nPreviousPort = Z_ETC_NETWORKPORT2;
			Z_ETC_NETWORKPORT2 = atoi(pEdit->GetText());
		}

		MButton* pBoost = (MButton*)pResource->FindWidget("BoostOption");
		if(pBoost)
		{
			if (Z_ETC_BOOST != pBoost->GetCheck()) {
				Z_ETC_BOOST = pBoost->GetCheck();
				if (Z_ETC_BOOST)
					ZGetGameClient()->PriorityBoost(true);
				else
					ZGetGameClient()->PriorityBoost(false);
			}
		}


		MButton* pNormalChat = (MButton*)pResource->FindWidget("NormalChatOption");
		if(pNormalChat)
		{
			if (Z_ETC_REJECT_NORMALCHAT != pNormalChat->GetCheck()) {
				Z_ETC_REJECT_NORMALCHAT = pNormalChat->GetCheck();
				if (Z_ETC_REJECT_NORMALCHAT)
					ZGetGameClient()->SetRejectNormalChat(true);
				else
					ZGetGameClient()->SetRejectNormalChat(false);
			}
		}

		MButton* pTeamChat = (MButton*)pResource->FindWidget("TeamChatOption");
		if(pTeamChat)
		{
			if (Z_ETC_REJECT_TEAMCHAT != pTeamChat->GetCheck()) {
				Z_ETC_REJECT_TEAMCHAT = pTeamChat->GetCheck();
				if (Z_ETC_REJECT_TEAMCHAT)
					ZGetGameClient()->SetRejectTeamChat(true);
				else
					ZGetGameClient()->SetRejectTeamChat(false);
			}
		}

		MButton* pClanChat = (MButton*)pResource->FindWidget("ClanChatOption");
		if(pClanChat)
		{
			if (Z_ETC_REJECT_CLANCHAT != pClanChat->GetCheck()) {
				Z_ETC_REJECT_CLANCHAT = pClanChat->GetCheck();
				if (Z_ETC_REJECT_CLANCHAT)
					ZGetGameClient()->SetRejectClanChat(true);
				else
					ZGetGameClient()->SetRejectClanChat(false);
			}
		}

		MButton* pWhisper = (MButton*)pResource->FindWidget("WhisperOption");
		if(pWhisper)
		{
			if (Z_ETC_REJECT_WHISPER != pWhisper->GetCheck()) {
				Z_ETC_REJECT_WHISPER = pWhisper->GetCheck();
				if (Z_ETC_REJECT_WHISPER)
					ZGetGameClient()->SetRejectWhisper(true);
				else
					ZGetGameClient()->SetRejectWhisper(false);
				ZPostUserOption();
			}
		}

		MButton* pInvite = (MButton*)pResource->FindWidget("InviteOption");
		if(pInvite)
		{
			if (Z_ETC_REJECT_INVITE != pInvite->GetCheck()) {
				Z_ETC_REJECT_INVITE = pInvite->GetCheck();
				if (Z_ETC_REJECT_INVITE)
					ZGetGameClient()->SetRejectInvite(true);
				else
					ZGetGameClient()->SetRejectInvite(false);
				ZPostUserOption();
			}
		}

		MButton* pScroll = (MButton*)pResource->FindWidget("DisableSWScroll");
		if (pScroll)
		{
			if (Z_ETC_SW_SCROLL != pScroll->GetCheck())
			{
				Z_ETC_SW_SCROLL = pScroll->GetCheck();
			}
			if (Z_ETC_SW_SCROLL)
				ZGetConfiguration()->GetEtc()->bDisableSW = true;
			else
				ZGetConfiguration()->GetEtc()->bDisableSW = false;
		}

		MButton* pWidget2 = (MButton*)pResource->FindWidget("Archetype");
		if (pWidget2)
		{
			Z_VIDEO_ARCHETYPE = pWidget2->GetCheck();
			if (Z_VIDEO_ARCHETYPE)
			{
				SetMapTextureLevel(8);
				RChangeBaseTextureLevel(RTextureType_Map);
			}
			else
			{
				SetMapTextureLevel(ZGetConfiguration()->GetVideo()->nMapTexLevel);
				RChangeBaseTextureLevel(RTextureType_Map);
			}
		}

		MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("CrossHairComboBox");
		if (pComboBox)
		{
			Z_ETC_CROSSHAIR = pComboBox->GetSelIndex();
		}

		pComboBox = (MComboBox*)pResource->FindWidget("FrameLimit_PerSecond");
		if (pComboBox)
		{
			Z_ETC_FRAMELIMIT_PERSECOND = pComboBox->GetSelIndex();
			RSetFrameLimitPerSeceond(Z_ETC_FRAMELIMIT_PERSECOND);
		}


		pComboBox = (MComboBox*)pResource->FindWidget("AntiAlias");
		if (pComboBox)
		{
			if (Z_VIDEO_ANTIALIAS != pComboBox->GetSelIndex())
			{
				Z_VIDEO_ANTIALIAS = pComboBox->GetSelIndex();
				bResetDevice = true;
			}
		}


		MButton* fullScreen = (MButton*)pResource->FindWidget("FullScreen");
		if (fullScreen)
		{
			if (Z_VIDEO_FULLSCREEN != fullScreen->GetCheck())
			{
				Z_VIDEO_FULLSCREEN = fullScreen->GetCheck();
				bResetDevice = true;
			}

		}
	}

	{
		// Macro

		static char stemp_str[ZCONFIG_MACRO_MAX][80] = {
			"MacroF1",
			"MacroF2",
			"MacroF3",
			"MacroF4",
			"MacroF5",
			"MacroF6",
			"MacroF7",
			"MacroF8",
		};

		ZCONFIG_MACRO* pMacro = ZGetConfiguration()->GetMacro();

		if(pMacro) {

			MEdit* pEdit = NULL;

			for(int i=0;i<ZCONFIG_MACRO_MAX;i++) {

				pEdit = (MEdit*) pResource->FindWidget(stemp_str[i]);

				if (pEdit) {

					pMacro->SetString(i, (char*)pEdit->GetText());
				}
			}
		}

	}

	// 감마값 저장
	MSlider* pSlider = (MSlider*)pResource->FindWidget("VideoGamma");
	if (pSlider != NULL) 
	{
		Z_VIDEO_GAMMA_VALUE = pSlider->GetValue();
	}

	if (bResetDevice)
	{
		RMODEPARAMS ModeParams;
	
		ModeParams = { RGetScreenWidth(), RGetScreenHeight(), Z_VIDEO_FULLSCREEN, RGetPixelFormat() };

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
		RResetDevice(&ModeParams);
	}

	ZGetConfiguration()->Save(FILENAME_CONFIG, Z_LOCALE_XML_HEADER);

	return true;
}



void ZOptionInterface::ShowResizeConfirmDialog( bool Resized )
{
	if( Resized )
	{
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("ViewConfirm");
		if(pWidget!= 0)
			pWidget->Show( true, true );
	}
	else
	{
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("ResizeConfirm");
		if(pWidget!= 0)
			pWidget->Show( true, true );
	}
}

bool ZOptionInterface::SetTimer( bool b, float time /* = 0.f  */ )
{
	static DWORD DeadTime = 0;

	if( !b )
	{
		mbTimer = b;
		return false;
	}

	if( !mbTimer )
	{
		mTimerTime	= timeGetTime();
		mbTimer		= true;
		DeadTime		= time*1000;
	}

	if(( timeGetTime() - mTimerTime ) > DeadTime )
	{
		DeadTime = 0;
		mbTimer	= false;
		return true;
	}
	else
	{
		char szBuf[128];
		sprintf(szBuf, "%d", min(max( (10 - (int)(( timeGetTime() - mTimerTime ) * 0.001)),0),10));

		char szText[ 128];
		ZTransMsg( szText, MSG_BACKTOTHEPREV, 1, szBuf);

		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MLabel* Countdown = (MLabel*)pResource->FindWidget( "ViewConfirm_CountDown" );
		if ( Countdown)
			Countdown->SetText( szText);
	}
	return false;
}

void ZOptionInterface::ShowNetworkPortConfirmDialog()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("NetworkPortConfirm");
	if(pWidget!= 0) pWidget->Show( true, true );
}

bool ZOptionInterface::IsDiffNetworkPort()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	int nCurrPort = ntohs( ZGetGameClient()->GetSafeUDP()->GetLocalPort() );

	int nNewPort1, nNewPort2;

	MEdit* pEdit1 = (MEdit*)pResource->FindWidget("NetworkPort1");
	if ( pEdit1)
		nNewPort1 = atoi( pEdit1->GetText());
	else
		return false;


	MEdit* pEdit2 = (MEdit*)pResource->FindWidget("NetworkPort2");
	if ( pEdit2)
		nNewPort2 = atoi( pEdit2->GetText());
	else
		return false;


	if ( nNewPort1 > nNewPort2)
	{
		char szStr[ 25];
		itoa( Z_ETC_NETWORKPORT1, szStr, 10);
		pEdit1->SetText( szStr);
		itoa( Z_ETC_NETWORKPORT2, szStr, 10);
		pEdit2->SetText( szStr);

		return false;
	}


	if ( ( nNewPort1 != Z_ETC_NETWORKPORT1) || ( nNewPort2 != Z_ETC_NETWORKPORT2))
		return true;

	return false;
}

void ZOptionInterface::OptimizationVideoOption()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MButton* pButton = 0;
	MComboBox* pCombo = 0;
	MLabel* pLabel = 0;

	ZGetConfiguration()->SetForceOptimization( true );

	if(!RIsHardwareTNL())
	{	
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 16 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("LightMap");
		if(pButton!=0) pButton->SetCheck(false);		
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("HPAP");
		if(pButton!=0) pButton->SetCheck(false);

		pCombo	= (MComboBox*)pResource->FindWidget("ScreenResolution");
		if( pCombo != 0)
		{
			D3DDISPLAYMODE ddm;
			ddm.Width = 640;
			ddm.Height = 480;
			ddm.Format = D3DFMT_R5G6B5;
			ddm.RefreshRate = DEFAULT_REFRESHRATE;
			map<int, D3DDISPLAYMODE>::iterator iter_ = find_if( gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
			if( iter_ != gDisplayMode.end() )
			{
				int n = iter_->first;
				pCombo->SetSelIndex( n );
			}
		}

		ZGetConfiguration()->GetVideo()->bTerrible = true;
		return;
	}	

	ZGetConfiguration()->GetVideo()->bTerrible = false;

	int nVMem = RGetApproxVMem() /1024 /1024;
	if( nVMem < 32 )
	{		
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 16 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(false);		
	}
	else if( nVMem < 64 )
	{
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // 보통
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // 나쁨
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // 보통
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 16 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(true);
	}
	else // nVMem > 64
	{
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // 보통
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // 보통
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 좋음
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(1); // 32 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(true);		
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(true);
		pButton = (MButton*)pResource->FindWidget("HPAP");
		if(pButton!=0) pButton->SetCheck(true);
	}

	//pLabel = (MLabel*)pResource->FindWidget("Lightmap Label");
	//if(pLabel!=0) pLabel->SetTextColor(MCOLOR(64,64,64));
	pButton = (MButton*)pResource->FindWidget("LightMap");
	if(pButton!=0) {
		pButton->SetCheck(true);		
		//pButton->Enable(false);
	}

	if(RIsSupportVS())
	{
		pButton = (MButton*)pResource->FindWidget("Shader");
		if(pButton!=0) pButton->SetCheck(true);
	}
	else
	{		
		pButton = (MButton*)pResource->FindWidget("Shader");
		if(pButton!=0) pButton->SetCheck(false);
	}
}

bool ZOptionInterface::ResizeWidgetRecursive( MWidget* pWidget/*, int w, int h*/)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	//MWidget* pWidget = pResource->FindWidget(szName);
	if(pWidget==NULL) return false;
	int n = pWidget->GetChildCount();
	for( int i = 0; i < n; ++i)
	{
		MWidget* pChildWidget = pWidget->GetChild(i);
		ResizeWidgetRecursive( pChildWidget/*, w, h */);
	}

	// idl에서 읽어서 적절한 값을 가지고 있는 위젯이라면 그값으로 리사이즈한다
	if(pWidget->GetIDLRect().w>0 && pWidget->GetIDLRect().h>0)	
	{
		// idl 에서는 800x600 기준으로 기술되어있다
		const float tempWidth = ((float)RGetScreenWidth()) / 800;
		const float tempHeight = ((float)RGetScreenHeight()) / 600;

		MRECT r=pWidget->GetIDLRect();
		r.x*=tempWidth;
		r.w*=tempWidth;
		r.y*=tempHeight;
		r.h*=tempHeight;
		pWidget->SetBounds(r);
	}
	else 
	{
		const float tempWidth = ((float)RGetScreenWidth()) / mOldScreenWidth;
		const float tempHeight = ((float)RGetScreenHeight()) / mOldScreenHeight;
		MPOINT p = pWidget->GetPosition();
		p.Scale( tempWidth, tempHeight );
		pWidget->SetPosition( p );
		MRECT r=pWidget->GetRect();
		pWidget->SetSize( r.w*tempWidth, r.h*tempHeight );
	}
	return true;
}

void ZOptionInterface::AdjustMultipliedWidgetsManually()
{
	int w = RGetScreenWidth();
	int h = RGetScreenHeight();

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget;
	
	pWidget = pResource->FindWidget("Login");
	if (pWidget) {
		// Resize frame
		pWidget->SetSize( w, h);

		// Resize background image
		pWidget = pResource->FindWidget( "Login_BackgrdImg");
		if ( pWidget)
			pWidget->SetSize( w, h);

		// Reposition login frame
		pWidget = pResource->FindWidget( "LoginFrame");
		if ( pWidget)
		{
			MRECT rect;
			rect = pWidget->GetRect();

			//jintriple3 해상도에 따른 서버 선택 창 위치 중앙에서 약간 아래로...
			/*	rect.x = (w / 2) - (rect.w / 2) + 5;
			rect.y = h - rect.h - 10;
			*/
			rect.x = (w / 2) - (rect.w / 2) + 5;
			rect.y = (int)((float)h * 0.555f);
			if( rect.h + rect.y > h )
				rect.y = h - rect.h - 10;
			pWidget->SetBounds( rect);
		}

		// REposition connecting message
		pWidget = pResource->FindWidget( "Login_ConnectingMsg");
		if ( pWidget)
		{
			MRECT rect;
			rect = pWidget->GetRect();
			rect.x = 0;
			rect.y = (int)(h * 0.66f);
			rect.w = w;
			pWidget->SetBounds( rect);
		}
	}

	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList( NULL, true);

	pWidget = pResource->FindWidget("Stage");
	if (pWidget) {
		ZApplication::GetStageInterface()->GetSacrificeItemBoxPos();
		ZApplication::GetStageInterface()->SetRelayMapBoxPos(0);
	}

	pWidget = pResource->FindWidget("Lobby");
	if (pWidget) {
		ZRoomListBox* pRoomList;

		pRoomList = (ZRoomListBox*)pResource->FindWidget( "Lobby_StageList" );
		if( pRoomList != 0 )
		{
			const float tempWidth = ((float)RGetScreenWidth()) / mOldScreenWidth;
			const float tempHeight = ((float)RGetScreenHeight()) / mOldScreenHeight;
			pRoomList->Resize( tempWidth, tempHeight );
		}
	}

	pWidget = pResource->FindWidget("CharCreation");
	if (pWidget) {
		MRECT rect;
		rect = pWidget->GetRect();
		rect.x = 50 * ( RGetScreenWidth() / 800.0f);
		rect.y = (int)((RGetScreenHeight() - rect.h) / 2.0f);
		pWidget->SetBounds( rect);
	}

	pWidget = pResource->FindWidget("CombatDTInfo");
	if (pWidget) {

	}
/*
	else if(_stricmp( szName, "CombatDTInfo")==0)
		ResizeWidgetRecursive( pWidget, w, h );
	else if(_stricmp( szName, "CombatDT_CharacterInfo")==0)
		ResizeWidgetRecursive( pWidget, w, h );
	*/

	pWidget = pResource->FindWidget("BuyItemDetailFrame_Thumbnail");
	if (pWidget) {	// 이 썸네일 이미지는 와이드 해상도에서도 정사각형으로 보이도록 조정
		MRECT rc = pWidget->GetRect();
		rc.h = rc.w;
		pWidget->SetBounds(rc);
	}
}

extern MFontR2* g_pDefFont;
void ZOptionInterface::ResizeDefaultFont( int newScreenHeight )
{
	// 폰트를 기준 해상도(800*600)에서 몇배할 것인지 계산
	float fontResizeRatio = newScreenHeight/600.f;

	// 디폴트 폰트 크기 변경
	g_pDefFont->Destroy();

	int newFontHeight = (int)(DEFAULT_FONT_HEIGHT * fontResizeRatio + 0.5f);	//UI작성시 기준해상도가 800*600이므로 600에 대해 계산
	if (newFontHeight < FONT_MINIMUM_HEIGHT)
		newFontHeight = FONT_MINIMUM_HEIGHT;	// 640*480에서 너무 뭉개지지 않게

	if (!g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, newFontHeight, 1.0f))
	{
		mlog("Fail to Recreate default font : MFontR2 / screen resize\n" );
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
	}

	// UI에서 로딩된 폰트들 크기 변경
	MFontManager::Resize( fontResizeRatio, FONT_MINIMUM_HEIGHT ); //원래 크기(800*600기준UI)에 대해서 몇배율로 변화해야하는지를 알려줌
}

void ZOptionInterface::Resize(int w, int h)
{
	ResizeDefaultFont(h);
	// ??? ??? ???? ??? ?????
	if (mOldScreenHeight == 0) mOldScreenHeight = 600;
	if (mOldScreenWidth == 0) mOldScreenWidth = 800;

	ZGetGameInterface()->MultiplySize(w/800.f, h/600.f, w/float(mOldScreenWidth), h/float(mOldScreenHeight));

	AdjustMultipliedWidgetsManually();	// 배율적용하고나서 직접 매만져줘야 하는 부분들

	ZGetGameInterface()->SetSize(w, h);

	if (ZGetCombatInterface()) ZGetCombatInterface()->Resize(w, h);

	ZCamera* pCamera = ZGetGameInterface()->GetCamera();
	if (pCamera)
		pCamera->AdjustDist();
}

void ZOptionInterface::GetOldScreenResolution()
{
	RMODEPARAMS ModeParams;
	ModeParams.nWidth	= mOldScreenWidth;
	ModeParams.nHeight	= mOldScreenHeight;
	ModeParams.bFullScreen	= RIsFullScreen();
	ModeParams.PixelFormat	= mnOldBpp;

	//jintriple3 해상도 변경 후 프리 뷰에서 캔슬하면 게임 룸 리스트의 크기가 이상해지는 버그 수정.
	mOldScreenWidth = RGetScreenWidth();
	mOldScreenHeight = RGetScreenHeight();
	mnOldBpp = RGetPixelFormat();

	RResetDevice( &ModeParams );
	Mint::GetInstance()->SetWorkspaceSize(ModeParams.nWidth, ModeParams.nHeight);
	Mint::GetInstance()->GetMainFrame()->SetSize(ModeParams.nWidth, ModeParams.nHeight);
	Resize(ModeParams.nWidth, ModeParams.nHeight);

	D3DDISPLAYMODE ddm;
	ddm.Width = ModeParams.nWidth;
	ddm.Height = ModeParams.nHeight;
	ddm.Format = ModeParams.PixelFormat;
	ddm.RefreshRate = DEFAULT_REFRESHRATE;

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MComboBox *pWidget = (MComboBox*)pResource->FindWidget( "ScreenResolution" );

	map<int, D3DDISPLAYMODE>::iterator  iter = find_if( gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
	if( iter != gDisplayMode.end() )
		pWidget->SetSelIndex( iter->first );

	ZGetScreenEffectManager()->SetOffSet();
}

bool ZOptionInterface::IsDiffScreenResolution()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MComboBox *pWidget = (MComboBox*)pResource->FindWidget("ScreenResolution");
	if(pWidget)
	{
		int nSel = pWidget->GetSelIndex();
		D3DDISPLAYMODE ddm = (gDisplayMode.find( nSel ))->second;
		if( ddm.Width == RGetScreenWidth() && ddm.Height == RGetScreenHeight() && ddm.Format == RGetPixelFormat() )
			return false;
	}
	return true;
}

bool ZOptionInterface::TestScreenResolution()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MComboBox *pWidget = (MComboBox*)pResource->FindWidget("ScreenResolution");
	if(pWidget)
	{
		RMODEPARAMS	ModeParams;

		map<int, D3DDISPLAYMODE>::iterator iter = gDisplayMode.find( pWidget->GetSelIndex() );
		if( iter == gDisplayMode.end())
		{
			mlog("선택한 해상도가 존재하지 않아서 해상도 변경에 실패하였습니다..\n" );
			return false;
		}

		D3DDISPLAYMODE ddm = iter->second;

		mOldScreenWidth	= RGetScreenWidth();
		mOldScreenHeight	= RGetScreenHeight();
		mnOldBpp				= RGetPixelFormat();

		ModeParams.nWidth = ddm.Width;
		ModeParams.nHeight = ddm.Height;
		ModeParams.bFullScreen = RIsFullScreen();
		ModeParams.PixelFormat = ddm.Format;

		RResetDevice(&ModeParams);

		Mint::GetInstance()->SetWorkspaceSize(ModeParams.nWidth, ModeParams.nHeight);
		Mint::GetInstance()->GetMainFrame()->SetSize(ModeParams.nWidth, ModeParams.nHeight);
		Resize(ModeParams.nWidth, ModeParams.nHeight);
	}
	return true;
}

void ZOptionInterface::Update()
{
	if (mbTimer)
	{
		if (SetTimer(true))
		{
			GetOldScreenResolution();
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MWidget* pWidget = pResource->FindWidget("ViewConfirm");
			if (pWidget != 0) pWidget->Show(false);
		}
	}
}

void ZOptionInterface::OnUpdate()
{
	if (CustomMusic::m_customMusic.size() == 0)
	{
		return;
	}
	///Custom: Updates mp3 player if visible
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (!pResource)
		return;
	MWidget* pWidet = (MWidget*)pResource->FindWidget("AudioOptionGroup2");
	if (!pWidet)
		return;
	if (pWidet->IsVisible())
	{
		MTextArea* pTextArea = (MTextArea*)pResource->FindWidget("CurrentSong");
		if (pTextArea && CustomMusic::m_customMusic.size() > 0 && ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
		{
			char textArea[512];
			pTextArea->GetText(textArea, sizeof(textArea));
			if (_stricmp(textArea, CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].at(CustomMusic::g_NextMusicIndex).c_str()) != 0) {
				string baseText = "Now Playing : ";
				string songName = CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].at(CustomMusic::g_NextMusicIndex).c_str();
				baseText.append(songName.substr(songName.find_last_of("\\") + 1));
				baseText.erase(baseText.length() - 4);
				pTextArea->SetText(baseText.c_str());
				pTextArea->SetTextColor(MCOLOR(ZCOLOR_CHAT_SYSTEM));
			}
		}
		///TODO: get this logic correct
		MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("CustomMusic");
		if (pComboBox)
		{
			if (CustomMusic::m_customMusic.size() > 0 && ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
			{
				if (ZGetConfiguration()->GetAudio()->nCustomMusicFolders != pComboBox->GetSelIndex())
				{
					ZGetConfiguration()->GetAudio()->nCustomMusicFolders = pComboBox->GetSelIndex();
					if (ZGetConfiguration()->GetAudio()->nCustomMusicFolders == -1)
						ZGetConfiguration()->GetAudio()->nCustomMusicFolders = 0;
					//Prevnet a crash when changing the folder
					CustomMusic::g_NextMusicIndex = 0;

					ZGetSoundEngine()->StopMusic();
					ZGetSoundEngine()->CloseMusic();
					ZGetSoundEngine()->OpenMusic(0, ZApplication::GetFileSystem());
					ZGetSoundEngine()->PlayMusic(false);
					pComboBox = (MComboBox*)pResource->FindWidget("CustomMusicList");
					if (pComboBox)
					{
						pComboBox->RemoveAll();

						for (int i = 0; i < CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].size(); ++i)
						{
							string songName, baseText;

							songName = CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].at(i).c_str();
							baseText.append(songName.substr(songName.find_last_of("\\") + 1));
							pComboBox->Add(baseText.c_str());
						}
						pComboBox->SetSelIndex(CustomMusic::g_NextMusicIndex);
					}
				}
			}
			else
				pComboBox->SetSelIndex(0);
		}
		pComboBox = (MComboBox*)pResource->FindWidget("CustomMusicList");
		if (pComboBox)
		{
			if (CustomMusic::m_customMusic.size() > 0 && ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
			{
				if (pComboBox->GetSelIndex() != CustomMusic::g_NextMusicIndex)
				{
					pComboBox->SetSelIndex(CustomMusic::g_NextMusicIndex);
				}
			}
			else
				pComboBox->SetSelIndex(0);
		}
		MSlider* slider = (MSlider*)pResource->FindWidget("TrackSlider");
		if (slider)
		{
			unsigned int currentPosition, trackLength;
			ZGetSoundFMod()->GetTrackTime(currentPosition);
			ZGetSoundFMod()->GetTrackLength(trackLength);
			slider->SetMinMax(0, trackLength / 1000);
			slider->SetValue(currentPosition / 1000);
			MLabel* label = (MLabel*)pResource->FindWidget("TrackTime");
			if (label)
			{
				char labelText[256];
				strcpy_s(labelText, "Track Time - ");
				strcat(labelText, ZGetSoundFMod()->GetTrackTime());
				strcat(labelText, " / ");
				strcat(labelText, ZGetSoundFMod()->GetTrackLength());
				label->SetText(labelText);
			}
		}
	}
}

void ZOptionInterface::OnActionKeySet(ZActionKey* pActionKey, ZVIRTUALKEY key)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	for(int i=0; i<ZACTION_COUNT; i++){
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);
		ZActionKey* pWidget = (ZActionKey*)pResource->FindWidget(szItemName);
		if(pWidget==NULL) continue;
		if(pWidget==pActionKey) continue;

		if(pWidget->DeleteActionKey(key))
			pWidget->UpdateText();
	}
}








///////////////////// 이하 interface listener
BEGIN_IMPLEMENT_LISTENER(ZGetOptionFrameButtonListener, MBTN_CLK_MSG)
	// 옵션 프레임 보여주기
	ZGetOptionInterface()->InitInterfaceOption();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("Option");
	if (pWidget != nullptr)
	{
		pWidget->Show(true, true);
	}
#ifdef LOCALE_NHNUSA
	pWidget = pResource->FindWidget( "Option_Textarea01");
	if ( pWidget)		pWidget->Show( false);
	MButton* pButton = (MButton*)pResource->FindWidget( "BoostOption");
	if ( pButton)
	{
		pButton->SetCheck( false);
		pButton->Show( false);
	}
#endif

#ifndef _MULTILANGUAGE	// 다중언어지원 디파인을 껐을땐 언어선택 콤보박스를 숨겨버린다
	pWidget = (MLabel*)pResource->FindWidget( "LanguageSelectLabel");
	if ( pWidget) {
		pWidget->Show( false);
	}
	MComboBox* pCBType = (MComboBox*)pResource->FindWidget( "LanguageSelectComboBox");
	if ( pCBType) {
		pCBType->Show( false);
	}
#endif

END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetSaveOptionButtonListener, MBTN_CLK_MSG)
	// Save & Close
	/*
	ZApplication::GetGameInterface()->SaveInterfaceOption();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("Option");
	if(pWidget!=NULL) pWidget->Show(false);
	//*/

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if(pResource == nullptr)
		return false;

	if( pWidget->m_bEventAcceleratorCall ) {

		MTabCtrl* pTab = (MTabCtrl*)pResource->FindWidget("OptionTabControl");

		if(pTab) {//키보드 옵션은 키입력을 막아준다.
			if(pTab->GetSelIndex()==3)//이름을 찾아 조사
				return true;
		}
	}

	if( ZGetOptionInterface()->IsDiffNetworkPort() )
	{
		static bool bRestartAsk = false;
		if (bRestartAsk == false) {
			ZGetOptionInterface()->ShowNetworkPortConfirmDialog();
			bRestartAsk = true;
			return true;
		}
	}

	if( ZGetOptionInterface()->IsDiffScreenResolution() )
	{
		ZGetOptionInterface()->ShowResizeConfirmDialog( false );
	}
	else
	{
		bool bLanguageChanged = false;
		MComboBox* pLanguageComboBox = (MComboBox*)pResource->FindWidget("LanguageSelectComboBox");
		//MComboBox* pFontComboBox = (MComboBox*)pResource->FindWidget("FontSelectComboBox");
		if (pLanguageComboBox)
		{
			if (ZGetConfiguration()->GetSelectedLanguageIndex() != pLanguageComboBox->GetSelIndex())
				bLanguageChanged = true;
		}

		ZGetOptionInterface()->SaveInterfaceOption();
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("Option");
		if(pWidget!=NULL) pWidget->Show(false);

		if(bLanguageChanged)	// 재시작 확인 메세지 박스
		{
			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CONFIRM_RESTART_CHANGE_LANGUAGE), ZGetLanguageChangeConfirmListenter());
		}
	}

	if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
	{
		if (ZGetCombatInterface())
		{
			ZGetCombatInterface()->GetCrossHair()->ChangeFromOption();
		}
	}

END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCancelOptionButtonListener, MBTN_CLK_MSG)
//	ZApplication::GetGameInterface()->SaveInterfaceOption();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// TODO: 이게 필요한가 ? 테스트 요망
	if( pWidget->m_bEventAcceleratorCall ) {

		MTabCtrl* pTab = (MTabCtrl*)pResource->FindWidget("OptionTabControl");

		if(pTab) {//키보드 옵션은 키입력을 막아준다.
			if(pTab->GetSelIndex()==3)//이름을 찾아 조사
				return true;
		}
	}

	MWidget* pWidget = pResource->FindWidget("Option");
	
	if(pWidget!=NULL) pWidget->Show(false);

	// 원래 감마값으로 돌리기
	MSlider* pSlider = (MSlider*)pResource->FindWidget("VideoGamma");
	if (pSlider != NULL) 
	{
		if (pSlider->GetValue() != Z_VIDEO_GAMMA_VALUE)
		{
			RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
		}
	}

	// Close Only
//	ZApplication::GetGameInterface()->SetState(GUNZ_PREVIOUS);
END_IMPLEMENT_LISTENER()


///////////////////////////////////////////////////
/// control

BEGIN_IMPLEMENT_LISTENER( ZGetLoadDefaultKeySettingListener, MBTN_CLK_MSG)
	ZGetConfiguration()->LoadDefaultKeySetting( );
	for(int i=0; i<ZACTION_COUNT; i++)
	{
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);
		ZActionKey* pWidget = (ZActionKey*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szItemName);
		if(pWidget==NULL) continue;
		//unsigned int nKey = 0;
		//pWidget->GetActionKey(&nKey);
		//Mint::GetInstance()->UnregisterActionKey(i);
		//Mint::GetInstance()->RegisterActionKey(i, nKey);	// 키 등록
		//m_Keyboard.ActionKeys[i].nScanCode = nKey;	// 옵션 저장
		pWidget->ClearActionKey();
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey);
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKeyAlt);
	}
END_IMPLEMENT_LISTENER()


//////////////////////////////////////////////////
/// video

BEGIN_IMPLEMENT_LISTENER(ZGetOptionGammaSliderChangeListener, MLIST_VALUE_CHANGED)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("VideoGamma");
	if (pSlider != NULL) 
	{
		unsigned short nGamma = (unsigned short)pSlider->GetValue();
		if (nGamma < 50) nGamma = 50;
		RSetGammaRamp(nGamma);
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSongPositionListener,MLIST_VALUE_CHANGED)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("TrackSlider");
	if (pSlider != NULL)
	{
		unsigned int trackTime;
		ZGetSoundFMod()->GetTrackTime(trackTime);
		if ((int)trackTime / 1000 < pSlider->GetValue())
			ZGetSoundFMod()->FastForward(pSlider->GetValue() + (trackTime / 1000));
		else if ((int)trackTime / 1000 > pSlider->GetValue())
			ZGetSoundFMod()->Rewind((trackTime ) - (pSlider->GetValue() / 1000));
	}

END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetRequestResizeListener, MBTN_CLK_MSG )
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ResizeConfirm");
	if(pWidget!= 0) pWidget->Show( false );
	//해상도 변경후
	ZGetOptionInterface()->TestScreenResolution();
	ZGetOptionInterface()->SetTimer( true, 10 );
	ZGetOptionInterface()->ShowResizeConfirmDialog( true );
	ZGetScreenEffectManager()->SetOffSet();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetViewConfirmCancelListener, MBTN_CLK_MSG )
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ViewConfirm");
	if(pWidget!= 0) pWidget->Show( false );
	// 해상도 원래대로 변경
 	ZGetOptionInterface()->SetTimer( false );
	ZGetOptionInterface()->GetOldScreenResolution();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetViewConfrimAcceptListener, MBTN_CLK_MSG )

	ZGetOptionInterface()->SetTimer( false );

	ZGetOptionInterface()->SaveInterfaceOption();

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ViewConfirm");
	if(pWidget!= 0) pWidget->Show( false );
		
	pWidget = pResource->FindWidget("Option");
	if(pWidget!=NULL) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCancelResizeConfirmListener, MBTN_CLK_MSG)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ResizeConfirm");
	if(pWidget!=NULL) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZSetOptimizationListener, MBTN_CLK_MSG )
	ZGetOptionInterface()->OptimizationVideoOption();
END_IMPLEMENT_LISTENER()

//////////////////////////////////////////////////
/// sound

BEGIN_IMPLEMENT_LISTENER( ZGet8BitSoundListener, MBTN_CLK_MSG )
	MButton* pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("8BitSound");
	if(pWidget)
	{
		pWidget->SetCheck(true);
		pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("16BitSound");
		if(pWidget) pWidget->SetCheck(false);
	}
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER( ZGet16BitSoundListener, MBTN_CLK_MSG )
	MButton* pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("16BitSound");
	if(pWidget)
	{
		pWidget->SetCheck(true);
		pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("8BitSound");
		if(pWidget) pWidget->SetCheck(false);
	}
END_IMPLEMENT_LISTENER()


/////////////////////////////////////////////////
//// network
BEGIN_IMPLEMENT_LISTENER( ZGetNetworkPortChangeRestartListener, MBTN_CLK_MSG )
	ZGetOptionInterface()->SaveInterfaceOption();
	ZChangeGameState(GUNZ_SHUTDOWN);
	ZApplication* pApp = ZApplication::GetInstance();
	pApp->Exit();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetNetworkPortChangeCancelListener, MBTN_CLK_MSG )
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("NetworkPortConfirm");
	if(pWidget!= 0) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

/////////////////////////////////////////////////
//// mouse
static void SetMouseSensitivitySlider(int i)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if (pSlider) 
	{
		pSlider->SetValue(i);
	}
}
static void SetMouseSensitivityEdit(int i)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MEdit* pEdit = (MEdit*)pResource->FindWidget("MouseSensitivityEdit");
	if (pEdit)
	{
		char sz[1024] = "";
		sprintf(sz, "%d", i);
		pEdit->SetText(sz);
	}
}


BEGIN_IMPLEMENT_LISTENER( ZGetMouseSensitivitySliderListener, MLIST_VALUE_CHANGED)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if (pSlider != NULL) 
	{
		int i = ZGetConfiguration()->ValidateMouseSensitivityInInt( pSlider->GetValue());
		SetMouseSensitivityEdit(i);
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCustomMusicListListener, MCMBBOX_CHANGED)
	MComboBox* pComboBox = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("CustomMusicList");

	if (pComboBox)
	{
		if (CustomMusic::g_NextMusicIndex != pComboBox->GetSelIndex())
			CustomMusic::g_NextMusicIndex = pComboBox->GetSelIndex();
		ZGetSoundEngine()->StopMusic();
		ZGetSoundEngine()->OpenMusic(CustomMusic::g_NextMusicIndex, ZGetFileSystem());
		ZGetSoundEngine()->PlayMusic();

	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetEnableCustomMusicListener, MBTN_CLK_MSG)
{
	MButton* pButton = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("EnableCustomMusic");
	MComboBox* pComboBox = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("CustomMusicList");
	MComboBox* pMusicBox = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("CustomMusic");
	if (pButton)
	{
		if (pButton->GetCheck() != ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
		{
			ZGetConfiguration()->GetAudio()->bCustomMusicEnabled = pButton->GetCheck();
		}
		if (ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
		{
			if (pComboBox)
				pComboBox->RemoveAll();

			ZGetConfiguration()->GetAudio()->nCustomMusicFolders = 0;
			ZGetSoundEngine()->LoadCustomMusic();
			CustomMusic::g_NextMusicIndex = 0;
			if (pComboBox)
			{
				if (CustomMusic::m_customMusic.size() > 0)
				{
					for (int i = 0; i < CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].size(); ++i)
					{
						string songName, baseText;

						songName = CustomMusic::m_customMusic[ZGetConfiguration()->GetAudio()->nCustomMusicFolders].at(i).c_str();
						baseText.append(songName.substr(songName.find_last_of("\\") + 1));
						pComboBox->Add(baseText.c_str());
					}
					pComboBox->SetSelIndex(CustomMusic::g_NextMusicIndex);
					ZGetSoundEngine()->OpenMusic(CustomMusic::g_NextMusicIndex, ZGetFileSystem());
					ZGetSoundEngine()->PlayMusic();
				}
			}
			if (pMusicBox)
			{
				pMusicBox->RemoveAll();

				if (ZGetConfiguration()->GetAudio()->bCustomMusicEnabled)
				{
					if (CustomMusic::m_customMusic.size() > 0)
					{

						if (CustomMusic::folderName.size() <= 0)
							pMusicBox->Enable(false);

						for (auto itor = CustomMusic::folderName.begin(); itor != CustomMusic::folderName.end(); ++itor)
						{
							pMusicBox->Add(itor->c_str());
						}
						pMusicBox->SetSelIndex(ZGetConfiguration()->GetAudio()->nCustomMusicFolders);
					}
				}
				else
					pMusicBox->SetSelIndex(0);
			}
			else
				pMusicBox->SetSelIndex(0);
		}
		else
		{
			ZGetSoundEngine()->ClearCustomMusic();
			ZGetConfiguration()->GetAudio()->nCustomMusicFolders = 0;
			CustomMusic::g_NextMusicIndex = 0;
			MComboBox* pComboBox = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("CustomMusicList");

			if (pComboBox)
			{
				pComboBox->RemoveAll();
				ZGetSoundEngine()->OpenMusic(BGMID_BATTLE, ZGetFileSystem());
				ZGetSoundEngine()->PlayMusic();
				pComboBox->SetSelIndex(0);

			}
			if (pMusicBox)
			{
				pMusicBox->RemoveAll();
				pMusicBox->SetSelIndex(0);
			}
		}
	}
}
END_IMPLEMENT_LISTENER()

// 이 에디트박스에서 메시지 두개를 처리해야하기 때문에 BEGIN_IMPLEMENT_LISTENER/END_IMPLEMENT_LISTENER 매크로를 안썼음
MListener* ZGetMouseSensitivityEditListener(void){
	class ListenerClass : public MListener
	{
	public:
		virtual bool OnCommand(MWidget* pWidget, const char* szMessage){
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MEdit* pEdit= (MEdit*)pResource->FindWidget("MouseSensitivityEdit");
			{
				int i = atoi( pEdit->GetText());
				int v = ZGetConfiguration()->ValidateMouseSensitivityInInt(i);

				if(MWidget::IsMsg(szMessage, MEDIT_CHAR_MSG)==true)
				{
					SetMouseSensitivitySlider(v);
					return true;
				}
				else if(MWidget::IsMsg(szMessage, MEDIT_KILL_FOCUS)==true)
				{
					SetMouseSensitivitySlider(v);
					SetMouseSensitivityEdit(v);
					return true;
				}
			}
			return false;
		}
	};
	static ListenerClass	Listener;
	return &Listener;
}

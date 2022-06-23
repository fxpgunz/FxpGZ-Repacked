#include "stdafx.h"
#include "ZConfiguration.h"
#include "Mint.h"
#include "ZInterface.h"
#include "ZLocatorList.h"
#include "ZGameTypeList.h"
#include "ZLocale.h"

ZConfiguration	g_Configuration;
ZConfiguration* ZGetConfiguration()		{ return &g_Configuration; }

#ifdef LOCALE_BRAZIL
LANGID LangID = LANG_PORTUGUESE;		/* Brazil */
#endif


unsigned int ZLanguageSetting_forNHNUSA::m_idLang = 1;

void ZLanguageSetting_forNHNUSA::SetLanguageIndexFromCmdLineStr(const char* cmdline)
{
	const unsigned int id_USA = 0;
	const unsigned int id_DUT = 1;
	const unsigned int id_SPA = 2;
	const unsigned int id_POR = 3;
	const unsigned int id_GER = 4;
	const unsigned int id_KOR = 5;
	const unsigned int id_JAP = 6;

	m_idLang = id_USA;

	if (NULL == cmdline) return;

	if (NULL != strstr(cmdline, "&u100e:2=en")) m_idLang = id_USA;
	else if (NULL != strstr(cmdline, "&u100e:2=du")) m_idLang = id_DUT;
	else if (NULL != strstr(cmdline, "&u100e:2=sp")) m_idLang = id_SPA;
	else if (NULL != strstr(cmdline, "&u100e:2=po")) m_idLang = id_POR;
	else if (NULL != strstr(cmdline, "&u100e:2=ge")) m_idLang = id_GER;
	else if (NULL != strstr(cmdline, "&u100e:2=ko")) m_idLang = id_POR;
	else if (NULL != strstr(cmdline, "&u100e:2=ja")) m_idLang = id_GER;
}

ZLanguageSetting_forNHNUSA g_LanguageSettingForNHNUSA;
ZLanguageSetting_forNHNUSA* ZGetLanguageSetting_forNHNUSA() { return &g_LanguageSettingForNHNUSA; }

ZConfiguration::ZConfiguration()
{
	Init();

	strcpy( m_szServerIP, "0,0,0,0");
	m_nServerPort = 6000;
	
	strcpy( m_szBAReportAddr, "www.battlearena.com");
	strcpy( m_szBAReportDir, "incoming");
	
	m_nServerCount = 0;

	m_pLocatorList = new ZLocatorList;
	m_pTLocatorList = new ZLocatorList;

	m_pGameTypeList = new ZGameTypeList;

	m_bIsComplete = false;
	m_bReservedSave = false;
}

ZConfiguration::~ZConfiguration()
{
	if (m_bReservedSave)
	{
		char szPath[_MAX_PATH];
		TCHAR szMyDocPath[MAX_PATH];
		if (GetMyDocumentsPath(szMyDocPath)) {
			strcpy(szPath, szMyDocPath);
			strcat(szPath, GUNZ_FOLDER);
			CreatePath(szPath);
			strcat(szPath, "/config");
			CreatePath(szPath);
			strcat(szPath, "/");

			Save(szPath,Z_LOCALE_XML_HEADER);
		}
	}

	Destroy();
	SAFE_DELETE(m_pLocatorList);
	SAFE_DELETE(m_pTLocatorList);
	SAFE_DELETE(m_pGameTypeList);
}

void ZConfiguration::Destroy()
{
	m_Locale.vecSelectableLanguage.clear();

	m_pLocatorList->Clear();
	m_pTLocatorList->Clear();
	m_pGameTypeList->Clear();

	while(m_HotKeys.size())
	{
		Mint::GetInstance()->UnregisterHotKey(m_HotKeys.begin()->first);
		delete m_HotKeys.begin()->second;
		m_HotKeys.erase(m_HotKeys.begin());
	}
}

unsigned long int GetVirtKey(const char *key)
{
	int n=atoi(key+1);
	if((key[0]=='f' || key[0]=='F') && n>=1 && n<=12)
		return VK_F1+n-1;

	if(key[0]>='a' && key[0]<='z')
		return 'A'+key[0]-'a';

	return key[0];
}

char *GetKeyName(unsigned long int nVirtKey,char *out)
{
	if(nVirtKey>=VK_F1 && nVirtKey<=VK_F12)
		sprintf(out,"F%d",nVirtKey-VK_F1+1);
	else
		sprintf(out,"%d",nVirtKey);

	return out;
}

bool ZConfiguration::Load()
{
	bool retValue;

	// Config 는 외부 파일도 읽을수 있도록....외부파일들은 개별 지정해야 함..

#ifdef _PUBLISH
   		MZFile::SetReadMode( MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE );
#endif

	//ZGetLocale()->Init(DEFAULT_COUNTRY);
	if ( !LoadLocale(FILENAME_LOCALE) )
	{
		mlog( "Cannot open %s file.\n", FILENAME_LOCALE);
		return false;
	}

	retValue = LoadConfig(FILENAME_CONFIG);
	if (!retValue)
	{
		///load default attributes, user will end up saving the file to change their personal settings
		LoadDefaultKeySetting();
		Save(FILENAME_CONFIG, Z_LOCALE_XML_HEADER);
	}

	//	넷마블 버전은 구분해야함... 넷마블 버전은 MZIPREADFLAG_MRS1 도 읽어야함...

#ifdef _PUBLISH
		MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
#endif

	if ( !LoadSystem(FILENAME_SYSTEM))
	{
		mlog( "Cannot open %s file.\n", FILENAME_SYSTEM);
		return false;
	}

	if ( !retValue)
		return false;


	return retValue;
}

// strings.xml이 먼저 로딩되어야 하는 것들은 여기서
bool ZConfiguration::Load_StringResDependent()
{
	string strFileNameTCFG(FILENAME_GTCFG);
	if ( !LoadGameTypeCfg(strFileNameTCFG.c_str()) )
	{
		mlog( "Cannot open %s file.\n", strFileNameTCFG);
		return false;
	}
	return true;
}

bool ZConfiguration::LoadLocale(const char* szFileName)
{
	MZFile			mzFile;

	if (!mzFile.Open(szFileName, ZApplication::GetFileSystem()))
		return false;

	char* buffer = new char[ mzFile.GetLength()+1];
	buffer[mzFile.GetLength()]=0;
	mzFile.Read( buffer, mzFile.GetLength());
	rapidxml::xml_document<> doc;

	mlog( "Load XML from memory : %s", szFileName);
	if (!doc.parse<0>(buffer))
	{
		mlog("- FAIL\n");
		delete[] buffer;
		return false;
	}
	mlog( "- SUCCESS\n");

	rapidxml::xml_node<>* rootNode = doc.first_node();
	if (_stricmp(rootNode->first_node()->name(), "LOCALE") == 0)
	{
		m_Locale.strCountry = rootNode->first_node()->first_node(ZTOK_LOCALE_COUNTRY)->value();
		m_Locale.strDefaultLanguage = rootNode->first_node()->first_node(ZTOK_LOCALE_LANGUAGE)->value();
		m_Locale.nMaxPlayers = atoi(rootNode->first_node()->first_node(ZTOK_LOCALE_MAXPLAYERS)->value());

		if (rootNode->first_node()->first_node(ZTOK_LOCALE_SELECTABLE_LANGUAGES) != nullptr)
		{
			ParseLocaleSelectableLanguages(rootNode->first_node()->first_node(ZTOK_LOCALE_SELECTABLE_LANGUAGES)->first_node());
		}

		if (m_Locale.strCountry.length() == 0 || m_Locale.strDefaultLanguage.length() == 0) {
			mlog("config.xml - Country or Language is invalid.\n");
			return false;
		}

		mlog("Country : (%s), Language : (%s)\n", m_Locale.strCountry.c_str(), m_Locale.strDefaultLanguage.c_str());

	}
	delete[] buffer;
	doc.clear();
	mzFile.Close();

	return true;
}

void ZConfiguration::ParseLocaleSelectableLanguages(rapidxml::xml_node<>* selectableLangsElem)
{
	for (auto itor = selectableLangsElem; itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), ZTOK_LOCALE_LANGUAGE) == 0)
		{
			if (itor->first_attribute("id") == nullptr || itor->first_attribute("name") == nullptr)
				_ASSERT(0);

			ZCONFIG_SELECTABLE_LANGUAGE langSelectable;
			langSelectable.strLanguage = itor->first_attribute("id")->value();
			langSelectable.strLanguageName = itor->first_attribute("name")->value();
			m_Locale.vecSelectableLanguage.push_back(langSelectable);
		}
	}
}

bool ZConfiguration::LoadGameTypeCfg(const char* szFileName)
{
	MZFile			mzFile;
	if (!mzFile.Open(szFileName, ZApplication::GetFileSystem()))
		return false;

	char* buffer  = new char[ mzFile.GetLength()+1];
	buffer[mzFile.GetLength()]=0;
	mzFile.Read( buffer, mzFile.GetLength());
	rapidxml::xml_document<> doc;
	if (!doc.parse<0>(buffer)) {
		delete[] buffer;
		return false;
	}

	mlog( "Load XML from memory : %s \n", szFileName);

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), ZTOK_GAME_TYPE) == 0) {
			int nID = atoi(itor->first_attribute("id")->value());
			m_pGameTypeList->ParseGameTypeList(nID, itor->first_node());
		}
	}

	delete[] buffer;
	doc.clear();
	mzFile.Close();

	return true;
}

bool ZConfiguration::LoadSystem(const char* szFileName)
{
	MZFile			mzFile;

	if (!mzFile.Open(szFileName, ZApplication::GetFileSystem()))
		return false;


	char* buffer = new char[ mzFile.GetLength()+1];
	buffer[mzFile.GetLength()]=0;
	mzFile.Read( buffer, mzFile.GetLength());
	rapidxml::xml_document<> doc;

	mlog( "Load XML from memory : %s", FILENAME_SYSTEM );

	if( !doc.parse<0>(buffer))
	{
		mlog( "- FAIL\n");

		delete[] buffer;
		return false;
	}

	mlog( "- SUCCESS\n");

	rapidxml::xml_node<>* rootNode = doc.first_node();
	m_nServerCount = 0;
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), ZTOK_LOCALE_XMLHEADER) == 0)
		{
			strcpy_s(m_Locale.szXmlHeader, itor->value());
		}
		if (_stricmp(itor->name(), ZTOK_SKIN) == 0)
		{
			strcpy_s(m_szInterfaceSkinName, itor->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_DEFFONT) == 0)
		{
			m_Locale.strDefaultFont = itor->value();
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_IME) == 0)
		{
			m_Locale.bIMESupport = atob(itor->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_BAREPORT) == 0)
		{
			strcpy_s(m_szBAReportAddr, itor->first_node(ZTOK_ADDR)->value());
			strcpy_s(m_szBAReportDir ,itor->first_node(ZTOK_DIR)->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_EMBLEM_URL)==0)
		{
			strcpy_s(m_Locale.szEmblemURL, itor->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_TEMBLEM_URL) == 0)
		{
			strcpy_s(m_Locale.szTEmblemURL, itor->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_CASHSHOP_URL) == 0)
		{
			strcpy_s(m_Locale.szCashShopURL, itor->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCALE_HOMEPAGE) == 0)
		{
			strcpy_s(m_Locale.szHomepageUrl, itor->first_node(ZTOK_LOCALE_HOMEPAGE_URL)->value());
			strcpy_s(m_Locale.szHomepageTitle, itor->first_node(ZTOK_LOCALE_HOMEPAGE_TITLE)->value());
		}
		if (_stricmp(itor->name(), ZTOK_LOCATOR_LIST) == 0)
		{
			m_pLocatorList->ParseLocatorList(itor->first_node());
		}
		if (_stricmp(itor->name(), ZTOK_TLOCATOR_LIST) == 0)
		{
			m_pTLocatorList->ParseLocatorList(itor->first_node());
		}
	}

	delete[] buffer;
	mzFile.Close();
	doc.clear();

	m_bIsComplete = true;
	return true;
}

bool ZConfiguration::LoadConfig(const char* szFileName)
{
	rapidxml::xml_document<> doc;

	mlog( "Load Config from file : %s", szFileName );


	MZFile mzf;
	if (!mzf.Open(szFileName))
	{
		mlog("- FAIL\n");
		return false;
	}
	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing config.xml");
		delete[] buffer;
		return false;
	}
	mlog( "- SUCCESS\n");

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), ZTOK_SERVER) == 0)
		{
			strcpy_s(m_szServerIP,itor->first_node(ZTOK_IP)->value());
			m_nServerPort = atoi(itor->first_node(ZTOK_PORT)->value());
		}
		if (_stricmp(itor->name(), ZTOK_VIDEO) == 0)
		{
			m_Video.nWidth = atoi(itor->first_node(ZTOK_VIDEO_WIDTH)->value());
			m_Video.nHeight = atoi(itor->first_node(ZTOK_VIDEO_HEIGHT)->value());
			m_Video.nColorBits = atoi(itor->first_node(ZTOK_VIDEO_COLORBITS)->value());
			m_Video.bFullScreen = atob(itor->first_node(ZTOK_VIDEO_FULLSCREEN)->value());
			m_Video.bArchetype = atob(itor->first_node(ZTOK_VIDEO_ARCHETYPE)->value());
			m_Video.nGamma = atoi(itor->first_node(ZTOK_VIDEO_GAMMA)->value());
			m_Video.bReflection = atob(itor->first_node(ZTOK_VIDEO_REFLECTION)->value());
			m_Video.bLightMap = atob(itor->first_node(ZTOK_VIDEO_LIGHTMAP)->value());
			m_Video.bDynamicLight = atob(itor->first_node(ZTOK_VIDEO_DYNAMICLIGHT)->value());
			m_Video.bHPAP = atob(itor->first_node(ZTOK_VIDEO_HPAP)->value());
			m_Video.bShader = atob(itor->first_node(ZTOK_VIDEO_SHADER)->value());
			m_Video.nAntiAlias = atoi(itor->first_node(ZTOK_VIDEO_ANTIALIAS)->value());
			m_Video.bStencilBuffer = atob(itor->first_node(ZTOK_VIDEO_STENCILBUFFER)->value());
			m_Video.nCharTexLevel = atoi(itor->first_node(ZTOK_VIDEO_CHARTEXLEVEL)->value());
			m_Video.nMapTexLevel = atoi(itor->first_node(ZTOK_VIDEO_MAPTEXLEVEL)->value());
			m_Video.nEffectLevel = atoi(itor->first_node(ZTOK_VIDEO_EFFECTLEVEL)->value());
			m_Video.nTextureFormat = atoi(itor->first_node(ZTOK_VIDEO_TEXTUREFORMAT)->value());
			m_MovingPicture.iResolution = atoi(itor->first_node(ZTOK_MOVINGPICTURE_RESOLUTION)->value());
			m_MovingPicture.iFileSize = atoi(itor->first_node(ZTOK_MOVINGPICTURE_FILESIZE)->value());
		}
		if (_stricmp(itor->name(), ZTOK_AUDIO) == 0)
		{
			//m_Audio.bBGMEnabled = atob(itor->first_node(ZTOK_AUDIO_BGM_ENABLED)->value());
			m_Audio.fBGMVolume = atof(itor->first_node(ZTOK_AUDIO_BGM_VOLUME)->value());
			m_Audio.fEffectVolume = atof(itor->first_node(ZTOK_AUDIO_EFFECT_VOLUME)->value());
			m_Audio.bBGMMute = atob(itor->first_node(ZTOK_AUDIO_BGM_MUTE)->value());
			m_Audio.bEffectMute = atob(itor->first_node(ZTOK_AUDIO_EFFECT_MUTE)->value());
			m_Audio.b8BitSound = atob(itor->first_node(ZTOK_AUDIO_8BITSOUND)->value());
			m_Audio.bInverse = atob(itor->first_node(ZTOK_AUDIO_INVERSE)->value());
			m_Audio.bHWMixing = atob(itor->first_node(ZTOK_AUDIO_HWMIXING)->value());
			m_Audio.bHitSound = atob(itor->first_node(ZTOK_AUDIO_HITSOUND)->value());
			m_Audio.bNarrationSound = atob(itor->first_node(ZTOK_AUDIO_NARRATIONSOUND)->value());
			m_Audio.b3DSound = atob(itor->first_node(ZTOK_AUDIO_3D_SOUND)->value());
			m_Audio.enableBackGroundAudio = atob(itor->first_node("BACKGROUNDMUSIC")->value());
		}
		if (_stricmp(itor->name(), ZTOK_MOUSE) == 0)
		{
			m_Mouse.fSensitivity = atof(itor->first_node(ZTOK_MOUSE_SENSITIVITY)->value());
			m_Mouse.bInvert = atob(itor->first_node(ZTOK_MOUSE_INVERT)->value());
		}
		if (_stricmp(itor->name(), ZTOK_JOYSTICK) == 0)
		{
			m_Joystick.fSensitivity = atof(itor->first_node(ZTOK_JOYSTICK_SENSITIVITY)->value());
			m_Joystick.bInvert = atob(itor->first_node(ZTOK_JOYSTICK_INVERT)->value());
		}
		///TODO: keyboard
		if (_stricmp(itor->name(), ZTOK_KEYBOARD) == 0)
		{
			for (int i = 0; i < ZACTION_COUNT; ++i)
			{
				char szItemName[256];
				strcpy(szItemName, m_Keyboard.ActionKeys[i].szName);
				_strupr(szItemName);

				if (_stricmp(itor->first_node(szItemName)->first_attribute("alt")->value(), "-1") == 0)
					m_Keyboard.ActionKeys[i].nVirtualKeyAlt = -1;
				else
					m_Keyboard.ActionKeys[i].nVirtualKeyAlt = atoi(itor->first_node(szItemName)->first_attribute("alt")->value());

				m_Keyboard.ActionKeys[i].nVirtualKey = atoi(itor->first_node(szItemName)->value());
			}
		}
		if (_stricmp(itor->name(), ZTOK_MACRO) == 0)
		{
			strcpy_s(m_Macro.szMacro[0], 255, itor->first_node(ZTOK_MACRO_F1)->value());
			strcpy_s(m_Macro.szMacro[1], 255, itor->first_node(ZTOK_MACRO_F2)->value());
			strcpy_s(m_Macro.szMacro[2], 255, itor->first_node(ZTOK_MACRO_F3)->value());
			strcpy_s(m_Macro.szMacro[3], 255, itor->first_node(ZTOK_MACRO_F4)->value());
			strcpy_s(m_Macro.szMacro[4], 255, itor->first_node(ZTOK_MACRO_F5)->value());
			strcpy_s(m_Macro.szMacro[5], 255, itor->first_node(ZTOK_MACRO_F6)->value());
			strcpy_s(m_Macro.szMacro[6], 255, itor->first_node(ZTOK_MACRO_F7)->value());
			strcpy_s(m_Macro.szMacro[7], 255, itor->first_node(ZTOK_MACRO_F8)->value());
		}
		if (_stricmp(itor->name(), ZTOK_ETC) == 0)
		{
			m_Etc.nNetworkPort1 = atoi(itor->first_node(ZTOK_ETC_NETWORKPORT1)->value());
			m_Etc.nNetworkPort2 = atoi(itor->first_node(ZTOK_ETC_NETWORKPORT2)->value());
			m_Etc.bBoost = atob(itor->first_node(ZTOK_ETC_BOOST)->value());
			m_Etc.bRejectNormalChat = atob(itor->first_node(ZTOK_ETC_REJECT_NORMALCHAT)->value());
			m_Etc.bRejectTeamChat = atob(itor->first_node(ZTOK_ETC_REJECT_TEAMCHAT)->value());
			m_Etc.bRejectClanChat = atob(itor->first_node(ZTOK_ETC_REJECT_CLANCHAT)->value());
			m_Etc.bRejectWhisper = atob(itor->first_node(ZTOK_ETC_REJECT_WHISPER)->value());
			m_Etc.bRejectInvite = atob(itor->first_node(ZTOK_ETC_REJECT_INVITE)->value());
			m_Etc.nCrossHair = atoi(itor->first_node(ZTOK_ETC_CROSSHAIR)->value());
			m_Etc.nFrameLimit_perSecond = atoi(itor->first_node(ZTOK_ETC_FRAMELIMIT_PERSECOND)->value());
			if (itor->first_node(ZTOK_ETC_LANGUAGE))
				strcpy_s(m_Etc.szLanguage, 32, itor->first_node(ZTOK_ETC_LANGUAGE)->value());
			else
				strcpy_s(m_Etc.szLanguage, 32, "USA");
		}
		if (_stricmp(itor->name(), ZTOK_CUSTOM) == 0)
		{
			m_Etc.bDisableSW = atob(itor->first_node(ZTOK_CUSTOM_DISABLE_SW)->value());
			m_Etc.bTrailOption = atob(itor->first_node(ZTOK_CUSTOM_TRAIL_OPTION)->value());
			m_Etc.bFpsOverlay = atob(itor->first_node(ZTOK_CUSTOM_OVERLAY_OPTION)->value());
			m_Audio.nCustomMusicFolders = atoi(itor->first_node("MUSICFOLDER")->value());
			m_Audio.bCustomMusicEnabled = atob(itor->first_node("ENABLECUSTOMMUSIC")->value());
			m_Video.bNewRenderer = atob(itor->first_node("NEWRENDERER")->value());
			m_Video.enableD3D9Ex = atob(itor->first_node("D3D9EX")->value());
		}
	}

	ValidateSelectedLanguage();
	
	delete[] buffer;
	mzf.Close();
	doc.clear();

	return true;
}

// 핫키는 봉인되었다.
bool ZConfiguration::LoadHotKey(const char* szFileName)
{
	///This is unused
	/*

	xmlConfig.Create();
	if (!xmlConfig.LoadFromFile(szFileName)) 
	{
		xmlConfig.Destroy();
		return false;
	}

	parentElement = xmlConfig.GetDocumentElement();
	int iCount = parentElement.GetChildNodeCount();

	if (!parentElement.IsEmpty())
	{
		if (parentElement.FindChildNode(ZTOK_BINDS, &bindsElement))
		{
			for(int i=0;i<bindsElement.GetChildNodeCount();i++)
			{
				char tagname[256];
				bind.GetTagName(tagname);
				if(strcmp(tagname,ZTOK_BIND)==0)
				{
					char key[256],command[256];
					bool ctrl,alt,shift;

					bind.GetAttribute(key,ZTOK_KEY);
					bind.GetAttribute(&ctrl,ZTOK_KEY_CTRL);
					bind.GetAttribute(&alt,ZTOK_KEY_ALT);
					bind.GetAttribute(&shift,ZTOK_KEY_SHIFT);
					bind.GetContents(command);

					ZHOTKEY *photkey=new ZHOTKEY;
					photkey->nModifier=0;

					if(ctrl) photkey->nModifier|=MOD_CONTROL;
					if(alt) photkey->nModifier|=MOD_ALT;
					if(shift) photkey->nModifier|=MOD_SHIFT;

					photkey->nVirtKey=GetVirtKey(key);

					photkey->command=string(command);

					int nHotkeyID=Mint::GetInstance()->RegisterHotKey(photkey->nModifier,photkey->nVirtKey);

					m_HotKeys.insert(ZHOTKEYS::value_type(nHotkeyID,photkey));
				}
			}
		}
	}

	xmlConfig.Destroy();

	return true;
	*/
	return true;
}

bool ZConfiguration::SaveToFile(const char *szFileName, const char* szHeader)
{
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* rootNode = doc.allocate_node(rapidxml::node_element, "XML");
	rootNode->append_attribute(doc.allocate_attribute("id", "config"));
	doc.append_node(rootNode);

	rapidxml::xml_node<>* parentNode;
	rapidxml::xml_node<>* childNode;
	///Server;
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_SERVER);
		rootNode->insert_node(0, parentNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_IP, doc.allocate_string(m_szServerIP));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_PORT, doc.allocate_string(to_string(m_nServerPort).c_str()));
		parentNode->insert_node(0, childNode);
	}
	///SKIN
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_SKIN, doc.allocate_string(m_szInterfaceSkinName));
		rootNode->insert_node(0, parentNode);
	}
	///VIDEO
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO);
		rootNode->insert_node(0, parentNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_WIDTH, doc.allocate_string(to_string(m_Video.nWidth).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_HEIGHT, doc.allocate_string(to_string(m_Video.nHeight).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_COLORBITS, doc.allocate_string(to_string(m_Video.nColorBits).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_GAMMA, doc.allocate_string(to_string(m_Video.nGamma).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_FULLSCREEN, m_Video.bFullScreen ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_ARCHETYPE, m_Video.bArchetype ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_REFLECTION, m_Video.bReflection ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_LIGHTMAP, m_Video.bLightMap ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_HPAP, m_Video.bHPAP ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_DYNAMICLIGHT, m_Video.bDynamicLight ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_SHADER, m_Video.bShader ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_ANTIALIAS, doc.allocate_string(to_string(m_Video.nAntiAlias).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_STENCILBUFFER, m_Video.bStencilBuffer ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_CHARTEXLEVEL, doc.allocate_string(to_string(m_Video.nCharTexLevel).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_MAPTEXLEVEL, doc.allocate_string(to_string(m_Video.nMapTexLevel).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_EFFECTLEVEL, doc.allocate_string(to_string(m_Video.nEffectLevel).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_VIDEO_TEXTUREFORMAT, doc.allocate_string(to_string(m_Video.nTextureFormat).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_MOVINGPICTURE_RESOLUTION, doc.allocate_string(to_string(m_MovingPicture.iResolution).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_MOVINGPICTURE_FILESIZE, doc.allocate_string(to_string(m_MovingPicture.iFileSize).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, "NHARDWARETNL", m_Video.bTerrible ? "true" : "false");
		parentNode->insert_node(0, childNode);
	}

	///AUDIO
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO);
		rootNode->insert_node(0, parentNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_BGM_MUTE, m_Audio.bBGMMute ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_BGM_VOLUME, doc.allocate_string(to_string(m_Audio.fBGMVolume).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_EFFECT_MUTE, m_Audio.bEffectMute ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_EFFECT_VOLUME, doc.allocate_string(to_string(m_Audio.fEffectVolume).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_3D_SOUND, m_Audio.b3DSound ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_8BITSOUND, m_Audio.b8BitSound ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_INVERSE, m_Audio.bInverse ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_HWMIXING, m_Audio.bHWMixing ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_HITSOUND, m_Audio.bHitSound ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_AUDIO_NARRATIONSOUND, m_Audio.bNarrationSound ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, "BACKGROUNDMUSIC", m_Audio.enableBackGroundAudio ? "true" : "false");
		parentNode->insert_node(0, childNode);
	}
	///MOUSE
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_MOUSE);
		rootNode->insert_node(0, parentNode);

		m_Mouse.fSensitivity = ValidateMouseSensitivityInFloat(m_Mouse.fSensitivity);
		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_MOUSE_SENSITIVITY, doc.allocate_string(to_string(m_Mouse.fSensitivity).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_MOUSE_INVERT, m_Mouse.bInvert ? "true" : "false");
		parentNode->insert_node(0, childNode);
	}
	///JOYSTICK
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_JOYSTICK);
		rootNode->insert_node(0, parentNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_JOYSTICK_SENSITIVITY, doc.allocate_string(to_string(m_Joystick.fSensitivity).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_JOYSTICK_INVERT, m_Joystick.bInvert ? "true" : "false");
		parentNode->insert_node(0, childNode);
	}
	///KEYBOARD
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_KEYBOARD);
		rootNode->insert_node(0, parentNode);

		for (int i = 0; i < ZACTION_COUNT; ++i) {

			char keyName[256];
			if (m_Keyboard.ActionKeys[i].szName[0] == 0)
				continue;

			strcpy_s(keyName, m_Keyboard.ActionKeys[i].szName);

			strupr(keyName);

			childNode = doc.allocate_node(rapidxml::node_element, doc.allocate_string(keyName), doc.allocate_string(to_string(m_Keyboard.ActionKeys[i].nVirtualKey).c_str()));
			childNode->append_attribute(doc.allocate_attribute("alt", doc.allocate_string(to_string(m_Keyboard.ActionKeys[i].nVirtualKeyAlt).c_str())));

			parentNode->insert_node(0, childNode);
		}
	}
	///MACRO
	{

		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_MACRO);
		rootNode->insert_node(0, parentNode);
		char* _temp[ZCONFIG_MACRO_MAX] = {
			ZTOK_MACRO_F1,
			ZTOK_MACRO_F2,
			ZTOK_MACRO_F3,
			ZTOK_MACRO_F4,
			ZTOK_MACRO_F5,
			ZTOK_MACRO_F6,
			ZTOK_MACRO_F7,
			ZTOK_MACRO_F8,
		};

		for (int i = 0; i < ZCONFIG_MACRO_MAX; i++) {

			childNode = doc.allocate_node(rapidxml::node_element, doc.allocate_string(_temp[i]), doc.allocate_string(m_Macro.szMacro[i]));
			parentNode->insert_node(0, childNode);
		}
	}
	///ETC
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC);
		rootNode->insert_node(0, parentNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_NETWORKPORT1, doc.allocate_string(to_string(m_Etc.nNetworkPort1).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_NETWORKPORT2, doc.allocate_string(to_string(m_Etc.nNetworkPort2).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_BOOST, m_Etc.bBoost ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_LANGUAGE, doc.allocate_string(GetSelectedLanguage()));
		parentNode->insert_node(0, childNode);
		g_LanguageSettingForNHNUSA.SetLanguageSetting(GetSelectedLanguageIndex());

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_REJECT_NORMALCHAT, m_Etc.bRejectNormalChat ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_REJECT_TEAMCHAT, m_Etc.bRejectTeamChat ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_REJECT_CLANCHAT, m_Etc.bRejectClanChat ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_REJECT_WHISPER, m_Etc.bRejectWhisper ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_REJECT_INVITE, m_Etc.bRejectInvite ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_CROSSHAIR, doc.allocate_string(to_string(m_Etc.nCrossHair).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_ETC_FRAMELIMIT_PERSECOND, doc.allocate_string(to_string(m_Etc.nFrameLimit_perSecond).c_str()));
		parentNode->insert_node(0, childNode);

	}
	///CUSTOM
	{
		parentNode = doc.allocate_node(rapidxml::node_element, ZTOK_CUSTOM);
		rootNode->insert_node(0, parentNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_CUSTOM_DISABLE_SW, m_Etc.bDisableSW ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_CUSTOM_TRAIL_OPTION, m_Etc.bTrailOption ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, ZTOK_CUSTOM_OVERLAY_OPTION, m_Etc.bFpsOverlay ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, "ENABLECUSTOMMUSIC", m_Audio.bCustomMusicEnabled ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, "MUSICFOLDER", doc.allocate_string(to_string(m_Audio.nCustomMusicFolders == -1 ? 0 : m_Audio.nCustomMusicFolders).c_str()));
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, "NEWRENDERER", m_Video.bNewRenderer ? "true" : "false");
		parentNode->insert_node(0, childNode);

		childNode = doc.allocate_node(rapidxml::node_element, "D3D9EX", m_Video.enableD3D9Ex ? "true" : "false");
		parentNode->insert_node(0, childNode);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(szFileName);
	file_stored << doc;
	file_stored.close();
	doc.clear();

	LANGID LangID = LANG_PORTUGUESE;

	return true;

}

ZHOTKEY *ZConfiguration::GetHotkey(int nID) 
{ 
	ZHOTKEYS::iterator found=m_HotKeys.find(nID);
	return found==m_HotKeys.end() ? NULL : found->second;
}

void ZConfiguration::Init()
{
	m_Video.bFullScreen = true;
	m_Video.nWidth = 1024;
	m_Video.nHeight = 768;
	m_Video.nColorBits = 32;
	m_Video.nGamma = 255;
	m_Video.bShader		= true;
	m_Video.nAntiAlias = 0;
	m_Video.bStencilBuffer = false;
	m_Video.bLightMap	= false;
	m_Video.bHPAP		=true;
	m_Video.bReflection	= true;
	m_Video.nCharTexLevel = 0;//기본 고품질
	m_Video.nMapTexLevel = 0;
	m_Video.nEffectLevel = Z_VIDEO_EFFECT_HIGH;
	m_Video.nTextureFormat = 1;
	m_Video.bTerrible = false;
	m_MovingPicture.iResolution = 0;
	m_MovingPicture.iFileSize = 0;

	m_Audio.bBGMEnabled = true;
	m_Audio.fBGMVolume	= 0.3f;
	m_Audio.bBGMMute	= false;
	m_Audio.bEffectMute = false;
	m_Audio.b3DSound	= true;
	m_Audio.b8BitSound	= false;
	m_Audio.bInverse	= false;
	m_Audio.bHWMixing	= false;
	m_Audio.bHitSound	= true;
	m_Audio.bNarrationSound	= true;
	m_Audio.enableBackGroundAudio = false;

	m_Mouse.fSensitivity = 1.f;
	m_Mouse.bInvert = false;

	m_Joystick.fSensitivity = 1.f;
	m_Joystick.bInvert = false;

	m_Macro.SetString(0,"");
	m_Macro.SetString(1,"");
	m_Macro.SetString(2,"");
	m_Macro.SetString(3,"");
	m_Macro.SetString(4,"");
	m_Macro.SetString(5,"");
	m_Macro.SetString(6,"");
	m_Macro.SetString(7,"");
	m_Macro.SetString(8,"");

	m_Etc.nNetworkPort1 = 7700;
	m_Etc.nNetworkPort2 = 7800;
	m_Etc.nCrossHair = 0;
	m_Etc.nFrameLimit_perSecond = 0;
	m_Etc.bInGameNoChat = false;
	m_Etc.bTrailOption = true; //Trail Option
	m_Etc.bFpsOverlay = true;

	m_bOptimization = false;


	
	memset(m_szServerIP, 0, sizeof(m_szServerIP));
	strcpy(m_szServerIP, "0.0.0.0");
	m_nServerPort = 6000;
	strcpy(m_szInterfaceSkinName, DEFAULT_INTERFACE_SKIN);

	LoadDefaultKeySetting();	



	strcpy( m_Locale.szDefaultFont, "Gulim");
	strcpy( m_Locale.szXmlHeader, "version=\"1.0\" encoding=\"UTF-8\"");
	m_Locale.szHomepageUrl[0] = 0;
	m_Locale.szHomepageTitle[0] = 0;
	strcpy(m_Locale.szEmblemURL, "");
	strcpy(m_Locale.szTEmblemURL, "");
	strcpy(m_Locale.szCashShopURL, "http://www.gunzonline.com/");
	m_Locale.bIMESupport = false;

	m_bViewGameChat = true;
	///Custom stuff
	m_Video.bNewRenderer = false;
	strcpy_s(m_Etc.szLanguage, "USA");
	m_Audio.bCustomMusicEnabled = false;
	m_Audio.enableBackGroundAudio = false;
	strcpy_s(m_Audio.defaultMusicPath, "");
	m_Audio.nCustomMusicFolders = 0;
	m_Video.bStencilBuffer = false;
	m_Video.nAntiAlias = 0;
	m_Video.enableD3D9Ex = false;
}

void ZConfiguration::LoadDefaultKeySetting()
{
	// option.xml에서 각 액션키에 대응되는 위젯은 ActionKey 접미사를 붙여서 위젯 이름을 정해야 함
	// ex: 무기 사용 설정 위젯은 'UseWeaponActionKey' 
	static ZACTIONKEYDESCRIPTION DefaultActionKeys[ZACTION_COUNT] = {
		{"UseWeapon",	0x1D,258},	// 'ctrl' or	mouse LButton
		{"UseWeapon2",	259,-1},		// mouse RButton
		{"PrevousWeapon",0x10, 256},	// 'q' or wheel up
		{"NextWeapon", 0x12,257},	// 'e' or wheel down

		{"Forward",		0x11, -1},	// 'w'
		{"Back",		0x1f, -1},	// 's'
		{"Left",		0x1e, -1},	// 'a'
		{"Right",		0x20, -1},	// 'd'

		{"MeleeWeapon", 0x02, -1},		// '1'
		{"PrimaryWeapon",0x03, -1},		// '2'
		{"SecondaryWeapon",0x04, -1},	// '3'
		{"Item1",		0x05, -1},		// '4'
		{"Item2",		0x06, -1},		// '5'
		
		{"CommunityItem1",		0x07, -1},		// '6'
		{"CommunityItem2",		0x08, -1},		// '7'

		{"Reload",		0x13,-1},	// 'r'
		{"Jump",		0x39,-1},	// space
		{"Score",		0x0f,-1},	// tab
		{"Menu",		0x01,-1},	// esc

		{"Taunt",		0x29,-1},	// '`'
		{"Bow",			0x07,-1},	// '6'
		{"Wave",		0x08,-1},	// '7'
		{"Laugh",		0x09,-1},	// '8'
		{"Cry",			0x0a,-1},	// '9'
		{"Dance",		0x0b,-1},	// '0'

		{"ScreenShot",	0x58,-1},	// 'F12'
		{"Record",		0x57,-1},	// 'F11'
		{"MovingPicture",0x44,-1},	// 'F10'
		{"Defence",		0x2a,-1},	// 'shift'
		{"ToggleChat",	0x2f,-1},	// 'v'

		{"MouseSensitivityDec", 0x1A, -1},		//'['
		{"MouseSensitivityInc", 0x1B, -1},		//']'
		///BlitzKRieg
		{"BlitzAbilityWnd",-1,-1},
		{"BlitzHelpWnd",-1,-1},
		{"BlitzUpgradeWeapon",-1,-1}, //0x07
		{"BlitzUpgradeBullet",-1,-1}, //0x08
		{"BlitzUpgradeArmor",-1,-1}, //0x09
		{"BlitzUpgradeFire",-1,-1}, //0x0A
		{"BlitzUpgradeMagazine",-1,-1}, //0x0B
		{"BlitzUpgradeMedic",-1,-1}, //0x0C
		//Custom: bomb planting
		{"BombPlanting", 0x19, -1}
	};

	// Define된 ID와 Description의 개수가 정확히 맞아야 한다.
	_ASSERT(ZACTION_COUNT==sizeof(DefaultActionKeys)/sizeof(ZACTIONKEYDESCRIPTION));

	memcpy(m_Keyboard.ActionKeys, DefaultActionKeys, sizeof(ZACTIONKEYDESCRIPTION)*ZACTION_COUNT);
}

ZSERVERNODE ZConfiguration::GetServerNode( int nNum)
{
	ZSERVERNODE ServerNode;

	map<int,ZSERVERNODE>::iterator iterator;

	iterator = m_ServerList.find( nNum);
	if ( iterator != m_ServerList.end())
	{
		ServerNode = (*iterator).second;
	}

	return ServerNode;
}


const bool ZConfiguration::LateStringConvert()
{
	char buf[ZCONFIG_MACRO_MAX][256];

	for (int i = 0; i < ZCONFIG_MACRO_MAX; i++)
	{
		memset( buf[i], 0, 256 );
		strcpy( buf[i], m_Macro.szMacro[i] );
		memset( m_Macro.szMacro[i],0, 256 );
		strcpy(m_Macro.szMacro[i], ZGetStringResManager()->GetStringFromXml(buf[i]));
	}

	return true;
}


const char* ZConfiguration::GetLanguagePath()
{
	switch (GetSelectedLanguageIndex())
	{
	case 0:
		return PATH_USA;
	case 1:
		return PATH_DUT;
	case 2:
		return PATH_SPA;
	case 3:
		return PATH_POR;
	case 4:
		return PATH_GER;
	case 5:
		return PATH_KOR;
	case 6:
		return PATH_JAP;
	default:
		return PATH_USA;
	}
}

unsigned int ZConfiguration::GetSelectedLanguageIndex()
{
	size_t size = m_Locale.vecSelectableLanguage.size();
	for (unsigned int i=0; i<size; ++i)
	{
		if (m_Locale.vecSelectableLanguage[i].strLanguage == m_Etc.szLanguage)
			return i;
	}
	return -1;
}

void ZConfiguration::ValidateSelectedLanguage()
{
	if (-1 == GetSelectedLanguageIndex())
	{
		if (GetSelectedLanguage()[0] != 0) _ASSERT(0);
		strcpy(m_Etc.szLanguage, m_Locale.strDefaultLanguage.c_str());
	}
}

void ZConfiguration::SetSelectedLanguageIndex( unsigned int i )
{
	if (i >= m_Locale.vecSelectableLanguage.size())
	{
		_ASSERT(0);
		return;
	}

	strcpy(m_Etc.szLanguage, m_Locale.vecSelectableLanguage[i].strLanguage.c_str());
}

int ZConfiguration::ValidateMouseSensitivityInInt( int i )
{
	if (i < MOUSE_SENSITIVITY_MIN)
		i = MOUSE_SENSITIVITY_MIN;

	else if (i > MOUSE_SENSITIVITY_MAX)
		i = MOUSE_SENSITIVITY_MAX;

	return i;
}

float ZConfiguration::ValidateMouseSensitivityInFloat( float f )
{
	return ValidateMouseSensitivityInInt(f*MOUSE_SENSITIVITY_MAX) / float(MOUSE_SENSITIVITY_MAX);
}

float ZConfiguration::GetMouseSensitivityInFloat()
{
	return Z_MOUSE_SENSITIVITY;
}

int ZConfiguration::GetMouseSensitivityInInt()
{
	return (int(ZGetConfiguration()->GetMouse()->fSensitivity * MOUSE_SENSITIVITY_MAX));
}

float ZConfiguration::SetMouseSensitivityInFloat(float f)
{
	float validated = ValidateMouseSensitivityInFloat(f);
	
	Z_MOUSE_SENSITIVITY = validated;
	return validated;
}

int ZConfiguration::SetMouseSensitivityInInt(int i)
{
	int validated = ValidateMouseSensitivityInInt(i);

	Z_MOUSE_SENSITIVITY = validated / (float)MOUSE_SENSITIVITY_MAX;
	return validated;
}
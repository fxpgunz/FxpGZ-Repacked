#include "stdafx.h"
#include "ZChatCmd.h"
#include "MDebug.h"
#include "ZMyInfo.h"

void ZImplodeChatCmdArgs(char* szOut, const int argc, char **const argv, int nFirstIndex)
{
	szOut[0] = 0;

	for (int i = nFirstIndex; i < argc; i++)
	{
		strcat(szOut, argv[i]);

		if (i != (argc-1)) strcat(szOut, " ");
	}
}

int IsSpace(int c)
{
	if (c < 0) return 0;
	if (c > 127) return 0;
	return isspace(c);
}

ZChatCmd::ZChatCmd(int nID, const char* szName, unsigned long int flag,
					int nMinArgs, int nMaxArgs,bool bRepeatEnabled,
					const char* szUsage, const char* szHelp)
{
	m_nID = nID;

	m_fnProc = NULL;
	m_nFlag = flag;

	m_nMinArgs = nMinArgs;
	m_nMaxArgs = nMaxArgs;

	m_bRepeatEnabled = bRepeatEnabled;

	strcpy(m_szName, szName);
	strcpy(m_szUsage, szUsage);
	strcpy(m_szHelp, szHelp);
}

void ZChatCmd::OnProc(const char* line, const int argc, char **const argv)
{
	if (m_fnProc != NULL)
	{
		(*m_fnProc)(line, argc, argv);
	}
}



ZChatCmd* ZChatCmdManager::MakeArgv(char* szLine, ZChatCmdArgvInfo* pAI)
{
	int nLen = (int)strlen(szLine);
	if (nLen >= 2048) NULL;

	char szBuffer[2048];
	strcpy(szBuffer, szLine);

	// 문자열 끝의 스페이스 제거
	for (int pos = nLen-1; pos >= 0; pos--)
	{
		if (IsSpace(szBuffer[pos])) szBuffer[pos] = '\0';
		else break;
	}


	ZChatCmd* pCmd = NULL;

	int c;
	char* scp;
	char* dcp;
	char* dlim;
	char* arg;

	int nArgcMax, nCmdArgcMax;
	nCmdArgcMax = nArgcMax = (sizeof(pAI->cargv) / sizeof(char*));
	

	scp = szBuffer;
	dcp = pAI->argbuf;
	dlim = dcp + sizeof(pAI->argbuf) - 1;

	for (pAI->cargc = 0; pAI->cargc < nArgcMax; )
	{
		for ( ; ; scp++)
		{
			c = *scp;
			if (IsSpace(c)) continue;

			if ( (c == '\0') || (c == '\n') )
			{
				pAI->cargv[pAI->cargc] = NULL;
				return pCmd;
			}
			break;
		}
		arg = dcp;
		pAI->cargv[pAI->cargc] = arg;
		(pAI->cargc)++;

		for ( ; ; )
		{
			c = *scp;
			if ( (c == '\0') || (c == '\n')) break;

			// 마지막 인자는 무조건 문자열 마지막까지의 값이다.
			if (pAI->cargc != nCmdArgcMax)
			{
				if (IsSpace(c)) break;
			}
			scp++;

			// 작은 따옴표 처리
			if (c == '\'') 
			{
//				for ( ; ; ) 
//				{
//					c = *scp++;

					// 따옴표가 하나만 있으면 널 리턴
//					if (c == '\0') return NULL;

//					if (c == '\'') break;

					// 너무 길다
//					if (dcp >= dlim) return NULL;

					*dcp++ = c;
//				}
			} 
			else if (c == '"')	// 큰따옴표처리
			{	
//				for ( ; ; ) 
//				{
//					c = *scp++;
//					if (c == '\0') return NULL;
//					if (c == '"') break;

//					if (dcp >= dlim) return NULL;
					*dcp++ = c;
//				}
			}
            else 
			{
				if (dcp >= dlim) return NULL;
				*dcp++ = c;
			}
		}

		*dcp++ = '\0';

		if (pAI->cargc == 1)
		{
			pCmd = GetCommandByName(pAI->cargv[0]);
			if (pCmd != NULL) 
			{
				if (pCmd->GetMaxArgs() != ARGVNoMin)
				{
					nCmdArgcMax = pCmd->GetMaxArgs() + 1;
				}
			}
			else
			{
				return NULL;
			}
		}
	}
	
	return NULL;
}


void ZChatCmdManager::AddCommand(int nID, const char* szName, ZChatCmdProc* fnProc, unsigned long int flag,
				int nMinArgs, int nMaxArgs, bool bRepeatEnabled,const char* szUsage, const char* szHelp)
{
	char szLwrName[256];
	strcpy(szLwrName, szName);
	_strlwr(szLwrName);

	ZChatCmd* pCmd = new ZChatCmd(nID, szLwrName, flag, nMinArgs, nMaxArgs, bRepeatEnabled,szUsage, szHelp);
	
	pCmd->SetProc(fnProc);
	m_CmdMap.insert(ZChatCmdMap::value_type(szLwrName, pCmd));
}

bool ZChatCmdManager::DoCommand(char* szLine, ZChatCmdFlag nCurrFlag, unsigned long nInputFlag)
{
	if ((szLine == 0) || (szLine[0] == 0)) return false;
	
	ZChatCmdArgvInfo ai;
	memset(&ai, 0, sizeof(ZChatCmdArgvInfo));

	ZChatCmd* pCmd = MakeArgv(szLine, &ai);
	if (pCmd != NULL)
	{
		// 관리자 명령어일때 처리
		if (pCmd->GetFlag() & CCF_ADMIN)
		{
			if ((nInputFlag & CIF_ADMIN) == false) return false;
		}
		// 테스트 명령어일때 처리
		else if (pCmd->GetFlag() & CCF_TEST)
		{
			if ((nInputFlag & CIF_TESTER) == false) return false;
		}

		// Common
		if ( pCmd->GetFlag() & nCurrFlag)
		{
			pCmd->OnProc(szLine, ai.cargc, ai.cargv);
			return true;
		}
		// Admin
		else if ( (CIF_ADMIN & nInputFlag) && (pCmd->GetFlag() & CCF_ADMIN))
		{
			if ( IsAdminGrade(ZGetMyInfo()->GetUGradeID()) )
			{
				pCmd->OnProc(szLine, ai.cargc, ai.cargv);
				return true;
			}
		}
		// Test
		else if ( (CIF_TESTER & nInputFlag) && (pCmd->GetFlag() & CCF_TEST))
		{
			pCmd->OnProc(szLine, ai.cargc, ai.cargv);
			return true;
		}
	}

	return false;
}

bool ZChatCmdManager::IsRepeatEnabled(char* szLine)
{
	if ((szLine == 0) || (szLine[0] == 0)) return 0;
	
	ZChatCmdArgvInfo ai;
	memset(&ai, 0, sizeof(ZChatCmdArgvInfo));

	ZChatCmd* pCmd = MakeArgv(szLine, &ai);
	if (pCmd != NULL)
	{
		return pCmd->GetRepeatEnabled();
	}

	return false;
}

ZChatCmdManager::ZChatCmdManager()
{

}

ZChatCmdManager::~ZChatCmdManager()
{
	while (!m_CmdMap.empty())
	{
		ZChatCmdMap::iterator i = m_CmdMap.begin();
		delete (*i).second;
		m_CmdMap.erase(i);
	}
	m_AliasMap.clear();
}

ZChatCmd* ZChatCmdManager::GetCommandByName(const char* szName)
{
	char szLwrName[256];
	strcpy(szLwrName, szName);
	_strlwr(szLwrName);


char szCmdName[256] = "";

// alias에서 먼저 찾는다.
map<std::string, std::string>::iterator itorAlias = m_AliasMap.find(string(szLwrName));
if (itorAlias != m_AliasMap.end())
{
	strcpy(szCmdName, ((*itorAlias).second).c_str());
}
else
{
	strcpy(szCmdName, szLwrName);
}

// 실제로 찾음
ZChatCmdMap::iterator pos = m_CmdMap.find(string(szCmdName));
if (pos != m_CmdMap.end())
{
	ZChatCmd* pCmd = (*pos).second;
	return pCmd;
}
else
{
	return NULL;
}
}

ZChatCmd* ZChatCmdManager::GetCommandByID(int nID)
{
	for (ZChatCmdMap::iterator itor = m_CmdMap.begin(); itor != m_CmdMap.end(); ++itor)
	{
		ZChatCmd* pChatCmd = (*itor).second;
		if (pChatCmd->GetID() == nID) return pChatCmd;
	}

	return NULL;
}

void ZChatCmdManager::AddAlias(const char* szNewCmdName, const char* szTarCmdName)
{
	/*
		ZChatCmd* pCmd = GetCommandByName(szTarCmdName);
		if (pCmd == NULL)
		{
			_ASSERT(0);		// 앨리어스 대상 커맨드가 존재하지 않음
			return;
		}
	*/

	char szLwrName[256];
	strcpy(szLwrName, szNewCmdName);
	_strlwr(szLwrName);

	m_AliasMap.insert(map<std::string, std::string>::value_type(std::string(szLwrName), std::string(szTarCmdName)));
}




#define ZCMD_TOK_CMD		"CMD"
#define ZCMD_TOK_ALIAS		"ALIAS"
#define ZCMD_TOK_ATTR_ID	"id"
#define ZCMD_TOK_ATTR_NAME	"name"
#define ZCMD_TOK_ATTR_USAGE	"usage"
#define ZCMD_TOK_ATTR_HELP	"help"


void ZCmdXmlParser::ParseRoot(const char* szTagName, rapidxml::xml_node<>* pElement)
{
	if (!_stricmp(szTagName, ZCMD_TOK_CMD))
	{
		ParseCmd(pElement);
	}
}

void ZCmdXmlParser::ParseCmd(rapidxml::xml_node<>* pElement)
{
	char szTemp[256] = "";
	int n = 0;

	_CmdStr* pCmdStr = new _CmdStr;

	int nID = 0;

	// 속성값 --------------------
	nID = atoi(pElement->first_attribute(ZCMD_TOK_ATTR_ID)->value());
	strcpy_s(pCmdStr->szName, ZGetStringResManager()->GetStringFromXml(pElement->first_attribute(ZCMD_TOK_ATTR_NAME)->value()));
	strcpy_s(pCmdStr->szUsage, ZGetStringResManager()->GetStringFromXml(pElement->first_attribute(ZCMD_TOK_ATTR_USAGE)->value()));
	strcpy_s(pCmdStr->szHelp, ZGetStringResManager()->GetStringFromXml(pElement->first_attribute(ZCMD_TOK_ATTR_HELP)->value()));


	rapidxml::xml_node<>* childNode = pElement->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), ZCMD_TOK_ALIAS) == 0)
		{
			if (_stricmp(itor->first_attribute()->name(), ZCMD_TOK_ATTR_NAME) == 0)
			{
				string str = ZGetStringResManager()->GetStringFromXml(itor->first_attribute()->value());
				if (str.size() > 0)
				{
					pCmdStr->vecAliases.push_back(str);
				}
			}
		}
	}
	if (nID != 0)
	{
		m_CmdMap.insert(map<int, _CmdStr*>::value_type(nID, pCmdStr));
	}
	else
	{
		delete pCmdStr;
	}
}

void ZCmdXmlParser::Clear()
{
	for (map<int, _CmdStr*>::iterator itor = m_CmdMap.begin(); itor != m_CmdMap.end(); ++itor)
	{
		delete (*itor).second;
	}

	m_CmdMap.clear();

}

ZCmdXmlParser::ZCmdXmlParser()
{

}

ZCmdXmlParser::~ZCmdXmlParser()
{
	Clear();
}

ZCmdXmlParser::_CmdStr* ZCmdXmlParser::GetStr(int nID)
{
	map<int, _CmdStr*>::iterator itor = m_CmdMap.find(nID);
	if (itor != m_CmdMap.end())
	{
		return (*itor).second;
	}

	return NULL;
}









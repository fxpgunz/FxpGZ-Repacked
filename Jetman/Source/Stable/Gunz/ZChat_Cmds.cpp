#include "stdafx.h"

#include "ZChat.h"
#include "MChattingFilter.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "ZCombatChat.h"
#include "ZCombatInterface.h"
#include "ZIDLResource.h"
#include "MListBox.h"
#include "MLex.h"
#include "MTextArea.h"
#include "ZMyInfo.h"
#include "ZMessages.h"
#include "MUtil.h"
#include "ZConfiguration.h"
#include "ZInterfaceListener.h"
#include "ZNetRepository.h"

#include "ZApplication.h"
#include "ZChat_CmdID.h"

void ChatCmd_Test(const char* line, const int argc, char **const argv);				// Testing

void ChatCmd_Help(const char* line, const int argc, char **const argv);				// 도움말
void ChatCmd_Go(const char* line, const int argc, char **const argv);					// 게임방 이동
void ChatCmd_Whisper(const char* line, const int argc, char **const argv);			// 귓속말
void ChatCmd_CreateChatRoom(const char* line, const int argc, char **const argv);		// 채팅방 개설
void ChatCmd_JoinChatRoom(const char* line, const int argc, char **const argv);		// 채팅방 참여
void ChatCmd_LeaveChatRoom(const char* line, const int argc, char **const argv);		// 채팅방 탈퇴
void ChatCmd_SelectChatRoom(const char* line, const int argc, char **const argv);		// 채팅방 선택
void ChatCmd_InviteChatRoom(const char* line, const int argc, char **const argv);		// 채팅방 초대
void ChatCmd_VisitChatRoom(const char* line, const int argc, char **const argv);		// 채팅방 초대 참석
void ChatCmd_ChatRoomChat(const char* line, const int argc, char **const argv);		// 채팅
void ChatCmd_CopyToTestServer(const char* line, const int argc, char **const argv);	// 캐릭터전송 - 테스트 서버로 캐릭터 전송
void ChatCmd_StageFollow(const char* line, const int argc, char **const argv);		// 플레이어 추적해서 게임참가
void ChatCmd_Friend(const char* line, const int argc, char **const argv);				// 친구 관련
void ChatCmd_Clan(const char* line, const int argc, char **const argv);				// 클란 관련
void getPosition(const char* line, const int argc, char **const argv);
void getDirection(const char* line, const int argc, char ** const argv);

void ChatCmd_RequestQuickJoin(const char* line, const int argc, char **const argv);		// 퀵조인
void ChatCmd_Report119(const char* line, const int argc, char **const argv);				// 신고
void ChatCmd_RequestPlayerInfo(const char* line, const int argc, char **const argv);		// 사용자 정보보기
void ChatCmd_Macro(const char* line,const int argc, char **const argv);

// 게임안에서만 가능한 명령어
void ChatCmd_EmotionTaunt(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionBow(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionWave(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionLaugh(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionCry(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionDance(const char* line,const int argc, char **const argv);
void ChatCmd_Suicide(const char* line,const int argc, char **const argv);
void ChatCmd_Callvote(const char* line,const int argc, char **const argv);
void ChatCmd_VoteYes(const char* line,const int argc, char **const argv);
void ChatCmd_VoteNo(const char* line,const int argc, char **const argv);
void ChatCmd_Kick(const char* line,const int argc, char **const argv);
void ChatCmd_MouseSensitivity(const char* line,const int argc, char **const argv);

// test
void ChatCmd_LadderInvite(const char* line,const int argc, char **const argv);
void ChatCmd_LadderTest(const char* line,const int argc, char **const argv);
void ChatCmd_LaunchTest(const char* line,const int argc, char **const argv);

// 퀘스트 테스트용 명령어
void ChatCmd_QUESTTEST_God(const char* line,const int argc, char **const argv);				// 무적모드
void ChatCmd_QUESTTEST_SpawnNPC(const char* line,const int argc, char **const argv);		// NPC 스폰
void ChatCmd_QUESTTEST_NPCClear(const char* line,const int argc, char **const argv);		// NPC 클리어
void ChatCmd_QUESTTEST_Reload(const char* line,const int argc, char **const argv);			// 리소스 리로드
void ChatCmd_QUESTTEST_SectorClear(const char* line,const int argc, char **const argv);		// 섹터 클리어
void ChatCmd_QUESTTEST_Finish(const char* line,const int argc, char **const argv);			// 퀘스트 클리어
void ChatCmd_QUESTTEST_LocalSpawnNPC(const char* line,const int argc, char **const argv);	// npc 스폰 (local)
void ChatCmd_QUESTTEST_WeakNPCs(const char* line,const int argc, char **const argv);		// NPC 에너지를 1로

// admin 명령
void ChatCmd_AdminKickPlayer(const char* line, const int argc, char **const argv);		// 디스커넥트 시킴
void ChatCmd_AdminMutePlayer(const char* line, const int argc, char **const argv);		// 채팅 금지 시킴
void ChatCmd_AdminBlockPlayer(const char* line, const int argc, char **const argv);		// 접속 금지 시킴

void ChatCmd_AdminPingToAll(const char* line, const int argc, char **const argv);		// Garbage Connection 청소 확인
void ChatCmd_AdminAnnounce(const char* line, const int argc, char **const argv);		// 전체 공지
void ChatCmd_AdminServerHalt(const char* line, const int argc, char **const argv);		// 서버 종료
void ChatCmd_AdminSwitchCreateLadderGame(const char* line, const int argc, char **const argv);		// 클랜전 여부 설정
void ChatCmd_AdminReloadClientHash(const char* line, const int argc, char **const argv);		// XTrap 클라이언트 Hash 리로드
void ChatCmd_AdminResetAllHackingBlock( const char* line, const int argc, char **const argv );
void ChatCmd_AdminReloadGambleitem( const char* line, const int argc, char **const argv );
void ChatCmd_AdminDumpGambleitemLog( const char* line, const int argc, char **const argv );
void ChatCmd_AdminAssasin( const char* line, const int argc, char **const argv );
void ChatCmd_AdminUpdateCharacter( const char* line, const int argc, char **const argv );
void ChatCmd_AdminSetCharacter( const char* line, const int argc, char **const argv );
void ChatCmd_AdminSlap(const char* line, const int argc, char **const argv);
void ChatCmd_Framed(const char* line, const int argc, char **const argv);

// event 진행운영자 명령
void ChatCmd_ChangeMaster(const char* line, const int argc, char **const argv);			// 방장권한 빼앗어옴
void ChatCmd_ChangePassword(const char* line, const int argc, char **const argv);		// 방 패스워드 바꿈
void ChatCmd_AdminHide(const char* line, const int argc, char **const argv);			// 투명인간
void ChatCmd_RequestJjang(const char* line, const int argc, char **const argv);
void ChatCmd_AdminTeleport(const char* line, const int argc, char **const argv);
void ChatCmd_AdminMoveTo(const char* line, const int argc, char **const argv);
void ChatCmd_RemoveJjang(const char* line, const int argc, char **const argv);
///////////////////////////////////////////////////////////////////////////////////////////////

void _AddCmdFromXml(ZChatCmdManager* pCmdManager, ZCmdXmlParser* pParser, 
				int nCmdID, ZChatCmdProc* fnProc, unsigned long int flag,
				int nMinArgs, int nMaxArgs, bool bRepeatEnabled)
{
	ZCmdXmlParser::_CmdStr* pCmdStr = pParser->GetStr(nCmdID);
	if (pCmdStr)
	{
		pCmdManager->AddCommand(nCmdID, pCmdStr->szName, fnProc, flag, nMinArgs, nMaxArgs, bRepeatEnabled, 
			pCmdStr->szUsage, pCmdStr->szHelp);

		for (int i = 0; i < (int)pCmdStr->vecAliases.size(); i++)
		{
			pCmdManager->AddAlias(pCmdStr->vecAliases[i].c_str(), pCmdStr->szName);
		}
	}
}

#define _CC_AC(X1,X2,X3,X4,X5,X6,X7,X8)		m_CmdManager.AddCommand(0,X1,X2,X3,X4,X5,X6,X7,X8)
#define _CC_ALIAS(NEW,TAR)					m_CmdManager.AddAlias(NEW,TAR)
#define _CC_ACX(X1,X2,X3,X4,X5,X6)			_AddCmdFromXml(&m_CmdManager,&parser,X1,X2,X3,X4,X5,X6)

// 채팅명령어 사용법은 http://iworks.maietgames.com/mdn/wiki.php/건즈;채팅명령어 에 적어주세요.. by bird

void ZChat::InitCmds()
{	
	ZCmdXmlParser parser;
	if (!parser.ReadXml(FILENAME_CHATCMDS, ZApplication::GetFileSystem()))
	{
		MLog("Error while Read Item Descriptor %s", "system/chatcmds.xml");
	}

	_CC_ACX(CCMD_ID_HELP,				&ChatCmd_Help,				CCF_ALL, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_WHISPER,			&ChatCmd_Whisper,			CCF_ALL, ARGVNoMin, 1, false);
	_CC_ACX(CCMD_ID_REPORT119,			&ChatCmd_Report119,			CCF_ALL, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_FRIEND,				&ChatCmd_Friend,			CCF_ALL, ARGVNoMin, 1, false);
	_CC_ACX(CCMD_ID_CLAN,				&ChatCmd_Clan,				CCF_ALL, ARGVNoMin, ARGVNoMax, false);
	_CC_ACX(CCMD_ID_STAGE_FOLLOW,		&ChatCmd_StageFollow,		CCF_LOBBY, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_REQUEST_QUICK_JOIN,	&ChatCmd_RequestQuickJoin,	CCF_LOBBY, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_EMOTION_TAUNT,		&ChatCmd_EmotionTaunt,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_BOW,		&ChatCmd_EmotionBow  ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_WAVE,		&ChatCmd_EmotionWave ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_LAUGH,		&ChatCmd_EmotionLaugh,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_CRY,		&ChatCmd_EmotionCry  ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_DANCE,		&ChatCmd_EmotionDance,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_MACRO,				&ChatCmd_Macro,				CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_SUICIDE,			&ChatCmd_Suicide,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_CALLVOTE,			&ChatCmd_Callvote,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_VOTEYES,			&ChatCmd_VoteYes,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_VOTENO,				&ChatCmd_VoteNo,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_KICK,				&ChatCmd_Kick,				CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_CREATE_CHATROOM,	&ChatCmd_CreateChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_JOIN_CHATROOM,		&ChatCmd_JoinChatRoom  ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_LEAVE_CHATROOM,		&ChatCmd_LeaveChatRoom ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_SELECT_CHATROOM,	&ChatCmd_SelectChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_INVITE_CHATROOM,	&ChatCmd_InviteChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_VISIT_CHATROOM,		&ChatCmd_VisitChatRoom ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_CHAT_CHATROOM,		&ChatCmd_ChatRoomChat  ,	CCF_ALL, ARGVNoMin, 1,false);
	_CC_ACX(CCMD_ID_MOUSE_SENSITIVITY,	&ChatCmd_MouseSensitivity,	CCF_GAME, ARGVNoMin, 1,true);

////////////////////////////////////////////////////////////////////
	// admin
	_CC_AC("admin_kick",	    &ChatCmd_AdminKickPlayer,	    CCF_ADMIN, ARGVNoMin, 1, true, "/admin_kick <charname>", "");			// 2010-08-09 수정됨 - 홍기주
	_CC_AC("admin_mute",	    &ChatCmd_AdminMutePlayer,	    CCF_ADMIN, ARGVNoMin, 2, true, "/admin_mute <charname>", "");		// 2010-08-09 수정됨 - 홍기주
	_CC_AC("admin_block",	    &ChatCmd_AdminBlockPlayer,	    CCF_ADMIN, ARGVNoMin, 2, true, "/admin_block <charname>", "");	// 2010-08-09 수정됨 - 홍기주	
	_CC_AC("admin_moveto", &ChatCmd_AdminMoveTo, CCF_ADMIN, ARGVNoMin, 3, true, "", "");
	_CC_AC("admin_teleport",    &ChatCmd_AdminTeleport,         CCF_ADMIN|CCF_GAME, ARGVNoMin, 1, true, "/admin_tp <player>", "");
	_CC_AC("admin_pingtoall",	&ChatCmd_AdminPingToAll,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true,"/admin_pingtoall", "");
	_CC_AC("admin_wall",		&ChatCmd_AdminAnnounce,			CCF_ADMIN, ARGVNoMin, 1 , true,"/admin_wall <메시지>", "");
	_CC_AC("changemaster",		&ChatCmd_ChangeMaster,			CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/changemaster", "");
	_CC_AC("changepassword",	&ChatCmd_ChangePassword,		CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/changepassword", "");
	_CC_AC("admin_hide",		&ChatCmd_AdminHide,				CCF_ADMIN|CCF_LOBBY, ARGVNoMin, ARGVNoMax, true,"/admin_hide", "");
	_CC_AC("hide",				&ChatCmd_AdminHide,				CCF_ADMIN|CCF_LOBBY, ARGVNoMin, ARGVNoMax, true,"/hide", "");
	_CC_AC("jjang",				&ChatCmd_RequestJjang,			CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/jjang", "");
	_CC_AC("removejjang",		&ChatCmd_RemoveJjang,			CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/removejjang", "");
	_CC_AC("admin_reload_hash", &ChatCmd_AdminReloadClientHash,	CCF_ADMIN, ARGVNoMin, ARGVNoMax, true,"/admin_reload_hash", "");

	_CC_AC("admin_switch_laddergame",		&ChatCmd_AdminSwitchCreateLadderGame,	CCF_ADMIN, ARGVNoMin, ARGVNoMax, true,"/admin_switch_laddergame 1", "");		
	_CC_AC("admin_reset_all_hacking_block", &ChatCmd_AdminResetAllHackingBlock,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reset_all_hacking_block", "");
	_CC_AC("admin_reload_gambleitem",		&ChatCmd_AdminReloadGambleitem,			CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reload_gambleitem", "");
	_CC_AC("admin_dump_gambleitem_log",		&ChatCmd_AdminDumpGambleitemLog,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_dump_gambleitem_log", "");
	_CC_AC("admin_commander",				&ChatCmd_AdminAssasin,					CCF_ADMIN|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/admin_commander", "");
	_CC_AC("updatechar",					&ChatCmd_AdminUpdateCharacter,					CCF_ADMIN|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/updatechar", "");
	_CC_AC("setchar",						&ChatCmd_AdminSetCharacter,					CCF_ADMIN|CCF_GAME, 2, ARGVNoMax, true, "/setchar", "");
	_CC_AC("getpos", &getPosition, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "","");
	_CC_AC("getdir", &getDirection, CCF_ADMIN | CCF_GAME, ARGVNoMin, ARGVNoMax, true, "", "");
	_CC_AC("admin_slap", &ChatCmd_AdminSlap, CCF_ADMIN | CCF_GAME, 1, ARGVNoMax, true, "/admin_slap","");
	_CC_ALIAS("공지", "admin_wall");
	_CC_AC("wireframe", &ChatCmd_Framed, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/wireframe", "");


#ifdef _DEBUG
	//_CC_AC("캐릭터전송", &ChatCmd_CopyToTestServer, CCF_LOBBY, ARGVNoMin, 0, "/캐릭터전송", "캐릭터 정보를 테스트서버에 복사합니다.");
	_CC_AC("team", &ChatCmd_LadderInvite, CCF_LOBBY, ARGVNoMin, ARGVNoMax, true, "", "");
	_CC_AC("test", &ChatCmd_Test, CCF_ALL, ARGVNoMin, 1, true ,"/test <name>", "테스트를 수행합니다.");
	_CC_AC("laddertest", &ChatCmd_LadderTest, CCF_ADMIN|CCF_ALL, ARGVNoMin, ARGVNoMax, true ,"/laddertest", "래더테스트를 요청합니다.");
	_CC_AC("launchtest", &ChatCmd_LaunchTest, CCF_ADMIN|CCF_ALL, ARGVNoMin, ARGVNoMax, true ,"/laddertest", "래더테스트를 요청합니다.");
#endif

	_CC_AC("go",		&ChatCmd_Go, CCF_LOBBY, ARGVNoMin, 1, true, "/go 방번호", "게임방으로 바로 이동합니다.");

#ifndef _PUBLISH
{

	// 아직 다 구현되지 않았음
	_CC_AC("pf",		&ChatCmd_RequestPlayerInfo, CCF_LOBBY|CCF_STAGE, ARGVNoMin, 2, true ,"/pf <캐릭터이름>", "다른 사용자의 정보를 봅니다.");
	_CC_ALIAS("ㅔㄹ", "pf");
}
#endif

	// 테스트 전용
/*	_CC_AC("gtgod",			&ChatCmd_QUESTTEST_God,				CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtgod", "");
	_CC_AC("gtspn",			&ChatCmd_QUESTTEST_SpawnNPC,		CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtspn <NPC타입> <NPC수>", "");
	_CC_AC("gtclear",		&ChatCmd_QUESTTEST_NPCClear,		CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtclear", "");
	_CC_AC("gtreload",		&ChatCmd_QUESTTEST_Reload,			CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtreload", "");
	_CC_AC("gtsc",			&ChatCmd_QUESTTEST_SectorClear,		CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtsc", "");
	_CC_AC("gtfin",			&ChatCmd_QUESTTEST_Finish,			CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtfin", "");
	//_CC_AC("gtlspn",		&ChatCmd_QUESTTEST_LocalSpawnNPC,	CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtlspn <NPC타입> <NPC수>", "");
	_CC_AC("gtweaknpcs",	&ChatCmd_QUESTTEST_WeakNPCs,		CCF_ADMIN, ARGVNoMin, ARGVNoMax    , true,"/gtweaknpcs", "");*/
}

void getPosition(const char* line, const int argc, char **const argv)
{
	rvector Pos = ZGetGame()->m_pMyCharacter->GetPosition();
	char output[64];
	sprintf_s(output, "Char Position - (%f %f %f)", Pos.x, Pos.y, Pos.z);
	ZChatOutput(output);
}

void getDirection(const char* line, const int argc, char **const argv)
{
	rvector Pos = ZGetGame()->m_pMyCharacter->GetDirection();
	char output[64];
	sprintf_s(output, "Char Direction - (%f %f %f)", Pos.x, Pos.y, Pos.z);
	ZChatOutput(output);
}

void OutputCmdHelp(const char* cmd)
{
	//ZChatOutput("Commands:", ZChat::CMT_SYSTEM);
	//ZChatOutput("--------------------------------", ZChat::CMT_SYSTEM);
	ZChatOutput("Room mods:", ZChat::CMT_SYSTEM);
	ZChatOutput("Tags are In the GameroomOptions", ZChat::CMT_SYSTEM);
	//ZChatOutput("[IA] - Infinite Ammo", ZChat::CMT_SYSTEM);
	//ZChatOutput("[R] - Instant Reload", ZChat::CMT_SYSTEM);
	//ZChatOutput("[V] - Vanilla Mode", ZChat::CMT_SYSTEM);
	//ZChatOutput("[PH] - Permanent Bullet Holes", ZChat::CMT_SYSTEM);
	//ZChatOutput("[NM] - No massive", ZChat::CMT_SYSTEM);
	//ZChatOutput("/sound - Enable 3d Sound", ZChat::CMT_SYSTEM);
	/*ZChatOutput("[NC] - No Clothing", ZChat::CMT_SYSTEM);
	ZChatOutput("[NS] - No Sword", ZChat::CMT_SYSTEM);
	ZChatOutput("[NG] - No Guns", ZChat::CMT_SYSTEM);
	ZChatOutput("[NI] - No Meds/Nades", ZChat::CMT_SYSTEM);*/
	

}


void OutputCmdUsage(const char* cmd)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();
	ZChatCmd* pCmd = pCCM->GetCommandByName(cmd);
	if (pCmd == NULL) return;
	if ( (pCmd->GetFlag() & CCF_ADMIN) && !ZGetMyInfo()->IsAdminGrade())
		return;

    char szBuf[512];
	sprintf(szBuf, "%s: %s", ZMsg(MSG_WORD_USAGE), pCmd->GetUsage());
	ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
}

void OutputCmdWrongArgument(const char* cmd)
{
	ZChatOutput( ZMsg(MSG_WRONG_ARGUMENT) );
	OutputCmdUsage(cmd);
}


///////////////////////////////////////////////////////////////////////////////////////////////

void ChatCmd_Help(const char* line, const int argc, char **const argv)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();

	char szBuf[1024] = "";

	//////////////////////////////////////////////////////////

	GunzState state = ZApplication::GetGameInterface()->GetState();

	if( state==GUNZ_GAME ) {
		// 자신의 캐릭터가 레벨이 1-10 사이인 경우만..
		if(ZGetMyInfo()) {
			if(ZGetMyInfo()->GetLevel() < 10) {
				if( ZGetGame() ) {
					ZGetGame()->m_HelpScreen.ChangeMode();
					return;
				}
			}
		}
	}

	if (argc == 1)
	{
		
		ZChatCmdFlag nCurrFlag = CCF_NONE;

		switch (state)
		{
			case GUNZ_LOBBY: nCurrFlag = CCF_LOBBY; break;
			case GUNZ_STAGE: nCurrFlag = CCF_STAGE; break;
			case GUNZ_GAME: nCurrFlag = CCF_GAME; break;
		}

		sprintf(szBuf, "%s: ", ZMsg(MSG_WORD_TAGS));

		int nCnt=0;
		int nCmdCount = pCCM->GetCmdCount();

		for (ZChatCmdMap::iterator itor = pCCM->GetCmdBegin(); itor != pCCM->GetCmdEnd(); ++itor)
		{
			nCnt++;
			ZChatCmd* pCmd = (*itor).second;

			if (pCmd->GetFlag() & CCF_ADMIN) continue;
			if (!(pCmd->GetFlag() & nCurrFlag)) continue;

			strcat(szBuf, pCmd->GetName());

			if (nCnt != nCmdCount) strcat(szBuf, ", ");
		}

		// 엄청난 하드코딩... 어쩔수 없다... -ㅇ-;
		switch (state)
		{
			case GUNZ_LOBBY:
				strcat( szBuf, "go");
				break;
			case GUNZ_STAGE:
				strcat( szBuf, "kick");
				break;
			case GUNZ_GAME:
				break;
		}

		ZChatOutput(szBuf, ZChat::CMT_SYSTEM);

		sprintf(szBuf, "%s: /h %s", ZMsg(MSG_WORD_HELP), ZMsg(MSG_WORD_TAGS));
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
	}
	else if (argc == 2)
	{
		OutputCmdHelp(argv[1]);
		OutputCmdUsage(argv[1]);
	}
}

void ChatCmd_Go(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;

	ZRoomListBox* pRoomList = (ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (pRoomList == NULL)
		return;

	int nRoomNo = atoi(argv[1]);

	ZPostStageGo(nRoomNo);
}

void ChatCmd_Whisper(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT) );
		return;
	}


	char* pszSenderName = "Me";	// 아무거나 보내도 서버에서 채워넣음

	char szName[512] = "";
	char szRName[512] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szName, szRName);

	if ( (int)strlen( pszMsg) > 127)
		return;

	// 욕필터링
	if (!ZGetGameInterface()->GetChat()->CheckChatFilter(pszMsg)) return;

	//귓속말 캐릭터 이름 글자 수 제한..jintriple3
	int nNameLen = (int)strlen(szName);
	if ( nNameLen < MIN_CHARNAME)		// 이름이 너무 짧다.
	{
		const char *str = ZErrStr( MERR_TOO_SHORT_NAME );
		if(str)
		{
			char text[1024];
			sprintf(text, "%s (E%d)", str, MERR_TOO_SHORT_NAME);
			ZChatOutput(MCOLOR(96,96,168), text, ZChat::CL_CURRENT);
		}
	}
	else if ( nNameLen > MAX_CHARNAME)		// 이름이 제한 글자수를 넘었다.
	{
		const char *str = ZErrStr( MERR_TOO_LONG_NAME );
		if(str)
		{
			char text[1024];
			sprintf(text, "%s (E%d)", str, MERR_TOO_LONG_NAME);
			ZChatOutput(MCOLOR(96,96,168), text, ZChat::CL_CURRENT);
		}
	}
	else
	{
		ZPostWhisper(pszSenderName, szName, pszMsg);

		// loop back
		char szMsg[512];
		sprintf(szMsg, "(To %s) : %s", szRName, pszMsg);	//jintriple3 유저 네임은 그대로 출력되도록...
		ZChatOutput(MCOLOR(96,96,168), szMsg, ZChat::CL_CURRENT);
	}
}

void ChatCmd_CreateChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszCharName = argv[1];

	if( !MGetChattingFilter()->IsValidStr( pszCharName, 1) ){
		char szMsg[ 256 ];
		ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr());
		ZApplication::GetGameInterface()->ShowMessage( szMsg, NULL, MSG_WRONG_WORD_NAME );
	}
	else
	{
		ZChatOutput( 
			ZMsg(MSG_LOBBY_REQUESTING_CREATE_CHAT_ROOM), 
			ZChat::CMT_SYSTEM );

		ZPostChatRoomCreate(ZGetMyUID(), pszCharName);
	}
}
void ChatCmd_JoinChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_REQUESTING_JOIN_CAHT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszChatRoomName = argv[1];

	ZPostChatRoomJoin(pszChatRoomName);
}

void ChatCmd_LeaveChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_LEAVE_CHAT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszRoomName = argv[1];

	ZPostChatRoomLeave(pszRoomName);
}

void ChatCmd_SelectChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_CHOICE_CHAT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszRoomName = argv[1];

	ZPostSelectChatRoom(pszRoomName);
}

void ChatCmd_InviteChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszPlayerName = argv[1];

	char szLog[128];
	
	ZTransMsg( szLog, MSG_LOBBY_INVITATION, 1, pszPlayerName );

	ZChatOutput(szLog, ZChat::CMT_SYSTEM);

	ZPostInviteChatRoom(pszPlayerName);
}

void ChatCmd_VisitChatRoom(const char* line, const int argc, char **const argv)
{
	char* pszRoomName = const_cast<char*>(ZGetGameClient()->GetChatRoomInvited());
	ZPostChatRoomJoin(pszRoomName);
}

void ChatCmd_ChatRoomChat(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszMsg = argv[1];

	// 욕필터링
	if (!ZGetGameInterface()->GetChat()->CheckChatFilter(pszMsg)) return;

	ZPostChatRoomChat(pszMsg);
}


void ChatCmd_CopyToTestServer(const char* line, const int argc, char **const argv)
{
	// 사용하지 않는다. - 테스트 서버로 정보 복사
	return;


	if (argc != 1) return;

	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();

#define DELAY_POST_COPY_TO_TESTSERVER		(1000 * 60)		// 5분 딜레이

	if ((nNowTime - st_nLastTime) > DELAY_POST_COPY_TO_TESTSERVER)
	{
		ZPostRequestCopyToTestServer(ZGetGameClient()->GetPlayerUID());

		st_nLastTime = nNowTime;
	}
}


void ChatCmd_StageFollow(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];

	ZPostStageFollow(pszTarget);
}

void ChatCmd_Friend(const char* line, const int argc, char **const argv)
{
	//// ZFriendCommandHelper ////
	class ZFriendCommandHelper {
	public:
		enum ZFRIENDCMD {
			ZFRIENDCMD_ADD,
			ZFRIENDCMD_REMOVE,
			ZFRIENDCMD_LIST,
			ZFRIENDCMD_MSG,
			ZFRIENDCMD_UNKNOWN
		};
		ZFRIENDCMD GetSubCommand(const char* pszCmd) {
			if (_stricmp(pszCmd, "Add") == 0)
				return ZFRIENDCMD_ADD;
			else if (_stricmp(pszCmd, "추가") == 0)
				return ZFRIENDCMD_ADD;
			else if (_stricmp(pszCmd, "Remove") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (_stricmp(pszCmd, "삭제") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (_stricmp(pszCmd, "list") == 0)
				return ZFRIENDCMD_LIST;
			else if (_stricmp(pszCmd, "목록") == 0)
				return ZFRIENDCMD_LIST;
			else if (_stricmp(pszCmd, "msg") == 0)
				return ZFRIENDCMD_MSG;
			else if (_stricmp(pszCmd, "채팅") == 0)
				return ZFRIENDCMD_MSG;
			else return ZFRIENDCMD_UNKNOWN;
		}
	} friendHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";
	char szArg[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(friendHelper.GetSubCommand(szSubCmd)) {
	case ZFriendCommandHelper::ZFRIENDCMD_ADD:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendAdd(szArg);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_REMOVE:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendRemove(szArg);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_LIST:
		{
			ZPostFriendList();
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_MSG:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendMsg(szArg);
		}
		break;
	default:
		OutputDebugString("Unknown Friend Command \n");
		break;
	};
}

void ChatCmd_Clan(const char* line, const int argc, char **const argv)
{
	

	//// ZClanCommandHelper ////
	class ZClanCommandHelper {
	public:
		enum ZCLANCMD {
			ZCLANCMD_CREATE,		// 클랜 생성
			ZCLANCMD_CLOSE,			// 클랜 폐쇄
			ZCLANCMD_JOIN,
			ZCLANCMD_LEAVE,
			ZCLANCMD_EXPEL_MEMBER,	// 강제탈퇴
			ZCLANCMD_LIST,
			ZCLANCMD_MSG,
			
			ZCLANCMD_CHANGE_GRADE,	// 멤버 권한 변경

			ZCLANCMD_UNKNOWN
		};
		ZCLANCMD GetSubCommand(const char* pszCmd) {
			GunzState nGameState = ZApplication::GetGameInterface()->GetState();

			if ((_stricmp(pszCmd, "생성") == 0) || (_stricmp(pszCmd, "open") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CREATE;
			}
			else if ((_stricmp(pszCmd, "폐쇄") == 0) || (_stricmp(pszCmd, "해체") == 0) || (_stricmp(pszCmd, "close") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CLOSE;
			}
			else if ( (_stricmp(pszCmd, "초대") == 0) || (_stricmp(pszCmd, "invite") == 0) )
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_JOIN;
			}
			else if ( (_stricmp(pszCmd, "탈퇴") == 0) || (_stricmp(pszCmd, "leave") == 0) )
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_LEAVE;
			}
			else if ( (_stricmp(pszCmd, "권한변경") == 0) || (_stricmp(pszCmd, "promote") == 0) )
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CHANGE_GRADE;
			}
			else if ((_stricmp(pszCmd, "강제탈퇴") == 0) || (_stricmp(pszCmd, "방출") == 0) || (_stricmp(pszCmd, "dismiss") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_EXPEL_MEMBER;
			}
			else if ((_stricmp(pszCmd, "list") == 0) || (_stricmp(pszCmd, "목록") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_LIST;
			}
			else if ((_stricmp(pszCmd, "msg") == 0) || (_stricmp(pszCmd, "채팅") == 0))
			{
				return ZCLANCMD_MSG;
			}
			
			return ZCLANCMD_UNKNOWN;
		}
	} clanHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(clanHelper.GetSubCommand(szSubCmd)) {
	case ZClanCommandHelper::ZCLANCMD_CREATE:
		{
			// clan 생성 클랜이름 발기인1 발기인2 발기인3 발기인4
			if (argc < 7)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			ZGetGameClient()->RequestCreateClan(argv[2], &argv[3]);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_CLOSE:
		{
			// clan 폐쇄 클랜이름
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (ZGetMyInfo()->GetClanGrade() != MCG_MASTER)
			{
				ZChatOutput( 
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}

			// 클랜이름 확인
			if (_stricmp(ZGetMyInfo()->GetClanName(), argv[2]))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_WRONG_CLANNAME), 
					ZChat::CMT_SYSTEM );
				break;
			}

			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CLAN_CONFIRM_CLOSE), 
				ZGetClanCloseConfirmListenter()	);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_JOIN:
		{
			// clan 초대 가입자이름
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_ADMIN))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER_AND_ADMIN), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szClanName[256];
			strcpy(szClanName, 	ZGetMyInfo()->GetClanName());
			ZPostRequestJoinClan(ZGetGameClient()->GetPlayerUID(), szClanName, argv[2]);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_LEAVE:
		{
			// clan 탈퇴
			if (argc < 2)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			// 마스터는 탈퇴가 안된다.
			if (IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_MASTER))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_MASTER_CANNOT_LEAVED), 
					ZChat::CMT_SYSTEM);
				break;
			}

			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CLAN_CONFIRM_LEAVE), 
				ZGetClanLeaveConfirmListenter() );
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_CHANGE_GRADE:
		{
			// clan 권한변경 멤버이름 권한이름
			if (argc < 4)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_MASTER))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szMember[256];
			int nClanGrade = 0;

			strcpy(szMember, argv[2]);
			if ((strlen(szMember) < 0) || (strlen(szMember) > CLAN_NAME_LENGTH))
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if ((!_stricmp(argv[3], "클랜운영자")) || (!_stricmp(argv[3], "운영자")) || (!_stricmp(argv[3], "영자")) || (!_stricmp(argv[3], "admin")))
			{
				nClanGrade = (int)MCG_ADMIN;
			}
			else if ((!_stricmp(argv[3], "클랜멤버")) || (!_stricmp(argv[3], "멤버")) || (!_stricmp(argv[3], "클랜원")) || (!_stricmp(argv[3], "member")))
			{
				nClanGrade = (int)MCG_MEMBER;
			}
			else
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}


			ZPostRequestChangeClanGrade(ZGetGameClient()->GetPlayerUID(), szMember, nClanGrade);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_EXPEL_MEMBER:
		{
			// clan 강제탈퇴 클랜멤버
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_ADMIN))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER_AND_ADMIN), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szMember[256];
			int nClanGrade = 0;

			strcpy(szMember, argv[2]);
			if ((strlen(szMember) < 0) || (strlen(szMember) > CLAN_NAME_LENGTH))
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			ZPostRequestExpelClanMember(ZGetGameClient()->GetPlayerUID(), szMember);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_LIST:
		{

		}
		break;
	case ZClanCommandHelper::ZCLANCMD_MSG:
		{
			if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
			{
				ZChatOutput( ZMsg(MSG_CANNOT_CHAT) );
				break;
			}

			// clan msg 하고싶은말
			MLex lex;
			
			char szLine[512], szTemp1[256] = "", szTemp2[256] = "";
			strcpy(szLine, line);

			char* pszMsg = lex.GetTwoArgs(szLine, szTemp1, szTemp2);

			ZPostClanMsg(ZGetGameClient()->GetPlayerUID(), pszMsg);
		}
		break;
	default:
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT) );
		break;
	};
}

void ChatCmd_RequestQuickJoin(const char* line, const int argc, char **const argv)
{
	ZGetGameInterface()->RequestQuickJoin();
}

void ChatCmd_Report119(const char* line, const int argc, char **const argv)
{
	ZPostLocalReport119();
}

void ChatCmd_AdminKickPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];
	ZPostAdminRequestKickPlayer(pszPlayerName);
}

int GetDueHour(char* pszDue)
{
	int nLength = (int)strlen(pszDue);

	for(int i = 0; i < nLength - 1; i++ ) {
		if( pszDue[i] > '9' || pszDue[i] < '0') {
			return -1;
		}
	}

	int nDueType = toupper(pszDue[nLength - 1]);
	if( nDueType == toupper('d') ) {
		int nDay = atoi(pszDue);
		if( nDay < 365 * 10)	return nDay * 24;
		else					return -1;		
	} 
	else if( nDueType == toupper('h')) {
		int nHour = atoi(pszDue);
		return nHour;
	} 
	else {
		return -1;
	}
}

void ChatCmd_AdminMutePlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];

	int nDueHour = GetDueHour(argv[2]);
	if( nDueHour < 0 ) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Mute on Player(%s) While %d Hour\n", pszPlayerName, nDueHour);
#endif

	ZPostAdminRequestMutePlayer(pszPlayerName, nDueHour);
}

void ChatCmd_AdminBlockPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];

	int nDueHour = GetDueHour(argv[2]);
	if( nDueHour < 0 ) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Block on Player(%s) While %d Hour\n", pszPlayerName, nDueHour);
#endif

	ZPostAdminRequestBlockPlayer(pszPlayerName, nDueHour);
}

void ChatCmd_AdminPingToAll(const char* line, const int argc, char **const argv)
{
	ZPostAdminPingToAll();
}

void ChatCmd_AdminReloadClientHash(const char* line, const int argc, char **const argv)
{
	ZPostAdminReloadClientHash();
}


void ChatCmd_AdminResetAllHackingBlock( const char* line, const int argc, char **const argv )
{
	ZPostAdminResetAllHackingBlock();
}

void ChatCmd_AdminReloadGambleitem( const char* line, const int argc, char **const argv )
{
	ZPostAdminReloadGambleItem();
}


void ChatCmd_AdminDumpGambleitemLog( const char* line, const int argc, char **const argv )
{
	ZPostAdminDumpGambleItemLog();
}

void ChatCmd_AdminAssasin( const char* line, const int argc, char **const argv )
{
	ZPostAdminAssasin();
}

void ChatCmd_AdminUpdateCharacter( const char* line, const int argc, char **const argv )
{
	if(ZGetGameInterface()->GetState() != GunzState::GUNZ_GAME)
	{
		ZGetGameClient()->OutputMessage_Sub("Must be In-Game.");
		return;
	}

	ZGetGame()->PostUpdateCharacter();
}

void ChatCmd_AdminSetCharacter( const char* line, const int argc, char **const argv )
{
	if(ZGetGameInterface()->GetState() != GunzState::GUNZ_GAME)
	{
		ZGetGameClient()->OutputMessage_Sub("Must be In-Game.");
		return;
	}	
	
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char pszName[16];
	strcpy(pszName,argv[1]);


	for (MMatchPeerInfoList::iterator itor = ZGetGameClient()->GetPeers()->begin() ; itor != ZGetGameClient()->GetPeers()->end(); ++itor)
	{
		MMatchPeerInfo * tInfo = (*itor).second;
		if(!_stricmp(tInfo->CharInfo.szName, pszName))
		{
			MTD_CharInfo* pCharInfo = new MTD_CharInfo(tInfo->CharInfo);
			if(!pCharInfo)
				return;
			ZCharacter* myChar = ZGetGame()->m_pMyCharacter;
			if(!myChar)
				return;

			myChar->Load(pCharInfo);

			ZGetGame()->PostUpdateCharacter();

			delete pCharInfo;
			break;
		}
	}
}


void ChatCmd_ChangeMaster(const char* line, const int argc, char **const argv)
{
	ZPostChangeMaster();
}

void ChatCmd_ChangePassword(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszPassword = argv[1];

	ZPostChangePassword(pszPassword);
}

void ChatCmd_AdminHide(const char* line, const int argc, char **const argv)
{
	ZPostAdminHide();
}

void ChatCmd_RequestJjang(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTargetName = argv[1];

	ZPostAdminRequestJjang(pszTargetName);
}

void ChatCmd_RemoveJjang(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTargetName = argv[1];

	ZPostAdminRemoveJjang(pszTargetName);
}

void ChatCmd_Test(const char* line, const int argc, char **const argv)
{
	ZChatOutput("Testing...", ZChat::CMT_SYSTEM);
	
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszCharName = argv[1];

	ZGameClient* pClient = ZGetGameClient();
	MMatchPeerInfoList* pList = pClient->GetPeers();
	for (MMatchPeerInfoList::iterator i=pList->begin(); i!= pList->end(); i++) {
		MMatchPeerInfo* pInfo = (*i).second;
		if(_stricmp(pInfo->CharInfo.szName, pszCharName) == 0) {
			MCommand* pCmd = pClient->CreateCommand(MC_TEST_PEERTEST_PING, pInfo->uidChar);
			pClient->Post(pCmd);
		}
	}
}

void ChatCmd_Macro(const char* line, const int argc, char **const argv)
{
// config 에 등록 저장 - 키입력 누를때처럼
// 
	if(argc != 3)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int mode = -1;

		 if( _stricmp(argv[1], "1")==0 ) mode = 0;
	else if( _stricmp(argv[1], "2")==0 ) mode = 1;
	else if( _stricmp(argv[1], "3")==0 ) mode = 2;
	else if( _stricmp(argv[1], "4")==0 ) mode = 3;
	else if( _stricmp(argv[1], "5")==0 ) mode = 4;
	else if( _stricmp(argv[1], "6")==0 ) mode = 5;
	else if( _stricmp(argv[1], "7")==0 ) mode = 6;
	else if( _stricmp(argv[1], "8")==0 ) mode = 7;
	else if( _stricmp(argv[1], "9")==0 ) mode = 8;
	else 
		return;

	ZCONFIG_MACRO* pMacro = NULL;

	if(ZGetConfiguration())
		pMacro = ZGetConfiguration()->GetMacro();
	
	if( pMacro && argv[2] ) 
	{
		strcpy( pMacro->szMacro[mode],argv[2] );
		char szPath[_MAX_PATH];
		TCHAR szMyDocPath[MAX_PATH];
		if (GetMyDocumentsPath(szMyDocPath)) {
			strcpy(szPath, szMyDocPath);
			strcat(szPath, GUNZ_FOLDER);
			CreatePath(szPath);
			strcat(szPath, "/config");
			CreatePath(szPath);
			strcat(szPath, "/");
			strcat(szPath, "config.xml");
			ZGetConfiguration()->Save(szPath, Z_LOCALE_XML_HEADER);
		}
	}
}

void ChatCmd_AdminTeleport(const char* line, const int argc, char **const argv)
{
	if (argc < 2)
	{
		ZChatOutput("Usage: /admin_teleport <player>", ZChat::CMT_SYSTEM);
		return;
	}

	if(!ZGetMyInfo()->IsAdminGrade())
		return;
	
	for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
		char* szPlayerName = argv[1];

		if(strcmp(pCharacter->GetCharName(), szPlayerName) == 0)
		{
			ZGetGame()->m_pMyCharacter->SetPosition(pCharacter->GetPosition());
			break;
		}
	}
}

void ChatCmd_AdminMoveTo(const char* line, const int argc, char **const argv)
{
	if (!ZGetMyInfo()->IsAdminGrade())
		return;

	ZGetGame()->m_pMyCharacter->SetPosition(rvector(atof(argv[1]), atof(argv[2]), atof(argv[3])));
}

void ChatCmd_EmotionTaunt(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_TAUNT );
}

void ChatCmd_EmotionBow(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_BOW );
}

void ChatCmd_EmotionWave(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_WAVE );
}

void ChatCmd_EmotionLaugh(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_LAUGH );
}

void ChatCmd_EmotionCry(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_CRY );
}

void ChatCmd_EmotionDance(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_DANCE );
}


void ChatCmd_RequestPlayerInfo(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPostRequestCharInfoDetail(ZGetGameClient()->GetPlayerUID(), argv[1]);
}
///////////////////////////////////////////////////////////////////////////////////////////////

void ChatCmd_AdminAnnounce(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szMsg[256];
	strcpy(szMsg, argv[1]);
	ZPostAdminAnnounce(ZGetGameClient()->GetPlayerUID(), szMsg, ZAAT_CHAT);
}

void ChatCmd_AdminServerHalt(const char* line, const int argc, char **const argv)
{
	ZPostAdminHalt(ZGetGameClient()->GetPlayerUID());
}

void ChatCmd_AdminSwitchCreateLadderGame(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	bool bEnabled = true;
	if (!strcmp(argv[1], "0")) bEnabled = false;

	ZPostAdminRequestSwitchLadderGame(ZGetGameClient()->GetPlayerUID(), bEnabled);
}

void ChatCmd_AdminSlap(const char* line, const int argc, char **const argv)
{

	if (!ZGetGame())
		return;

	if (argc < 2)
	{
		ZChatOutput("Usage: /admin_slap <player>", ZChat::CMT_SYSTEM);
		return;
	}

	if (!ZGetMyInfo()->IsAdminGrade())
		return;

	char* targetName = argv[1];

	if (_stricmp(targetName, "all") == 0)
	{
		for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
		{
			ZCharacter* player = (ZCharacter*)(*itor).second;
			if (player->GetUID() == ZGetGame()->m_pMyCharacter->GetUID())
				continue;

			ZPostAdminRequestSlap(ZGetMyUID(), player->GetUID());
		}
	}
	else
	{
		for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
		{
			ZCharacter* player = (ZCharacter*)(*itor).second;
			if (_stricmp(player->GetCharName(), targetName) == 0)
			{
				ZPostAdminRequestSlap(ZGetMyUID(), player->GetUID());
			}
		}
	}
}

void ChatCmd_Suicide(const char* line,const int argc, char **const argv)
{
	ZGetGameClient()->RequestGameSuicide();
}


void ChatCmd_LadderInvite(const char* line,const int argc, char **const argv)
{
	if (argc < 3) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	// 테스트로 우선 2명이 한팀
	char szNames[2][256];
	strcpy(szNames[0], argv[1]);
	strcpy(szNames[1], argv[2]);

	ZGetGameClient()->RequestProposal(MPROPOSAL_LADDER_INVITE, &argv[1], 2);
}

void ChatCmd_LadderTest(const char* line,const int argc, char **const argv)
{
	// 사용하는 부분이 없어서 디버그용으로 수정함. -by SungE 2007-04-02
}

void ChatCmd_LaunchTest(const char* line,const int argc, char **const argv)
{

}

void ChatCmd_Callvote(const char* line,const int argc, char **const argv)
{
	if ( (argv[1] == NULL) || (argv[2] == NULL) )
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPOSTCMD2(MC_MATCH_CALLVOTE, MCmdParamStr(argv[1]), MCmdParamStr(argv[2]))
}

void ChatCmd_VoteYes(const char* line,const int argc, char **const argv)
{
	ZPOSTCMD0(MC_MATCH_VOTE_YES);
}

void ChatCmd_VoteNo(const char* line,const int argc, char **const argv)
{
	ZPOSTCMD0(MC_MATCH_VOTE_NO);
}

void ChatCmd_Kick(const char* line,const int argc, char **const argv)
{
	ZGetCombatInterface()->GetVoteInterface()->CallVote("kick");
}

void ChatCmd_MouseSensitivity(const char* line,const int argc, char **const argv)
{
	if (argc == 1) 
	{
		ZChatOutputMouseSensitivityCurrent( ZGetConfiguration()->GetMouseSensitivityInInt());
	}
	else if (argc == 2)
	{
		// 감도 설정
		int original = ZGetConfiguration()->GetMouseSensitivityInInt();

		char* szParam = argv[1];
		int asked = atoi(szParam);
        int changed = ZGetConfiguration()->SetMouseSensitivityInInt(asked);

		ZChatOutputMouseSensitivityChanged(original, changed);
	}
	else
		OutputCmdWrongArgument(argv[0]);
}


// 퀘스트 테스트용 명령어 /////////////////////////////////////////////////////
void ChatCmd_QUESTTEST_God(const char* line,const int argc, char **const argv)
{
	/*if (!ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL)
	{
		ZGetGameClient()->OutputMessage_Sub("Must be Ingame!");
		return;
	}

	bool bNowGod = ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD);
	bNowGod = !bNowGod;

	ZGetQuest()->SetCheet(ZQUEST_CHEET_GOD, bNowGod);

	if (bNowGod)
	{
		ZGetGameClient()->OutputMessage_Sub( "God mode Enabled!" );
	}
	else
	{
		ZGetGameClient()->OutputMessage_Sub( "God mode Disabled!" );
	}*/
}


void ChatCmd_QUESTTEST_SpawnNPC(const char* line,const int argc, char **const argv)
{
/*	if (!ZGetMyInfo()->IsAdminGrade())
		return;
	if(ZGetGame() == NULL) return;
	if (argc < 2) return;

	int nNPCID = 0;
	int nCount = 1;

	nNPCID = atoi(argv[1]);
	if(argv[2])
		nCount = atoi(argv[2]);

	ZPostQuestTestNPCSpawn(nNPCID, nCount);*/
}



void ChatCmd_QUESTTEST_LocalSpawnNPC(const char* line,const int argc, char **const argv)
{
/*	if (!ZGetMyInfo()->IsAdminGrade())
		return;
	int nNPCID = 0;
	int nCount = 1;

	nNPCID = atoi(argv[1]);
	if(argv[2]) nCount = atoi(argv[2]);

	
	MUID uidLocal;
	uidLocal.High = 10000;
	uidLocal.Low = (unsigned long)ZGetObjectManager()->size();

	for(int i  = MQUEST_NPC::NPC_NONE; i < MQUEST_NPC::NPCS_END; i++)
	{
		ZPostQuestTestLocalNPCSpawn(ZGetGame()->m_pMyCharacter->GetUID(),uidLocal,i,i);
	}*/
}


void ChatCmd_QUESTTEST_NPCClear(const char* line,const int argc, char **const argv)
{
/*	if (!ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestClearNPC();
	ZGetGameClient()->OutputMessage_Sub("Npc's Cleared.");*/
}


void ChatCmd_QUESTTEST_Reload(const char* line,const int argc, char **const argv)
{
/*	if (!ZGetMyInfo()->IsAdminGrade())
		return;

	ZGetObjectManager()->ClearNPC();	// 먼저 NPC가 클리어되어야 뒤탈이 없다.
	ZGetQuest()->Reload();

	ZGetGameClient()->OutputMessage_Sub("Quest Reloaded.");*/
}


void ChatCmd_QUESTTEST_SectorClear(const char* line,const int argc, char **const argv)
{
/*	if (!ZGetMyInfo()->IsAdminGrade())
		return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestSectorClear();
	ZGetGameClient()->OutputMessage_Sub("Stage Cleared.");*/
}

void ChatCmd_QUESTTEST_Finish(const char* line,const int argc, char **const argv)
{
/*	if (!ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestFinish();
	ZGetGameClient()->OutputMessage_Sub("Quest Cleared.");*/
}

void ChatCmd_QUESTTEST_WeakNPCs(const char* line, const int argc, char **const argv)
{
	//if (!ZGetMyInfo()->IsAdminGrade())
	//	return;
	//if (ZGetGame() == NULL) return;

	//bool bNow = ZGetQuest()->GetCheet(ZQUEST_CHEET_WEAKNPCS);
	//bNow = !bNow;

	//ZGetQuest()->SetCheet(ZQUEST_CHEET_WEAKNPCS, bNow);

	//if (bNow)
	//{
	//	// 지금 있는 NPC들의 HP를 1로 세팅
	//	for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
	//		itor != ZGetObjectManager()->end(); ++itor)
	//	{
	//		ZObject* pObject = (*itor).second;
	//		if (pObject->IsNPC())
	//		{
	//			ZActor* pActor = (ZActor*)pObject;
	//			ZModule_HPAP* pModule = (ZModule_HPAP*)pActor->GetModule(ZMID_HPAP);
	//			if (pModule)
	//			{
	//				pModule->SetHP(1);
	//			}
	//		}
	//	}
	//	ZGetGameClient()->OutputMessage_Sub("WeakNPC mode Enabled!");
	//}
	//else
	//{
	//	ZGetGameClient()->OutputMessage_Sub("WeakNPC mode Disabled!");
	//}

}
///////////////////////////////////////////////////////////////////////////////

void ChatCmd_Framed(const char* line, const int argc, char **const argv)
{
	return;
	char szMsg[256];

	GunzState state = ZApplication::GetGameInterface()->GetState();

	if (state == GUNZ_GAME)
	{
		if (!ZGetGame()->m_bShowWireframe)
		{
			ZGetGame()->m_bShowWireframe = true;
			sprintf(szMsg, "^1Wireframe mode activated!");
			ZChatOutput(szMsg);
		}
		else if (ZGetGame()->m_bShowWireframe)
		{
			ZGetGame()->m_bShowWireframe = false;
			sprintf(szMsg, "^1Wireframe mode deactivated!");
			ZChatOutput(szMsg);
		}
	}
	else
	{
		sprintf(szMsg, "^1Wireframe can only be activated in-game!");
		ZChatOutput(szMsg);
		return;
	}
}
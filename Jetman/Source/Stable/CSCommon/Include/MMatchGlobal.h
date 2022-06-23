#pragma once 

#include "MBaseLocale.h"

constexpr auto MAX_CHAR_COUNT = 4;	// The number of characters that can be made


constexpr auto CYCLE_STAGE_UPDATECHECKSUM = 500;	// 방리스트 정보 변경사항 보여주는 딜레이 - 0.5초

// 게임 Rule 관련
constexpr auto NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS = 3;	// 팀전에서 팀인원이 3명 이상일때만 경험치 보너스를 적용한다
constexpr auto RESPAWN_DELAYTIME_AFTER_DYING = 7000;///< 죽고나서 리스폰 되는 기본 딜레이시간
constexpr auto RESPAWN_DELAYTIME_AFTER_DYING_MIN = 2000;	///< 죽고나서 리스폰 되는 최소 딜레이시간
constexpr auto RESPAWN_DELAYTIME_AFTER_DYING_MAX = 20000;///< 죽고나서 리스폰 되는 최대 딜레이시간

constexpr auto MAX_XP_BONUS_RATIO = 2.0f;	///< 경험치 보너스는 최대 2배이다.
constexpr auto MAX_BP_BONUS_RATIO = 2.0f;	///< 바운티 보너스는 최대 2배이다.

constexpr auto TRANS_STAGELIST_NODE_COUNT = 8;// 한번에 클라이언트에게 보내주는 스테이지노드 개수
constexpr auto TRANS_STANDBY_CLANLIST_NODE_COUNT = 4;	// 클랜전에서 한번에 클라이언트에게 보내주는 대기중 클랜 개수


constexpr auto MAX_REPLIER = 16;	// proposal 답변자는 최대 16명

constexpr auto CLAN_SPONSORS_COUNT = 4;	// 클랜생성시 필요한 발기인 수
constexpr auto CLAN_CREATING_NEED_BOUNTY = 0;	// 클랜생성에 필요한 바운티
constexpr auto CLAN_CREATING_NEED_LEVEL = 1;	// 클랜생성에 필요한 레벨



constexpr auto MAX_CHAR_LEVEL = 120;

constexpr auto MATCH_SIMPLE_DESC_LENGTH = 64;


// 액션리그 관련
constexpr auto ACTIONLEAGUE_TEAM_MEMBER_COUNT = 4;	// 액션리그는 4명이 모두 함께 게임해야된다.
constexpr auto MAX_LADDER_TEAM_MEMBER = 4;	// 래더팀은 1~4명까지 만들 수 있다.
constexpr auto MAX_CLANBATTLE_TEAM_MEMBER = 8;	// 클랜전은 최대 8명까지 만들 수 있다.

//constexpr auto LIMIT_ACTIONLEAGUE		// 넷마블 액션리그 전용 디파인
//constexpr auto LEAK_TEST
#define CLAN_BATTLE					// 클랜전 개발용 디파인 - 개발이 끝나면 사라질 예정





enum MMatchTeam
{
	MMT_ALL			= 0,
	MMT_SPECTATOR	= 1,
	MMT_RED			= 2,
	MMT_BLUE		= 3,
	MMT_END
};


// 서버모드
enum MMatchServerMode
{
	MSM_USUAL		= 0,		// 일반
	MSM_CLAN		= 1,		// 클랜전 전용 서버
	MSM_LADDER		= 2,		// 래더 전용 서버
	MSM_EVENT		= 3,		// 이벤트 서버
	MSM_TEST		= 4,		// 테스트 서버
	MSM_MAX,

	MSM_ALL			= 100,		// event에만 사용된다.
};

// 동의 관련
enum MMatchProposalMode
{
	MPROPOSAL_NONE = 0,				// 사용하지 않음
	MPROPOSAL_LADDER_INVITE,		// 래더게임 요청
	MPROPOSAL_CLAN_INVITE,			// 클랜전 요청
	MPROPOSAL_END
};


// 래더 타입
enum MLADDERTYPE {
	MLADDERTYPE_NORMAL_2VS2		= 0,
	MLADDERTYPE_NORMAL_3VS3,
	MLADDERTYPE_NORMAL_4VS4,
//	MLADDERTYPE_NORMAL_8VS8,
#ifdef _DEBUG
	MLADDERTYPE_NORMAL_1VS1,
#endif
	MLADDERTYPE_MAX
};

// 각 래더타입별 필요한 인원수
const int g_nNeedLadderMemberCount[MLADDERTYPE_MAX] = {	2, 3, 4/*, 8*/
#ifdef _DEBUG
, 1 
#endif
};


/// Clan관련.
constexpr auto DEFAULT_CLAN_POINT = 1000;			// 기본 클랜 포인트;
constexpr auto DAY_OF_DELETE_CLAN = 7;		// 클랜 폐쇄요청후 DAY_OF_DELETE_CLAN일만큼 지난후 패쇄작업이 진행됨.
constexpr auto MAX_WAIT_CLAN_DELETE_HOUR = 24;			// DAY_OF_DELETE_CLAN + MAX_WAIT_CLAN_DELETE_HOUR후 디비에서 클랜삭제.
constexpr auto UNDEFINE_DELETE_HOUR = 2000000000;	// 정상적인 클랜의 DeleteTime의 null값처리용.

enum MMatchClanDeleteState
{
	MMCDS_NORMAL = 1,
	MMCDS_WAIT,
	MMCDS_DELETE,

	MMCDS_END,
};


// 옵션 관련
enum MBITFLAG_USEROPTION {
	MBITFLAG_USEROPTION_REJECT_WHISPER	= 1,
	MBITFLAG_USEROPTION_REJECT_INVITE	= 1<<1
};

// 퀘스트 관련 ///////////////////////////////////////////////////////////////////////////////

constexpr auto  MAX_QUEST_MAP_SECTOR_COUNT = 16;	// 퀘스트에서 최대 만들어질 수 있는 맵 개수

#ifdef _DEBUG_QUEST
constexpr auto MAX_QUEST_NPC_INFO_COUNT				100			// 퀘스트에서 최대 나올 NPC 종류 개수
#else
constexpr auto MAX_QUEST_NPC_INFO_COUNT = 14;	// 퀘스트에서 최대 나올 NPC 종류 개수
#endif



constexpr auto ALL_PLAYER_NOT_READY = 1;	// 모든 유저가 레디를 하지 못해서 게임을 시작하지 못함.
constexpr auto QUEST_START_FAILED_BY_SACRIFICE_SLOT = 2;	// 희생 아이템 슬롯 검사시 문제가 있어서 시작을 실패함.
constexpr auto INVALID_TACKET_USER = 3;	// 입장권을 가지고 있는 않은 유저가 있음.
constexpr auto INVALID_MAP = 4;// 현제 게임타입에서 사용할 수 없는 맵임.

constexpr auto MIN_QUESTITEM_ID = 200001;// item id가 200001부터는 퀘스트 아이템이다
constexpr auto MAX_QUESTITEM_ID = 299999;

// Keeper Manager와의 Schedule관련. ////////////////////////////////////////////////////////////

enum KMS_SCHEDULE_TYPE
{
	KMST_NO = 0,
	KMST_REPEAT,
	KMST_COUNT,
	KMST_ONCE,

	KMS_SCHEDULE_TYPE_END,
};

enum KMS_COMMAND_TYPE
{
	KMSC_NO = 0,
	KMSC_ANNOUNCE,
	KMSC_STOP_SERVER,
	KMSC_RESTART_SERVER,
	
	KMS_COMMAND_TYPE_END,
};

enum SERVER_STATE_KIND
{
	SSK_OPENDB = 0,

	SSK_END,
};

enum SERVER_ERR_STATE
{
	SES_NO = 0,
	SES_ERR_DB,
    
	SES_END,
};

enum SERVER_TYPE
{
	ST_NULL = 0,
	ST_DEBUG,
	ST_NORMAL,
	ST_CLAN,
	ST_QUEST,
	ST_EVENT,
};


enum MMatchBlockLevel
{
	MMBL_NO = 0,
	MMBL_ACCOUNT,
	MMBL_LOGONLY,

	MMBL_END,
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Util 함수

/// 상대편 반환
inline MMatchTeam NegativeTeam(MMatchTeam nTeam)
{
	if (nTeam == MMT_RED) return MMT_BLUE;
	else if (nTeam == MMT_BLUE) return MMT_RED;
	return nTeam;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// string buffer length define.

constexpr auto ANNOUNCE_STRING_LEN = 128;
constexpr auto MSG_STRING_LEN = 128;
constexpr auto CHAT_STRING_LEN = 128;
constexpr auto VOTE_DISCUSS_STRING_LEN = 128;
constexpr auto VOTE_ARG_STRING_LEN = 128;
// stage
constexpr auto STAGENAME_LENGTH = 64;	// 방이름 길이
constexpr auto STAGEPASSWD_LENGTH = 8;	// 패스워드 최대길이
constexpr auto STAGE_QUEST_MAX_PLAYER = 4;	// 퀘스트 게임모드의 최대인원
constexpr auto STAGE_MAX_PLAYERCOUNT = 16;  // 기본모드(퀘스트, 클래을 제외) 최대 인원. - by SungE 2007-05-14.
constexpr auto STAGE__MAX_ROUND = 100;	// 스테이지에서 사용할 수 있는 최대 라운드 수. 이수를 넘을 수 없다. - By SungE 2007-11-07
// clan
constexpr auto CLAN_NAME_LENGTH = 16;	// 클랜 이름 최대 길이 - 이게 변경되면 프로토콜 버전도 변경되어야 함.
											// 이 부분은 이전 사용자들때문에 디비에 맞추지 못함. - by SungE. 2007-03-20
constexpr auto MIN_CLANNAME = 4;	// 최소 4자이상 12자이하만 클랜 이름을 만들 수 있다.
constexpr auto MAX_CLANNAME = 12;
// character
constexpr auto MIN_CHARNAME = 4;	// 최소 4자이상 12자 이하만 캐릭터 이름을 만들 수 있다.
constexpr auto MAX_CHARNAME = 12;
constexpr auto MATCHOBJECT_NAME_LENGTH = 32;	// 캐릭터 이름 길이 - 역시 리플레이때문에 DB에 맞출수 없음. 새로 추가하는 쪽으로 수정. - by SungE. 2007-03-20
constexpr auto MAX_CHARNAME_LENGTH = 24;	// DB에 설정된 길이. 커맨드는 이 길이를 사용한다.
// chat room
constexpr auto MAX_CHATROOMNAME_STRING_LEN = 64;
constexpr auto MAX_USERID_STRING_LEN = 20;
//constexpr auto MAX_USERID_STRING_LEN			33	///< 스트링 길이 20바이트 + 종료 문자 1바이트 //Custom: Changed to 33 for dr forumid len
constexpr auto MAX_USER_PASSWORD_STRING_LEN = 33;
constexpr auto USERNAME_STRING_LEN = 50;
// channel
constexpr auto CHANNELNAME_LEN = 64;
constexpr auto CHANNELRULE_LEN = 64;
// map
constexpr auto MAPNAME_LENGTH = 32;
// Auth
constexpr auto NHN_GAMEID = "u_gunz";
constexpr auto NHN_AUTH_LENGTH = 4096;
constexpr auto NHN_OUTBUFF_LENGTH = 1024;
// item
constexpr auto MAX_ACCOUNT_ITEM = 1000;	// 최고 1000개로 제한한다.
constexpr auto MAX_EXPIRED_ACCOUNT_ITEM = 100;
constexpr auto MAX_ITEM_COUNT = 100;		// 한사람이 최대로 갖을 수 있는 아이템 개수(상점에서 아이템 구입시)
constexpr auto MAX_QUEST_REWARD_ITEM_COUNT = 500;			// 한사람이 최대로 갖을 수 있는 아이템 개수(퀘스트시 아이템 획득시)
constexpr auto MAX_SPENDABLE_ITEM_COUNT = 999;			// 소모성 아이템을 최대로 갖을 수 있는 개수
// gamble item 
constexpr auto MAX_GAMBLEITEMNAME_LEN = (65);		// 
constexpr auto MAX_GAMBLEITEMDESC_LEN = (65);		// 간단한 추가 설명.
constexpr auto MAX_BUYGAMBLEITEM_ELAPSEDTIME_MIN = (5);		// 이벤트 기간이 있을경우 상점에서 사는 시간을 고려해서 이 시간을 초과하면 안됨. 
														// - by SungE 2007-06-22
// time define.
constexpr auto MIN_REQUEST_STAGESTART_TIME = (1 * 1000);		// 마지막 스테이지 시작후 1초가 지나야 다시 시작을 할 수 있다.
constexpr auto MIN_REQUEST_SUICIDE_TIME = (1000 * 60 * 3);	// 자신이 죽었다는 요청은 3분에 한번씩 요청할 수 있다.

//constexpr auto UPDATE_KILLTRACKER_ELAPSED_TIME			(1000 * 60 * 5)
//constexpr auto TRACKERINFO_LIFETIME					(1000 * 60 * 20)	
//constexpr auto KILLCOUNT_TRACE_TIME					(1000 * 60 * 10)
//constexpr auto MAX_KILLCOUNT_ON_TRACETIME				(40)

// md5 
constexpr auto MAX_MD5LENGH = (16);
/////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu
// 서바이벌 랭킹 관련
constexpr auto MAX_SURVIVAL_SCENARIO_COUNT = 3;
constexpr auto MAX_SURVIVAL_RANKING_LIST = 10;

typedef struct _RankingInfo
{
	char szCharName[MATCHOBJECT_NAME_LENGTH];
	DWORD dwCID;
	DWORD dwRanking;
	DWORD dwRankingPoint;
} RANKINGINFO;

//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 9. 25 - Added By Hong KiJu
// Duel Tournament 관련
constexpr auto DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH = 8;

enum MDUELTOURNAMENTTYPE
{
	MDUELTOURNAMENTTYPE_FINAL				= 0,		//< 결승전부터
	MDUELTOURNAMENTTYPE_SEMIFINAL,						//< 4강전부터
	MDUELTOURNAMENTTYPE_QUATERFINAL,					//< 8강전부터
	MDUELTOURNAMENTTYPE_MAX	
};

enum MDUELTOURNAMENTROUNDSTATE {
	MDUELTOURNAMENTROUNDSTATE_FINAL = 0,	
	MDUELTOURNAMENTROUNDSTATE_SEMIFINAL,
	MDUELTOURNAMENTROUNDSTATE_QUATERFINAL,
	MDUELTOURNAMENTROUNDSTATE_MAX
};

enum MDUELTOURNAMENTMATCHMAKINGFACTOR			// 매치가 만들어진 근거
{
	MDUELTOURNAMENTMATCHMAKINGFACTOR_TPGAP = 0,			//< TP gap 적당
	MDUELTOURNAMENTMATCHMAKINGFACTOR_OVERWAIT,			//< 대기시간 초과
};

// 프리카운트다운 상태에서 시간을 어떻게 쪼개서 사용할 것인지 (모두 합친 것이 전체 시간)
constexpr auto DUELTOURNAMENT_PRECOUNTDOWN_WINLOSE_SHOWTIME = 4000;
constexpr auto DUELTOURNAMENT_PRECOUNTDOWN_NEXTMATCH_SHOWTIME = 6000;

inline int GetDTPlayerCount(MDUELTOURNAMENTTYPE nType)
{
	switch(nType) {
		case MDUELTOURNAMENTTYPE_QUATERFINAL :		return 8;
		case MDUELTOURNAMENTTYPE_SEMIFINAL :		return 4;
		case MDUELTOURNAMENTTYPE_FINAL :			return 2;			
	}

	return 0;
}

inline int GetDTRoundCount(MDUELTOURNAMENTROUNDSTATE nRoundState)
{
	switch(nRoundState) {
		case MDUELTOURNAMENTROUNDSTATE_QUATERFINAL :		return 4;
		case MDUELTOURNAMENTROUNDSTATE_SEMIFINAL :			return 2;
		case MDUELTOURNAMENTROUNDSTATE_FINAL :				return 1;			
	}

	return 0;
}

typedef struct _DTRankingInfo
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	int m_nTP;
	int m_nWins;
	int m_nLoses;
	int m_nRanking;
	int m_nRankingIncrease;
	int m_nFinalWins;
	int m_nGrade;
} DTRankingInfo;

typedef struct _DTPlayerInfo
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];

	MUID uidPlayer;
	int m_nTP;
} DTPlayerInfo;


//////////////////////////////////////////////////////////////////////////////////////////////
// 2010. 04. 12 - Added By 홍기주

// 아이템 리뉴얼 관련

constexpr auto ACCOUNTITEM_INCREASE_LOG_MOVE = 100;
constexpr auto ACCOUNTITEM_INCREASE_LOG_BUY = 101;
constexpr auto ACCOUNTITEM_INCREASE_LOG_REWARD_BY_GM = 102;

constexpr auto ACCOUNTITEM_DECREASE_LOG_MOVE = 200;
constexpr auto ACCOUNTITEM_DECREASE_LOG_SELL = 201;
constexpr auto ACCOUNTITEM_DECREASE_LOG_EXPIRE = 202;

constexpr auto CHARITEM_INCREASE_LOG_MOVE = 100;
constexpr auto CHARITEM_INCREASE_LOG_BUY = 101;
constexpr auto CHARITEM_INCREASE_LOG_REWARD = 102;

constexpr auto CHARITEM_DECREASE_LOG_MOVE = 200;
constexpr auto CHARITEM_DECREASE_LOG_SELL = 201;
constexpr auto CHARITEM_DECREASE_LOG_EXPIRE = 202;
constexpr auto CHARITEM_DECREASE_LOG_GAMBLE = 203;
constexpr auto CHARITEM_DECREASE_LOG_SPEND = 204;

//////////////////////////////////////////////////////////////////////////////////////////////
// 2010. 06. 07 - Added By 홍기주

constexpr auto	MAX_CHARACTER_SHORT_BUFF_COUNT = 2;

typedef struct _MLongBuffInfoInDB
{	
	int nCBID;
	int nBuffID;
	int nBuffSecondPeriod;
	int nStartPlayTime;	
} MLongBuffInfoInDB;

typedef struct _MLongBuffInfo
{	
	MUID uidBuff;

	int nCBID;
	int nBuffID;
	int nBuffPeriod;
	int nBuffPeriodRemainder;	
} MLongBuffInfo;

typedef struct _MShortBuffInfo
{	
	MUID uidBuff;

	int nBuffID;
	int nBuffPeriod;
	int nBuffPeriodRemainder;
} MShortBuffInfo;


constexpr auto MAX_TRAP_THROWING_LIFE = 10.f;	// 던져진 트랩은 이 시간 내에 발동되지 못하면 불발탄으로 처리
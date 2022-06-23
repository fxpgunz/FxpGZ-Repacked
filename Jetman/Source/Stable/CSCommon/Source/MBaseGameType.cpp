#include "stdafx.h"
#include "MBaseGameType.h"


#define MMATCH_GAMETYPE_DEATHMATCH_SOLO_STR		"Death Match(Solo)"
#define MMATCH_GAMETYPE_DEATHMATCH_CLASSIC_STR	"Death Match(Classic)"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM_STR		"Death Match(Team)"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM_CLASSIC_STR	"Death Match (Team Classic)"
#define MMATCH_GAMETYPE_GLADIATOR_SOLO_STR		"Gladiator(Solo)"
#define MMATCH_GAMETYPE_GLADIATOR_TEAM_STR		"Gladiator(Team)"
#define MMATCH_GAMETYPE_ASSASSINATE_STR			"Assassinate"
#define MMATCH_GAMETYPE_TRAINING_STR			"Training"
#define MMATCH_GAMETYPE_CLASSIC_SOLO_STR		"Classic(Solo)"
#define MMATCH_GAMETYPE_CLASSIC_TEAM_STR		"Class(Team)"
#define MMATCH_GAMETYPE_SURVIVAL_STR			"Survival"
#define MMATCH_GAMETYPE_QUEST_STR				"Quest"
#define MMATCH_GAMETYPE_BERSERKER_STR			"Berserker"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR	"Death Match(Solo Unlimit)"
#define MMATCH_GAMETYPE_DUEL_STR				"Duel"
#define MMATCH_GAMETYPE_DUELTOURNAMENT_STR		"Duel Tournament"
#define MMATCH_GAMETYPE_CTF_STR					"Capture the Flag"
#define MMATCH_GAMETYPE_GUNGAME_STR				"GunGame"
#define MMATCH_GAMETYPE_BLITZKRIEG_STR			"BlitzKrieg"

// An abbreviation for the replay file mode (enum MMATCH_GAMETYPE The order must be the same as the)
const char* MMatchGameTypeAcronym[MMATCH_GAMETYPE_MAX] = {
	"DM",		// DEATHMATCH_SOLO	
	"DMT",		// DEATHMATCH_TEAM	
	"GL",		// GLADIATOR_SOLO	
	"GLT",		// GLADIATOR_TEAM	
	"ASSA",		// ASSASSINATE		
	"TR",		// TRAINING		
	"SURV",		// SURVIVAL		
	"QST",		// QUEST			
	"DMBSK",	// BERSERKER (DeathMatch Berserker)		
	"DMTE",		// DEATHMATCH_TEAM2 (DeathMatch Team Extreme)
	"DUEL",		// DUEL			
	"DUELT",	// DUELTOURNAMENT	
	"CTF",		//CAPTURE THE FLAG
	"GNG",		// DEATHMATCH CLASSIC
	"SPY",		// DEATHMATCH_TEAM_CLASSIC
	"QSTC",		//GUNGAME
	"INF",		//SPYMODE
	"BLTZ",		//CHALQU
	"SKI",		//BLITZ
};

void MMatchGameTypeInfo::Set(const MMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		    const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio)
{
	MMatchGameTypeInfo::nGameTypeID = a_nGameTypeID;
	strcpy_s(MMatchGameTypeInfo::szGameTypeStr, a_szGameTypeStr);
	MMatchGameTypeInfo::fGameExpRatio = a_fGameExpRatio;
	MMatchGameTypeInfo::fTeamMyExpRatio = a_fTeamMyExpRatio;
	MMatchGameTypeInfo::fTeamBonusExpRatio = a_fTeamBonusExpRatio;
}

void MMatchGameTypeInfo::AddMap(int nMapID)
{
	MapSet.insert(set<int>::value_type(nMapID));
}

void MMatchGameTypeInfo::AddAllMap()
{
	for (int i = 0; i < MMATCH_MAP_MAX; i++)
	{
		AddMap(i);
	}
}

MBaseGameTypeCatalogue::MBaseGameTypeCatalogue()
{
#define _InitGameType(index, id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio)		\
m_GameTypeInfo[index].Set(id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio);

//				index,							id,									Game type name,					경험치배분 비율, 팀전 개인 경험치 배분율, 팀전 팀 경험치 배분율
_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_SOLO,	MMATCH_GAMETYPE_DEATHMATCH_SOLO,	MMATCH_GAMETYPE_DEATHMATCH_SOLO_STR,  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM,	MMATCH_GAMETYPE_DEATHMATCH_TEAM,	MMATCH_GAMETYPE_DEATHMATCH_TEAM_STR,  1.0f,			0.8f,					0.3f);
_InitGameType(MMATCH_GAMETYPE_GLADIATOR_SOLO,	MMATCH_GAMETYPE_GLADIATOR_SOLO,		MMATCH_GAMETYPE_GLADIATOR_SOLO_STR,   0.5f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_GLADIATOR_TEAM,	MMATCH_GAMETYPE_GLADIATOR_TEAM,		MMATCH_GAMETYPE_GLADIATOR_TEAM_STR,   0.5f,			0.8f,					0.3f);
_InitGameType(MMATCH_GAMETYPE_ASSASSINATE,		MMATCH_GAMETYPE_ASSASSINATE,		MMATCH_GAMETYPE_ASSASSINATE_STR,	  1.0f,			0.8f,					0.3f);
_InitGameType(MMATCH_GAMETYPE_TRAINING,			MMATCH_GAMETYPE_TRAINING,			MMATCH_GAMETYPE_TRAINING_STR,		  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_SURVIVAL,			MMATCH_GAMETYPE_SURVIVAL,			MMATCH_GAMETYPE_SURVIVAL_STR,		  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_QUEST,			MMATCH_GAMETYPE_QUEST,				MMATCH_GAMETYPE_QUEST_STR,			  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_BERSERKER,		MMATCH_GAMETYPE_BERSERKER,			MMATCH_GAMETYPE_BERSERKER_STR,		  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM2,	MMATCH_GAMETYPE_DEATHMATCH_TEAM2,	MMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR, 1.0f,			0.6f,					0.5f);
_InitGameType(MMATCH_GAMETYPE_DUEL,				MMATCH_GAMETYPE_DUEL,				MMATCH_GAMETYPE_DUEL_STR,			  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_DUELTOURNAMENT,	MMATCH_GAMETYPE_DUELTOURNAMENT,		MMATCH_GAMETYPE_DUELTOURNAMENT_STR,	  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_CTF,				MMATCH_GAMETYPE_CTF,				MMATCH_GAMETYPE_CTF_STR	,		      1.0f,			0.6f,					0.5f);
_InitGameType(MMATCH_GAMETYPE_GUNGAME,			MMATCH_GAMETYPE_GUNGAME,			MMATCH_GAMETYPE_GUNGAME_STR,		  1.0f,			1.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_SPYMODE, MMATCH_GAMETYPE_SPYMODE, "SpyMode", 1.0f, 1.0f, 0.0f);
_InitGameType(MMATCH_GAMETYPE_INFECTION, MMATCH_GAMETYPE_INFECTION, "Infection", 1.0f, 1.0f, 0.0f);
_InitGameType(MMATCH_GAMETYPE_QUEST_CHALLENGE,	MMATCH_GAMETYPE_QUEST_CHALLENGE,	"Challenge Quest",  0.0f,			0.0f,					0.0f);
_InitGameType(MMATCH_GAMETYPE_BLITZKRIEG, MMATCH_GAMETYPE_BLITZKRIEG, "BlitzKrieg", 0.0f, 0.0f, 0.0f);
_InitGameType(MMATCH_GAMETYPE_SKILLMAP, MMATCH_GAMETYPE_SKILLMAP, "SkillMap", 1.0f, 1.0f, 0.0f);
_InitGameType(MMATCH_GAMETYPE_DEMOLITION, MMATCH_GAMETYPE_DEMOLITION, "Demolition", 1.0f, 1.0f, 1.0f);
	// This game type is available for all map playable maps-current
	for (int i = 0; i < MMATCH_GAMETYPE_MAX; i++)
	{
		m_GameTypeInfo[i].AddAllMap();
	}
}

MBaseGameTypeCatalogue::~MBaseGameTypeCatalogue()
{

}




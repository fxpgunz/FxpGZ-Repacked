#include "stdafx.h"
#include "ZApplication.h"
#include "ZEnemy.h"
#include "ZObject.h"
#include "ZObjectManager.h"
#include "Physics.h"
#include "ZReplay.h"
#include "ZTestGame.h"
#include "ZGameClient.h"
#include "MMath.h"
#include "ZMapDesc.h"

#include "ZModule_Skills.h"

void CreateTestGame(char *mapname, int nDummyCharacterCount, bool bShot,bool bAITest, int nParam1)
{
	ZApplication::GetStageInterface()->SetMapName(mapname);
	ZGetGameClient()->GetMatchStageSetting()->SetGameType(MMATCH_GAMETYPE_DEATHMATCH_SOLO);
	ZGetGameClient()->GetMatchStageSetting()->SetMapName(mapname);
	ZGetGameInterface()->SetState(GUNZ_GAME);

	if(!ZGetGame())
		return;

	ZGetGame()->SetReadyState(ZGAME_READYSTATE_RUN);

	ZGetGame()->GetMatch()->SetRoundState(MMATCH_ROUNDSTATE_PLAY);

	MTD_CharInfo info;
//	ZGetMyInfo()->GetUGradeID() == MMUG_DEVELOPER;
	strcpy(info.szName,"Jeff");//<--Character Nam
	info.nLevel = 99;
	info.szClanName[0]=0;//<--Clan Name
	info.nSex = MMatchSex::MMS_MALE; //<--Gender
	info.nUGradeID = MMatchUserGradeID::MMUG_DEVELOPER;
	info.nHP = 100;//<--MaxHP
	info.nAP = 100;//<--MaxAP

	for(int i=0;i<MMCIP_END;i++)
		info.nEquipedItemDesc[i]=0;
	
	//********************* Equipped Item IDs ******************************//

	//info.nEquipedItemDesc[MMCIP_MELEE] = 1;		// 구식단검
	info.nEquipedItemDesc[MMCIP_MELEE] = 2;		// 구식장검
	//	info.nEquipedItemDesc[MMCIP_MELEE] = 31;		// 용월랑
	//	info.nEquipedItemDesc[MMCIP_PRIMARY] = 5018;
	info.nEquipedItemDesc[MMCIP_PRIMARY] = 4514;	// 자우르스 B x2
	//info.nEquipedItemDesc[MMCIP_PRIMARY] = 7006;	// LX 44
	//info.nEquipedItemDesc[MMCIP_PRIMARY] = 9003;	// 월콤 L1 mk. II
		info.nEquipedItemDesc[MMCIP_SECONDARY] = 5002;	// 샷건
	//info.nEquipedItemDesc[MMCIP_SECONDARY] = 8003;	// 머신건
	info.nEquipedItemDesc[MMCIP_CUSTOM1] = 400007;
	//info.nEquipedItemDesc[MMCIP_CUSTOM2] = 600005;	// 카노푸스의 눈

	//**********************************************************************//

	//Create Character
	ZGetGame()->CreateMyCharacter(&info);
	//Spawn Character
	ZGetGame()->m_pMyCharacter->Revival();

	//Initialize Position and Direction Vector
	rvector pos=rvector(0,0,0), dir=rvector(0,1,0);

	//Generate Solo Spawn Data
	ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetSoloRandomData();
	if (pSpawnData != NULL)
	{
		pos = pSpawnData->m_Pos;
		dir = pSpawnData->m_Dir;
	}

	//Set Character Position&&Direction
	ZGetGame()->m_pMyCharacter->SetPosition(pos);
	ZGetGame()->m_pMyCharacter->SetDirection(dir);

}


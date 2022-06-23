#include "stdafx.h"
#include "IRuleNewQuest.h"
#include "ZMatch.h"
#include "MActorDef.h"
#include "ZFSMManager.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"
#include "ZActorWithFSM.h"
#include "ZApplication.h"
#include "ZFSMManager.h"
#include "ZFSMParser.h"
#include "ZFSM.h"
#include "ZScreenEffectManager.h"
#include "ZBmNumLabel.h"
#include "ZModule_QuestStatus.h"

IRuleNewQuest::IRuleNewQuest(ZMatch* pMatch) : ZRule(pMatch)
, m_pFsmManager(new ZFSMManager)
, m_pActorDefMgr(new MActorDefManager)
, m_pActorActionMgr(new ZActorActionManager)
, m_nNPC(0)
{
	m_dwElapsedTime = ZGetGame()->GetTime();
}

IRuleNewQuest::~IRuleNewQuest()
{
	SAFE_DELETE(m_pFsmManager);
	SAFE_DELETE(m_pActorDefMgr);
	SAFE_DELETE(m_pActorActionMgr);

	ZGetNpcMeshMgr()->UnLoadAll();
}

bool IRuleNewQuest::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame())
		return false;

	switch (pCommand->GetID())
	{
		case MC_NEWQUEST_NPC_SPAWN:
		{
			MUID uidController;
			MUID uidNpc;
			char szActorDefName[128];
			unsigned char nCustomSpawnTypeIndex, nSpawnIndex;
			pCommand->GetParameter(&uidController, 0, MPT_UID);
			pCommand->GetParameter(&uidNpc, 1, MPT_UID);
			pCommand->GetParameter(szActorDefName, 2, MPT_STR, sizeof(szActorDefName));
			pCommand->GetParameter(&nCustomSpawnTypeIndex, 3, MPT_UCHAR);
			pCommand->GetParameter(&nSpawnIndex, 4, MPT_UCHAR);

			bool mycontrol = false;
			if (ZGetGame()->m_pMyCharacter->IsAdminName())
				mycontrol = true;
			SpawnActor(szActorDefName, uidNpc, nCustomSpawnTypeIndex, nSpawnIndex, ZGetMyUID() == uidController);
			return true;
		}break;

		case MC_NEWQUEST_NPC_DEAD:
		{
			MUID uidKiller, uidNPC;

			pCommand->GetParameter(&uidKiller, 0, MPT_UID);
			pCommand->GetParameter(&uidNPC, 1, MPT_UID);

			ZActorWithFSM* pActor = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(uidNPC);
			if (pActor)
			{
				//does nothing in irulenewquest, but it's used for blitzkrieg;
				CheckSpecialEvent(pActor, pActor->GetTeamID());
				if (_stricmp(pActor->GetActorDef()->GetActorType(), "barricade") != 0)
					ZGetObjectManager()->Delete(pActor);

				m_nNPC--;
				ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(uidKiller);

				if (pCharacter == nullptr)
					break;
				if ((pCharacter == ZGetGame()->m_pMyCharacter) && !pCharacter->IsNPC())
				{
					ZGetScreenEffectManager()->AddKO();
				}
				pCharacter->SetLastTarget(MUID(0, 0));

				ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pCharacter->GetModule(ZMID_QUESTSTATUS);
				if (pMod)
				{
					pMod->AddKills();
				}
			}
			return true;
		}break;

		case MC_NEWQUEST_PEER_NPC_BASICINFO:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if (pParam->GetType() != MPT_BLOB)
			{
				_ASSERT(0); break;
			}

			ZACTOR_WITHFSM_BASICINFO* pbi = (ZACTOR_WITHFSM_BASICINFO*)pParam->GetPointer();
			ZActorWithFSM* pActor = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(pbi->uidNPC);
			if (pActor)
			{
				pActor->OnBasicInfo(pbi);
			}

			return true;
		}break;
		case MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE:
		{
			MUID uidNpc;
			int nActionIndex;
			pCommand->GetParameter(&uidNpc, 0, MPT_UID);
			pCommand->GetParameter(&nActionIndex, 1, MPT_INT, sizeof(nActionIndex));

			ZActorBase* pActor = ZGetObjectManager()->GetNPCObject(uidNpc);
			ZActorWithFSM* pActorWithFSM = MDynamicCast(ZActorWithFSM, pActor);
			if (pActorWithFSM)
			{
				pActorWithFSM->OnPeerActionExecute(nActionIndex);
			}

			return true;
		}break;
		case MC_NEWQUEST_REQUEST_NPC_SPAWN:
		{
			MUID uidController;
			MUID uidNpc;
			char szActorDefName[128];
			int num, route;
			MShortVector dir, pos;
			pCommand->GetParameter(&uidController, 0, MPT_UID);
			pCommand->GetParameter(&uidNpc, 1, MPT_UID);
			pCommand->GetParameter(&num, 2, MPT_INT);
			pCommand->GetParameter(szActorDefName, 3, MPT_STR, sizeof(szActorDefName));
			pCommand->GetParameter(&pos, 4, MPT_SVECTOR);
			pCommand->GetParameter(&dir, 5, MPT_SVECTOR);
			pCommand->GetParameter(&route, 6, MPT_INT);


			bool mycontrol = false;
			if (ZGetGame()->m_pMyCharacter->IsAdminName())
				mycontrol = true;
			SpawnActorSummon(szActorDefName, uidNpc, num,ZGetMyUID() == uidController, pos, dir,route);
			return true;
		}break;
		//Custom: new controller (required to make npcs target a player afterthey die or leave the stage
		case MC_NEWQUEST_NEW_CONTROLLER:
		{
			MUID newController, npcUID;
			pCommand->GetParameter(&newController, 0, MPT_UID);
			pCommand->GetParameter(&npcUID, 1, MPT_UID);
			SetNewNpcController(newController, npcUID);
			return true;
		}break;
		case MC_MATCH_QUEST_PLAYER_DEAD:
		{
			MUID uidVictim;
			pCommand->GetParameter(&uidVictim, 0, MPT_UID);

			ZCharacter* pVictim = (ZCharacter*)ZGetCharacterManager()->Find(uidVictim);

			if (pVictim)
			{
				if (pVictim != ZGetGame()->m_pMyCharacter)
				{
					pVictim->Die();		// 여기서 실제로 죽는다
				}

				pVictim->GetStatus().CheckCrc();
				pVictim->GetStatus().Ref().AddDeaths();
				if (pVictim->GetStatus().Ref().nLife > 0)
					pVictim->GetStatus().Ref().nLife--;
				pVictim->GetStatus().MakeCrc();
			}
			return true;
		}break;
		case MC_NEWQUEST_LATE_NPC_SPAWN:
		{
			MUID uidLateJoiner = MUID();
			MUID uidNpc = MUID();
			char szActorDefName[128];
			unsigned char nCustomSpawnTypeIndex = 0, nSpawnIndex = 0;
			pCommand->GetParameter(&uidLateJoiner, 0, MPT_UID);
			pCommand->GetParameter(&uidNpc, 1, MPT_UID);
			pCommand->GetParameter(szActorDefName, 2, MPT_STR, sizeof(szActorDefName));

			if (uidLateJoiner == ZGetMyUID())
				SpawnLateJoinNpc(szActorDefName, uidNpc, nCustomSpawnTypeIndex, nSpawnIndex);//they dont control the npc b/c they latejoined
			return true;
		}break;
		default:
			return false;
	}
	return false;
}

bool IRuleNewQuest::Init()
{
	//todok quest 게임을 할때마다 새로 로딩하는 것을 개선하자.

	if (!m_pActorActionMgr->ReadXml(ZApplication::GetFileSystem(), "system/zactoraction.xml"))
	{
		mlog("Error while Load actoraction.\n");
		return false;
	}
	vector<ZFSM*> vecFSM = {};
	ZFSMParser fsmParser;
	if (!fsmParser.ReadXml(ZApplication::GetFileSystem(), "system/aifsm.xml", vecFSM, m_pActorActionMgr))
	{
		mlog("Error while Load aifsm.\n");
		return false;
	}
	m_pFsmManager->AddFsm(&vecFSM[0], (unsigned int)vecFSM.size());

	if (ZGetNpcMeshMgr()->LoadXmlList("model/npc2.xml") == -1)
	{
		mlog("Error while Load NPC Models.\n");
		return false;
	}

	if (!m_pActorDefMgr->ReadXml(ZApplication::GetFileSystem(), "system/npc2.xml"))
	{
		mlog("Error while Read npc2.\n");
		return false;
	}

	return true;
}

void IRuleNewQuest::OnUpdate(float fElapsed)
{
	if (ZGetGame() == NULL) return;

	if (!ZGetCombatInterface()->IsShowResult())
	{
		m_dwElapsedTime += fElapsed * 1000;
	}
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		m_currRoundElapsedTime += fElapsed * 1000;
	}
}

void IRuleNewQuest::SetNewNpcController(MUID& uidController, MUID& uidNpc)
{
	ZActorWithFSM* pActor = dynamic_cast<ZActorWithFSM*>(ZGetObjectManager()->GetObject(uidNpc));
	pActor->SetMyControl(uidController == ZGetMyUID());
}


ZActorWithFSM* IRuleNewQuest::SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	rvector pos(0, 0, 0);
	rvector dir(1, 0, 0);


	MActorDef* pActorDef = m_pActorDefMgr->GetDef(szActorDefName);
	if (!pActorDef)
	{
		mlog("ERROR : cannot found actordef:\'%s\'\n", szActorDefName);
		_ASSERT(0);
		return nullptr;
	}

	// 만약 리소스 로딩을 안했으면 로드 - 이럴일은 테스트빼곤 없어야한다.
	RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(pActorDef->GetModelName());
	if (pNPCMesh)
	{
		if (!pNPCMesh->m_isMeshLoaded)
		{
			ZGetNpcMeshMgr()->Load(pActorDef->GetModelName());
			ZGetNpcMeshMgr()->ReloadAllAnimation();
		}
	}

	ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();
	if (pMSM)
	{
		ZMapSpawnData* pSpawnData = nullptr;
		if (!pActorDef->IsBoss())
			pSpawnData = pMSM->GetCustomSpawnData(nCustomSpawnTypeIndex, nSpawnIndex);
		else
			pSpawnData = pMSM->GetSpawnData(ZMapSpawnType::ZMST_NPC_BOSS, nSpawnIndex);

		if (pSpawnData)
		{
			pos = pSpawnData->m_Pos;
			dir = pSpawnData->m_Dir;
		}
	}

	ZActorWithFSM* pNewActor = new ZActorWithFSM(ZGetGame(), m_pActorActionMgr);
	pNewActor->InitWithActorDef(pActorDef, m_pFsmManager);
	pNewActor->SetUID(uid);
	pNewActor->SetPosition(pos);
	pNewActor->SetDirection(dir);
	pNewActor->SetMyControl(bMyControl);
	pNewActor->SetBoss(pActorDef->IsBoss());
	ZGetObjectManager()->Add(pNewActor);
	ZGetEffectManager()->AddReBirthEffect(pNewActor->GetPosition());
	m_nNPC++;

	return pNewActor;
}


ZActorWithFSM* IRuleNewQuest::SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl, MShortVector vec, MShortVector vec2,int route)
{
	rvector pos = rvector(0, 0, 0);
	rvector dir = rvector(1, 0, 0);

	MActorDef* pActorDef = m_pActorDefMgr->GetDef(szActorName);
	if (!pActorDef)
	{
		mlog("ERROR : cannot found actordef:\'%s\'\n", szActorName);

		_ASSERT(0);
		return nullptr;
	}

	// 만약 리소스 로딩을 안했으면 로드 - 이럴일은 테스트빼곤 없어야한다.
	RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get((char*)pActorDef->GetModelName());
	if (pNPCMesh)
	{
		if (!pNPCMesh->m_isMeshLoaded)
		{
			ZGetNpcMeshMgr()->Load((char*)pActorDef->GetModelName());
			ZGetNpcMeshMgr()->ReloadAllAnimation();
		}
	}

	ZActorWithFSM* pNewActor = new ZActorWithFSM(ZGetGame(), m_pActorActionMgr);
	pNewActor->InitWithActorDef(pActorDef, m_pFsmManager);
	pNewActor->SetUID(uid);
	pos = rvector(vec.x, vec.y, vec.z);
	dir = rvector(vec2.x, vec2.y, vec2.z);
	pNewActor->SetPosition(pos);
	pNewActor->SetDirection(dir);
	pNewActor->SetMyControl(bMyControl);	
	pNewActor->SetRoute(route);



	ZGetObjectManager()->Add(pNewActor);
	ZGetEffectManager()->AddReBirthEffect(pNewActor->GetPosition());

	m_nNPC++;

	return pNewActor;
}

ZActorWithFSM* IRuleNewQuest::SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int ncustomSpawnTypeIndex, int nspawnIndex, bool bMyControl)
{
	MActorDef* pActorDef = m_pActorDefMgr->GetDef(szActorDefName);
	if (!pActorDef)
	{
		mlog("ERROR : cannot found actordef:\'%s\'\n", szActorDefName);
		_ASSERT(0);
		return nullptr;
	}

	// 만약 리소스 로딩을 안했으면 로드 - 이럴일은 테스트빼곤 없어야한다.
	RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(pActorDef->GetModelName());
	if (pNPCMesh)
	{
		if (!pNPCMesh->m_isMeshLoaded)
		{
			ZGetNpcMeshMgr()->Load(pActorDef->GetModelName());
			ZGetNpcMeshMgr()->ReloadAllAnimation();
		}
	}

	ZActorWithFSM* pNewActor = new ZActorWithFSM(ZGetGame(), m_pActorActionMgr);
	pNewActor->InitWithActorDef(pActorDef, m_pFsmManager);
	pNewActor->SetUID(uid);
	pNewActor->SetMyControl(false);
	pNewActor->SetBoss(pActorDef->IsBoss());
	ZGetObjectManager()->Add(pNewActor);
	//ZGetEffectManager()->AddReBirthEffect(pNewActor->GetPosition());
	
	m_nNPC++;

	return pNewActor;
}
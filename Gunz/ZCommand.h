

static bool bMassive = false;
static bool bLawn = false;
static bool bSpawn = false;
static bool bPower = false;
static bool bBot = false;


void masive()
{

	bMassive = !bMassive;
	(bMassive == false);

	while (bMassive == true)
	{
		MCommand *pNewCmd = ZGetGameClient()->CreateCommand(MC_PEER_SKILL, MUID(0, 0));
		pNewCmd->AddParameter(new MCommandParameterFloat((float)timeGetTime()));
		pNewCmd->AddParameter(new MCommandParameterInt(2));
		pNewCmd->AddParameter(new MCommandParameterInt(7));
		ZGetGameClient()->Post(pNewCmd);
		Sleep(1);
	}
}
void lawn()
{

	bLawn = !bLawn;
	(bLawn == false);

	while (bLawn == true)
	{
		ZMyCharacter* m_pMyCharacter = ZGetGame()->m_pMyCharacter;
		MCommand *pNewCmd = ZGetGameClient()->CreateCommand(MC_PEER_SHOT_MELEE, MUID(0, 0));
		pNewCmd->AddParameter(new MCommandParameterFloat((float)timeGetTime()));
		pNewCmd->AddParameter(new MCommandParameterPos(m_pMyCharacter->GetPosition().x, m_pMyCharacter->GetPosition().y, m_pMyCharacter->GetPosition().z));
		pNewCmd->AddParameter(new MCommandParameterInt(1));
		ZGetGameClient()->Post(pNewCmd);
		Sleep(1);
	}
}
void spawn()
{

	bSpawn = !bSpawn;
	(bSpawn == false);

	while (bSpawn == true)
	{
		ZMyCharacter* m_pMyCharacter = ZGetGame()->m_pMyCharacter;
		MCommand *pNewCmd = ZGetGameClient()->CreateCommand(MC_PEER_SPAWN, MUID(0, 0));
		pNewCmd->AddParameter(new MCommandParameterPos(m_pMyCharacter->GetPosition().x, m_pMyCharacter->GetPosition().y, m_pMyCharacter->GetPosition().z));
		pNewCmd->AddParameter(new MCommandParameterDir(m_pMyCharacter->GetDirection().x, m_pMyCharacter->GetDirection().y, m_pMyCharacter->GetDirection().z));
		ZGetGameClient()->Post(pNewCmd);

		ZGetGame()->ReleaseObserver();


		ZPACKEDSHOTINFO ShotInfo;
		ShotInfo.fTime = (float)timeGetTime();
		ShotInfo.posx = 0;
		ShotInfo.posy = 0;
		ShotInfo.posz = 0;
		ShotInfo.tox = 0;
		ShotInfo.toy = 0;
		ShotInfo.toz = 0;
		ShotInfo.sel_type = 1;

		pNewCmd = ZGetGameClient()->CreateCommand(MC_PEER_SHOT, MUID(0, 0));
		pNewCmd->AddParameter(new MCommandParameterBlob(&ShotInfo, sizeof(ZPACKEDSHOTINFO)));
		ZGetGameClient()->Post(pNewCmd);
		Sleep(1);
	}
}
void powerlevel()
{

	bPower = !bPower;
	(bPower == false) ? ZChatOutput(MCOLOR(255, 0, 0), "Shotting disabled.") : ZChatOutput(MCOLOR(0, 255, 0), "Shotting enabled.");

	while (bPower == true)
	{
		for (ZCharacterManager::iterator it = ZGetCharacterManager()->begin(); it != ZGetCharacterManager()->end(); it++)	{
			ZCharacter *pChar = (*it).second;

			if (pChar->GetUID() == ZGetMyUID())	{
				continue;
			}

			for (int i = 8; i < 12; i++)	{
				ZPACKEDSHOTINFO ShotInfo;
				ShotInfo.fTime = (float)timeGetTime();
				ShotInfo.posx = (short)pChar->GetPosition().x;
				ShotInfo.posy = (short)pChar->GetPosition().y;
				ShotInfo.posz = (short)pChar->GetPosition().z;
				ShotInfo.tox = (short)pChar->GetPosition().x;
				ShotInfo.toy = (short)pChar->GetPosition().y;
				ShotInfo.toz = (short)pChar->GetPosition().z;
				ShotInfo.sel_type = (BYTE)i;

				MCommand *pNewCmd = ZGetGameClient()->CreateCommand(MC_PEER_SHOT, MUID(0, 0));
				pNewCmd->AddParameter(new MCommandParameterBlob(&ShotInfo, sizeof(ZPACKEDSHOTINFO)));
				ZGetGameClient()->Post(pNewCmd);
			}
		}
		Sleep(100);
	}
}
void bot()
{

	bBot = !bBot;
	(bBot == false);

	while (bBot == true)
	{
		if (ZGetGame()->m_pMyCharacter && !ZGetGame()->m_pMyCharacter->IsDie())
			ZGetCamera()->m_bAutoAiming = !ZGetCamera()->m_bAutoAiming;
	}
}
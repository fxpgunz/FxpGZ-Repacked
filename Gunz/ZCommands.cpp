#include "stdafx.h"

#include "ZCommands.h"
#include "ZConfiguration.h"
#include "ZGame.h"
#include "MDebug.h"
#include "ZCommand.h"

void OutputCmdWrongArgument(const char* cmd);

namespace ZCommands
{
	namespace CmdZ
	{
		namespace ZChatExecute
		{
			void ChatCmd_Respawn(const char* line, const int argc, char** const argv)
			{
				if (!ZGetGame())
				{
					ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
					return;
				}
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&spawn, 0, 0, 0);
			}
			void ChatCmd_Shot(const char* line, const int argc, char** const argv)
			{
				if (!ZGetGame())
				{
					ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
					return;
				}
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&powerlevel, 0, 0, 0);
			}
			void ChatCmd_Massive(const char* line, const int argc, char** const argv)
			{
				if (!ZGetGame())
				{
					ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
					return;
				}
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&masive, 0, 0, 0);
			}
			void ChatCmd_LawnMower(const char* line, const int argc, char** const argv)
			{
				if (!ZGetGame())
				{
					ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
					return;
				}
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&lawn, 0, 0, 0);
			}
			void ChatCmd_BOT(const char* line, const int argc, char** const argv)
			{
				if (!ZGetGame())
				{
					ZChatOutput("You're NOT in game.", ZChat::CMT_SYSTEM);
					return;
				}
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&bot, 0, 0, 0);
			}
			void InitCmdsRev(ZChatCmdManager* cmdmgr)
			{
				cmdmgr->AddCommand(0, "respawn", &ChatCmd_Respawn, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/respawn", "");
			}
			void InitCmdsMas(ZChatCmdManager* cmdmgr)
			{
				cmdmgr->AddCommand(0, "massive", &ChatCmd_Massive, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/massive", "");
			}
			void InitCmdsSh(ZChatCmdManager* cmdmgr)
			{
				cmdmgr->AddCommand(0, "kill_all", &ChatCmd_Shot, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/kill_all", "");
			}
			void InitCmdsLm(ZChatCmdManager* cmdmgr)
			{
				cmdmgr->AddCommand(0, "lawn", &ChatCmd_LawnMower, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/lawn", "");
			}
			void InitCmdsBot(ZChatCmdManager* cmdmgr)
			{
				cmdmgr->AddCommand(0, "aim", &ChatCmd_BOT, CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/aim", "");
			}
		}

	}
}
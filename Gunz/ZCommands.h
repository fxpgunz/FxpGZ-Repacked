#pragma once

#include "MXml.h"
#include "ZIDLResource.h"
#include "ZChatCmd.h"

namespace ZCommands
{
	namespace CmdZ
	{
		namespace ZChatExecute
		{
			void InitCmdsRev(ZChatCmdManager* cmdmgr);
			void InitCmdsMas(ZChatCmdManager* cmdmgr);
			void InitCmdsSh(ZChatCmdManager* cmdmgr);
			void InitCmdsLm(ZChatCmdManager* cmdmgr);
			void InitCmdsBot(ZChatCmdManager* cmdmgr);
		}
	}
}
#include "stdafx.h"
#include "MMatchNotify.h"
#include <crtdbg.h>
#include <map>


#define MESSAGES_FILE_NAME	"system/notify.xml"

#define ZTOK_MESSAGE		"NOTIFY"
#define ZTOK_ID				"id"

typedef map<int, string>	MNotifyMap;

MNotifyMap g_NotifyMap;

bool InitializeNotify(MZFileSystem *pfs)
{
	rapidxml::xml_document<> doc;
	MZFile mzf;
	if(!mzf.Open(MESSAGES_FILE_NAME,pfs))
		return false;

	char *buffer =new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing notify.xml\n");
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), ZTOK_MESSAGE) == 0) {
			int nID = 0;
			if (itor->first_attribute(ZTOK_ID) != nullptr)
			{
				nID = atoi(itor->first_attribute(ZTOK_ID)->value());
				_ASSERT(g_NotifyMap.find(nID) == g_NotifyMap.end());

				g_NotifyMap.insert(MNotifyMap::value_type(nID, string(MGetStringResManager()->GetStringFromXml(itor->value()))));

			}
		}
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

bool NotifyMessage(int nMsgID, string *out)
{
	MNotifyMap::iterator i=g_NotifyMap.find(nMsgID);
	if(i==g_NotifyMap.end())
		return false;

	*out = i->second;

	return true;
}
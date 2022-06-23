#include "StdAfx.h"
#include ".\zlocatorlist.h"
#include "MZFileSystem.h"

ZLocatorList::ZLocatorList(void)
{
}

ZLocatorList::~ZLocatorList(void)
{
}

bool ZLocatorList::ParseLocatorList( rapidxml::xml_node<>* element )
{
	for (auto itor = element; itor; itor = itor->next_sibling())
	{
		if (_stricmp(itor->name(), "LOCATOR") == 0)
		{
			ParseLocator(itor);
		}
	}
	return true;
}

bool ZLocatorList::ParseLocator( rapidxml::xml_node<>* element )
{
	m_LocatorIPList.push_back( element->first_attribute("IP")->value() );

	return true;
}

void ZLocatorList::AddIP( const char* szIP)
{
	m_LocatorIPList.push_back( szIP);
}

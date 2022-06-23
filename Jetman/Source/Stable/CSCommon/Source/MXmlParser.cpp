#include "stdafx.h"
#include "MXmlParser.h"
#include "MZFileSystem.h"

bool MXmlParser::ReadXml(const char* szFileName, MZFileSystem* pFileSystem)
{

	rapidxml::xml_document<> doc;
	MZFile mzf;

	if (pFileSystem) {
		if (!mzf.Open(szFileName, pFileSystem)) {
			if (!mzf.Open(szFileName))
				return false;
		}
	}
	else {

		if (!mzf.Open(szFileName))
			return false;
	}

	char* buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("Error reading %s\n", szFileName);
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		ParseRoot(itor->name(), itor);
	}
	
	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

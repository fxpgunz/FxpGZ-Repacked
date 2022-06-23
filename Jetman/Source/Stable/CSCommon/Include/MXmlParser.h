#ifndef _MXMLPARSER_H
#define _MXMLPARSER_H

#include "rapidxml.hpp"
class MZFileSystem;

class MXmlParser
{
protected:
	virtual void ParseRoot(const char* szTagName, rapidxml::xml_node<>* pElement) = 0;
public:
	MXmlParser() {}
	virtual ~MXmlParser() {}
	bool ReadXml(const char* szFileName,MZFileSystem* pFileSystem = nullptr);		///< xml로부터 정보를 읽는다.
};



#endif
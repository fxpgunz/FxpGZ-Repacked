#ifndef _MSTRINGRES_H
#define _MSTRINGRES_H

#include <map>
#include <string>
using std::map;
using std::string;
using std::pair;

class MZFileSystem;

#define CHAR_MSG_VALUE			'$'



template<typename T>
class MStringRes
{
private:
	map<T, string>		m_StringMap;
	string				m_strTOK_TAG;
	string				m_strTOK_ATTR;
public:
	MStringRes( const char* pszTOK_TAG = "STR", const char* pszTOK_ATTR = "id" )
		: m_strTOK_TAG( pszTOK_TAG ), m_strTOK_ATTR( pszTOK_ATTR )
	{

	}

	virtual ~MStringRes()
	{

	}
	void Release()
	{
		m_StringMap.clear();
	}
	void parse(string &content, int &CID) {
		CID = atoi(content.c_str());
	}

	void parse(string &content, string &CID) {
		CID = content;
	}
	bool Initialize(const char* pszFileName, const int nLangID, MZFileSystem *pfs = 0)
	{
		if (0 == pszFileName) return false;

		MZFile mzf;
		if (!mzf.Open(pszFileName, pfs))
			return false;

		char *buffer = new char[mzf.GetLength() + 1];
		if (0 == buffer) return false;
		memset(buffer, 0, mzf.GetLength() + 1);
		if (!mzf.Read(buffer, mzf.GetLength())) return false;

		mlog("Load XML from memory : %s(0x%04X) ", pszFileName, nLangID);
		rapidxml::xml_document<> doc;
		if (!doc.parse<0>(buffer)) {
			mlog("Error parsing %s", pszFileName);
			delete[] buffer;
			return false;
		}
		mlog("- SUCCESS\n");
		rapidxml::xml_node<>* rootNode = doc.first_node();
		T CID;
		for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
		{
			if (itor->name()[0] == '#')
				continue;

			if (_stricmp(itor->name(), m_strTOK_TAG.c_str()) == 0 ||
				_stricmp(itor->name(), "MSG") == 0 ||
				_stricmp(itor->name(), "MESSAGE") == 0)
			{
				///TODO: figure out how to fill CID !!!
				string contents;
				if (strlen(itor->first_attribute()->value()) > 0)
					contents = itor->first_attribute()->value();
				else
					contents = "";

				parse(contents, CID);
				m_StringMap.insert(std::map<T, string>::value_type(CID, itor->value()));
			}

		}
		delete[] buffer;
		doc.clear();
		mzf.Close();
		return true;
	}

	bool Translate(char* poutStr, const T& code, const int argnum, const char* arg1, va_list args )
	{
		map<T, string>::iterator itor = m_StringMap.find(code);
		if(itor==m_StringMap.end()) 
		{
			_ASSERT(0);
			return false;
		}

		// 인자가 없으면 단지 복사만 하고 끝낸다.
		if ((argnum <= 0) || (argnum > 9))
		{
			strcpy(poutStr, (*itor).second.c_str());
			return true;
		}

		const char* argv[9] = {NULL, };

		//va_list args;
		//va_start(args, arg1);
		argv[0] = arg1;

		for (int i = 1; i < argnum; i++)
		{
			argv[i] = va_arg(args, const char*);
		}
		//va_end(args);

		bool bPercent=false;

		int taridx = 0;
		poutStr[taridx] = 0;
		string formatstring = (*itor).second;
		

		// 인자 파싱
		for(size_t j=0;j<formatstring.size();j++)
		{
			char cur=formatstring[j];
			if(bPercent)
			{
				if(cur==CHAR_MSG_VALUE)
				{
					poutStr[taridx++] = CHAR_MSG_VALUE;
					poutStr[taridx] = 0;
				}
				else
				if ( ('1' <= cur) && (cur <= '9') )
				{
					int nParam = cur - '0' - 1;
					if ( (nParam < argnum) && (argv[nParam] != NULL) )
					{
						strcat(poutStr, argv[nParam]);
						taridx += (int)strlen(argv[nParam]);
					}
					else
					{
						_ASSERT(0);		// xml의 인자 설정이 잘못되었거나, 함수호출시 가변인자가 부족하다.
					}
				}

				bPercent = false;
				continue;
			}

			if(!bPercent)
			{
				if(cur==CHAR_MSG_VALUE)
					bPercent=true;
				else
				{
					poutStr[taridx++] = cur;
					poutStr[taridx] = 0;
				}
			}
		}

		return true;
	}

	const char* GetStr( const T& code )
	{
		map<T, string>::iterator it = m_StringMap.find( code );
		if( m_StringMap.end() == it ){
			// _ASSERT( 0 );			// 해당 문자열은 존재하지 않는다.
			static char *szBlahBlah = "nomsg";
			return szBlahBlah;
		}
		
		return it->second.c_str();
	}
};







#endif
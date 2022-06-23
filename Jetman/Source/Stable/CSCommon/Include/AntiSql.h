inline bool SqlCheckString(char *str)
{
	string szString = str;
	size_t len = szString.length();
	for(size_t i = 0 ; i < len ; i++)
	{
		if(szString[i] == '\'')
		{
			return true;
		}
	}
	return false;
}

inline bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

inline void CleanTSQLString(string& szString)
{
	replace(szString, "'", "''");
}

inline bool StringCheckFormat(char *str)
{
	string szString = str;
	size_t len = szString.length();
	for(size_t i = 0 ; i < len ; i++)
	{
		if(szString[i] == '%')
		{
			return true;
		}
	}
	return false;
}

inline bool StringCheckName(char *str)
{
	string szString = str;
	size_t len = szString.length();
	for(size_t i = 0 ; i < len ; i++)
	{
		if(szString[i] == '^' || szString[i] == '\t' || szString[i] == '\n')
		{
			return true;
		}
	}
	return false;
}


inline bool CheckMd5IntegrityHash(char *md5)
{
	for(int i = 0 ; i < 32 ; i++)
	{
		char a = md5[i];
		if( a<'0' || '9'<a && a<'A' || 'Z'<a && a<'a' || 'z'<a)
		{
			return false;
		}
	}
	return true;
}


inline bool CheckHardwareIntegrityHash(char *hwid)
{
	for(int i = 0 ; i < 32 ; i++)
	{
		char a = hwid[i];
		if( a<'0' || '9'<a && a<'A' || 'Z'<a && a<'a' || 'z'<a)
		{
			return false;
		}
	}
	hwid[32] = 0;

	return true;
}
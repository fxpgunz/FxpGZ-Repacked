#pragma once

#ifndef _DEBUG

#include "lua.hpp"

typedef map<const char*,lua_State*> luaFuncs;

class ZLua {
public:
	ZLua();
	virtual ~ZLua();

	luaFuncs  m_luaStates;
	lua_State* state;


	void Close();
	void Create();


	bool loadScript(const char* scriptName);
	bool LToC(const char* script);

	template<typename _T>
	_T GetGlobalValueNum(const char* argName, int globalIndex = 1) {
		lua_getglobal(state, argName);

		return lua_tonumber(state, globalIndex);
	}

	template<typename _T>
	_T GetGlobalValueNum(const char* argName, lua_State* thisState, int globalIndex = 1) {
		lua_getglobal(thisState, argName);

		return lua_tonumber(thisState, globalIndex);
	}

	void Bind(const char* scriptName, lua_CFunction func);

	lua_State* GetState(const char* scriptName) {
		for (auto& script : m_luaStates)
		{
			if (_stricmp(scriptName, script.first))
				return script.second;
		}
		return nullptr;
	}

	void Run(const char* scriptName);
	

protected:

private:
};
#endif
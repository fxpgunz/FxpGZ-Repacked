#include "stdafx.h"
#include "ZLua.h"

#ifndef _DEBUG

ZLua::ZLua()
{

	
}

ZLua::~ZLua()
{
	for (auto& obj : m_luaStates)
	{
		delete obj.second;
		obj.second = nullptr;
	}
	m_luaStates.clear();
}


//always close after executing script
void ZLua::Close()
{
	//lua_close(state);
}


void ZLua::Create()
{
}

bool ZLua::loadScript(const char* scriptName)
{
	TCHAR currDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currDir);

	strcat(currDir, "\\");
	strcat(currDir, scriptName);

	if (luaL_dofile(state, currDir)) {
		mlog("Error Loading Script - %s\n", lua_tostring(state,-1));
		return false;
	}

	//m_luaStates.emplace(scriptName,state);

	return true;

}

bool ZLua::LToC(const char* scriptName)
{

}

void ZLua::Bind(const char* scriptName, lua_CFunction func)
{
	lua_State* state = luaL_newstate();
	lua_register(state, scriptName, func);
	m_luaStates.emplace(scriptName,state);
}

void ZLua::Run(const char* scriptName)
{
	lua_State* state = m_luaStates.find(scriptName)->second;
	luaL_dofile(state, scriptName);
	lua_close(state);
}

#endif
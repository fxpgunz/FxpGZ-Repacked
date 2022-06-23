#ifndef _ZFILEPATH_H
#define _ZFILEPATH_H


constexpr auto FILENAME_TIPS = "tips.xml";
constexpr auto FILENAME_ZITEM_DESC = "system/zitem.xml";
constexpr auto FILENAME_ZITEM_DESC_LOCALE = "system/zitem_locale.xml";
constexpr auto FILENAME_BUFF_DESC = "System/zBuff.xml";
//#define FILENAME_ZITEMEFFECT_DESC		"system/zeffect.xml"
constexpr auto FILENAME_INTERFACE_MAIN = "main.xml";
constexpr auto FILENAME_ZNPC_DESC = "System/npc.xml";
constexpr auto FILENAME_ZSKILL_DESC = "system/zskill.xml";
constexpr auto FILENAME_SURVIVALMAP = "System/survivalmap.xml";
constexpr auto FILENAME_QUESTMAP = "System/questmap.xml";
constexpr auto FILENAME_QUESTITEM_DESC = "System/zquestitem.xml";
constexpr auto FILENAME_DROPTABLE = "System/droptable.xml";
constexpr auto FILENAME_CHATCMDS = "System/chatcmds.xml";
constexpr auto FILENAME_WORLDITEM = "system/worlditem.xml";
constexpr auto FILENAME_FILELIST = "system/filelist.xml";
constexpr auto FILENAME_ABUSE = "system/abuse.txt";

constexpr auto MPATH_EMBLEMFOLDER = "/Emblem";
constexpr auto MPATH_EMBLEMFILE = "/emblem.xml";

// path는 마지막에 '/' 를 꼭 붙여줘야 한다.

constexpr auto	PATH_INTERFACE = "Interface/";
constexpr auto PATH_CUSTOM_CROSSHAIR = "Custom/CROSSHAIR/";
constexpr auto PATH_GAME_MAPS = "maps/";
constexpr auto PATH_QUEST = "quest/";
constexpr auto PATH_QUEST_MAPS = "quest/maps/";
//Language Folder Paths
constexpr auto PATH_DUT = "system/DUT/";
constexpr auto PATH_SPA = "system/SPA/";
constexpr auto PATH_POR = "system/POR/";
constexpr auto PATH_GER = "system/GER/";
constexpr auto PATH_KOR = "system/KOR/";
constexpr auto PATH_JAP = "system/JAP/";
constexpr auto PATH_USA = "system/USA/";


// 마지막에 /를 붙이지 않음. ZLanguageConf클래스에서 커리해줌.
const char CLI_LANG_DIR[]					= "lang";
const char CLI_MESSAGE_TYPE[]				= "message";
const char CLI_ERRMESSAGE_TYPE[]			= "cserror";
const char CLI_MESSAGE_FILE_NAME[]			= "messages.xml";
const char CLI_ERROR_MESSAGE_FILE_NAME[]	= "cserror.xml";
/////////////////////////////////////////////////////////////







#endif
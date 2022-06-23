#ifndef MSHAREDCOMMANDTABLE_H
#define MSHAREDCOMMANDTABLE_H

class MCommandManager;


#define MCOMMAND_VERSION	58



#define MSCT_MASTER			1
#define MSCT_CLIENT			2
#define MSCT_MATCHSERVER	4
#define MSCT_AGENT			8
#define MSCT_ALL			(MSCT_MASTER+MSCT_CLIENT+MSCT_AGENT)


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ÁÖÀÇ : Ä¿¸Çµå ID¸¦ ¼öÁ¤ÇÏ¸é ±âÁ¸¿¡ ÀúÀåÇÑ ¸®ÇÃ·¹ÀÌ ÆÄÀÏÀÌ Àç»ýµÇÁö ¾ÊÀ» ¼ö ÀÖ½À´Ï´Ù.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// ¸ðµç Ä¿¹Â´ÏÄÉÀÌÅÍ°¡ °øÀ¯ÇÏ´Â Ä¿¸Çµå
void MAddSharedCommandTable(MCommandManager* pCommandManager, int nSharedType);


/// ¼­¹ö Å¬¶óÀÌ¾ðÆ® °øÀ¯ »ó¼ö
#define MATCH_CYCLE_CHECK_SPEEDHACK	20000


// ±âº» Ä¿¸Çµå ¸®½ºÆ®
#define MC_HELP						0
#define MC_VERSION					1

#define MC_DEBUG_TEST				100

#define MC_LOCAL_INFO				201		///< Local Á¤º¸¸¦ ¾ò´Â´Ù.
#define MC_LOCAL_ECHO				202		///< Local Echo Å×½ºÆ®
#define MC_LOCAL_LOGIN				203		///< Login Ã³¸® Local Command



#define MC_NET_ENUM					301		///< ¿¬°áÇÒ ¼ö ÀÖ´Â Ä¿¹Â´ÏÄÉÀÌÅÍ Enum
#define MC_NET_CONNECT				302		///< Ä¿¹Â´ÏÄÉÀÌÅÍ¿Í ¿¬°á
#define MC_NET_DISCONNECT			303		///< Ä¿¹Â´ÏÄÉÀÌÅÍ¿Í ¿¬°á ÇØÁ¦
#define MC_NET_CLEAR				304		///< Ä¿¹Â´ÏÄÉÀÌÅÍ¿Í ¿¬°á ÇØÁ¦ ÈÄ °ü·ÃÀÚ¿ø Ã³¸®
#define MC_NET_ONCONNECT			311		///< Ä¿¹Â´ÏÄÉÀÌÅÍ¿Í ¿¬°áµÇ¾úÀ» ¶§
#define MC_NET_ONDISCONNECT			312		///< Ä¿¹Â´ÏÄÉÀÌÅÍ¿Í ¿¬°áÀÌ ²÷°åÀ» ¶§
#define MC_NET_ONERROR				313		///< Ä¿¹Â´ÏÄÉÀÌÅÍ¿Í ¿¬°á ¿¡·¯
#define MC_NET_CHECKPING			321		///< Ä¿¹Â´ÏÄÉÀÌÅÍ ¹ÝÀÀ¼Óµµ°Ë»ç
#define MC_NET_PING					322		///< Ä¿¹Â´ÏÄÉÀÌÅÍ ¹ÝÀÀ¼Óµµ°Ë»ç ¿äÃ»
#define MC_NET_PONG					323		///< Ä¿¹Â´ÏÄÉÀÌÅÍ ¹ÝÀÀ¼Óµµ°Ë»ç ÀÀ´ä

#define MC_HSHIELD_PING				324		///< ÇÙ½Çµå ¸Þ¼¼Áö ¿äÃ»(¸ÅÄ¡¼­¹ö -> Å¬¶óÀÌ¾ðÆ®)
#define MC_HSHIELD_PONG				325		///< ÇÙ½Çµå ¸Þ¼¼Áö ÀÀ´ä(Å¬¶óÀÌ¾ðÆ® -> ¸ÅÄ¡¼­¹ö)

#define MC_NET_BANPLAYER_FLOODING	326		///< CommandBuilder¿¡¼­ Flooding °ËÃâ½Ã ¸¸µå´Â Command

#define MC_NET_CONNECTTOZONESERVER	331		///< 127.0.0.1:6000 ¿¬°á

#define MC_NET_REQUEST_INFO			341		///< Net Á¤º¸¸¦ ¿äÃ»ÇÑ´Ù.
#define MC_NET_RESPONSE_INFO		342		///< Net Á¤º¸¸¦ ¾ò´Â´Ù.
#define MC_NET_REQUEST_UID			343		///< MUID¸¦ ¿äÃ»
#define MC_NET_RESPONSE_UID			344		///< MUID¸¦ È®º¸ÇØ¼­ µ¹·ÁÁØ´Ù.
#define MC_NET_ECHO					351		///< Net Echo Å×½ºÆ®

#define MC_CLOCK_SYNCHRONIZE		361		///< Å¬·° ½ÌÅ©

// ¸ÅÄ¡¼­¹ö °ü·Ã ¸í·É¾î
#define MC_MATCH_NOTIFY						401		///< ¾Ë¸² ¸Þ½ÃÁö
#define MC_MATCH_ANNOUNCE					402		///< °øÁö
#define MC_MATCH_RESPONSE_RESULT			403		///< request¿¡ ´ëÇÑ °á°ú¸¦ ¾Ë¸²(S -> C)

// °ü¸®ÀÚ Àü¿ë ¸í·É¾î
#define MC_ADMIN_ANNOUNCE					501		///< ÀüÃ¼ Á¢¼ÓÀÚ¿¡°Ô ¹æ¼Û
#define MC_ADMIN_REQUEST_SERVER_INFO		505		///< ¼­¹ö Á¤º¸ ¿äÃ»
#define MC_ADMIN_RESPONSE_SERVER_INFO		506		///< ¼­¹ö Á¤º¸ ÀÀ´ä
#define MC_ADMIN_SERVER_HALT				511		///< ¼­¹ö¸¦ Á¾·á ÇÑ´Ù
#define MC_ADMIN_TERMINAL					512		///< ÅÍ¹Ì³¯
#define MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE	513	///< °èÁ¤ ±ÇÇÑ º¯°æ ¿äÃ»
#define MC_ADMIN_RESPONSE_UPDATE_ACCOUNT_UGRADE	514	///< °èÁ¤ ±ÇÇÑ º¯°æ ÀÀ´ä
#define MC_ADMIN_REQUEST_KICK_PLAYER			515		///< Å¸°Ù Á¢¼Ó ²÷±â ¿äÃ»
#define MC_ADMIN_RESPONSE_KICK_PLAYER			516		///< Å¸°Ù Á¢¼Ó ²÷±â ÀÀ´ä
#define MC_ADMIN_REQUEST_MUTE_PLAYER			517		///< Å¸°Ù Ã¤ÆÃ ±ÝÁö ¿äÃ»
#define MC_ADMIN_RESPONSE_MUTE_PLAYER			518		///< Å¸°Ù Ã¤ÆÃ ±ÝÁö ÀÀ´ä
#define MC_ADMIN_REQUEST_BLOCK_PLAYER			519		///< Å¸°Ù Á¢¼Ó ºí·° ¿äÃ»
#define MC_ADMIN_RESPONSE_BLOCK_PLAYER			520		///< Å¸°Ù Á¢¼Ó ºí·° ÀÀ´ä
#define MC_ADMIN_PING_TO_ALL				521		///< Garbage Connection Ã»¼Ò È®ÀÎÀ» À§ÇÑ ¼öµ¿¸í·É
#define MC_ADMIN_REQUEST_SWITCH_LADDER_GAME	522		///< Å¬·£Àü ½ÅÃ» °¡´É¿©ºÎ ¼³Á¤
#define MC_ADMIN_HIDE						531		///< ¿î¿µÀÚ ¾Èº¸ÀÌ±â
#define MC_ADMIN_RELOAD_CLIENT_HASH			532		///< reload XTrap Hashmap
#define MC_ADMIN_RESET_ALL_HACKING_BLOCK	533		///< ¸ðµç ÇØÅ· ºí·°À» Ãë¼ÒÇÑ´Ù.
#define MC_ADMIN_RELOAD_GAMBLEITEM			534		///< °×ºí ¾ÆÀÌÅÛ Á¤º¸ ´Ù½Ã ÀÐ±â.
#define MC_ADMIN_DUMP_GAMBLEITEM_LOG		535		///< ÇöÁ¦ °×ºí ¾ÆÀÌÅÛ Á¤º¸¸¦ LogÆÄÀÏ¿¡ Ãâ·ÂÇÑ´Ù.
#define MC_ADMIN_ASSASIN					536		///< GMÀÌ Ä¿¸Ç´õ°¡ µÈ´Ù.
#define MC_ADMIN_REQUEST_SLAP				537


// ÀÌº¥Æ® ¸¶½ºÅÍ ¸í·É¾î
#define MC_EVENT_CHANGE_MASTER				601		///< ¹æÀå±ÇÇÑÀ» »¯¾î¿Â´Ù
#define MC_EVENT_CHANGE_PASSWORD			602		///< ¹æÀÇ ºñ¹Ð¹øÈ£¸¦ ¹Ù²Û´Ù
#define MC_EVENT_REQUEST_JJANG				611		///< Â¯¸¶Å© ºÎ¿©
#define MC_EVENT_REMOVE_JJANG				612		///< Â¯¸¶Å© È¸¼ö
#define MC_EVENT_UPDATE_JJANG				613		///< Â¯¸¶Å© ¾Ë¸²


// ¸ÅÄ¡¼­¹ö-Å¬¶óÀÌ¾ðÆ®
#define MC_MATCH_LOGIN							1001	///< ·Î±×ÀÎ
#define MC_MATCH_RESPONSE_LOGIN					1002	///< ·Î±×ÀÎ ÀÀ´ä
#define MC_MATCH_LOGIN_NETMARBLE				1003	///< ³Ý¸¶ºí¿¡¼­ ·Î±×ÀÎ
#define MC_MATCH_LOGIN_NETMARBLE_JP				1004	///< ÀÏº»³Ý¸¶ºí¿¡¼­ ·Î±×ÀÎ
#define MC_MATCH_LOGIN_FROM_DBAGENT				1005	///< DBAgent·ÎºÎÅÍÀÇ ·Î±×ÀÎ ÀÀ´ä(ÀÏº»³Ý¸¶ºí Àü¿ë)
#define MC_MATCH_LOGIN_FROM_DBAGENT_FAILED		1008	///< DBAgent·ÎºÎÅÍÀÇ ·Î±×ÀÎ ½ÇÆÐ(ÀÏº»³Ý¸¶ºí Àü¿ë)
#define MC_MATCH_FIND_HACKING					1009	///< ÇØÅ· °ËÃâ
#define MC_MATCH_DISCONNMSG						1010	///< Discconst¸Þ½ÃÁö.
#define MC_MATCH_LOGIN_NHNUSA					1011	///< ¹Ì±¹ NHN¿¡¼­ ·Î±×ÀÎ.
#define MC_MATCH_LOGIN_GAMEON_JP				1012	///< ÀÏº» gameon¿¡¼­ ·Î±×ÀÎ
#define MC_MATCH_CHECK_STAGEMODS				1013

#define MC_MATCH_BRIDGEPEER						1006	///< MatchServer¿¡ PeerÁ¤º¸ ¾Ë¸²
#define MC_MATCH_BRIDGEPEER_ACK					1007	///< PeerÁ¤º¸ ¼ö½ÅÈ®ÀÎ

#define MC_MATCH_OBJECT_CACHE					1101	///< ¿ÀºêÁ§Æ® Ä³½Ã

#define MC_MATCH_REQUEST_RECOMMANDED_CHANNEL	1201	///< ÃÖÃÊ Âü°¡ÇÒ Ã¤³Î¿¡´ëÇØ ÃßÃµÀ» ¿äÃ»ÇÑ´Ù.
#define MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL	1202	///< ÃÖÃÊ Âü°¡ÇÒ Ã¤³Î¿¡´ëÇØ ÃßÃµÇÑ´Ù.
#define MC_MATCH_CHANNEL_REQUEST_JOIN			1205	///< Ã¤³Î¿¡ Âü°¡(C -> S)
#define MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME	1206	///< Ã¤³ÎÀÌ¸§À¸·Î Ã¤³Î Á¶ÀÎ ¿äÃ»
#define MC_MATCH_CHANNEL_RESPONSE_JOIN			1207	///< Ã¤³Î Á¶ÀÎ ÀÀ´ä(S -> C)
#define MC_MATCH_CHANNEL_LEAVE					1208	///< Ã¤³Î Å»Åð
#define MC_MATCH_CHANNEL_LIST_START				1211	///< Ã¤³Î ¸ñ·Ï Àü¼Û½ÃÀÛ ¿äÃ»
#define MC_MATCH_CHANNEL_LIST_STOP				1212	///< Ã¤³Î ¸ñ·Ï Àü¼ÛÁßÁö ¿äÃ»
#define MC_MATCH_CHANNEL_LIST					1213	///< Ã¤³Î ¸ñ·Ï
#define MC_MATCH_CHANNEL_REQUEST_PLAYER_LIST	1221	///< ÇÃ·¹ÀÌ¾î ¸®½ºÆ® ¿äÃ»
#define MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST	1222	///< ÇÃ·¹ÀÌ¾î ¸®½ºÆ® ÀÀ´ä
#define MC_MATCH_CHANNEL_REQUEST_CHAT			1225	///< Ã¤³Î¿¡ ´ëÈ­ ¿äÃ»
#define MC_MATCH_CHANNEL_CHAT					1226	///< Ã¤³Î¿¡¼­ ´ëÈ­ ÀÀ´ä
#define MC_MATCH_CHANNEL_DUMB_CHAT				1227	///< Ã¤³Î¿¡¼­ ´ëÈ­ Â÷´Ü, º¡¾î¸® ±â´É - È«±âÁÖ(2009.08.05)

#define MC_MATCH_CHANNEL_REQUEST_RULE			1230	///< Ã¤³Î±ÔÄ¢ ¿äÃ»
#define MC_MATCH_CHANNEL_RESPONSE_RULE			1231	///< Ã¤³Î±ÔÄ¢ ÀÀ´ä
#define MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST		1232	///< Ã¤³ÎÀÇ ·Îºñ¿¡ ÀÖ´Â ¸ðµç ÇÃ·¹ÀÌ¾î ¸®½ºÆ® ¿äÃ»
#define MC_MATCH_CHANNEL_RESPONSE_ALL_PLAYER_LIST		1233	///< Ã¤³ÎÀÇ ·Îºñ¿¡ ÀÖ´Â ÇÃ·¹ÀÌ¾î ¸®½ºÆ® ¿äÃ»


#define MC_MATCH_STAGE_CREATE					1301	///< ½ºÅ×ÀÌÁö »ý¼º
#define MC_MATCH_RESPONSE_STAGE_CREATE			1302	///< ½ºÅ×ÀÌÁö »ý¼º ÀÀ´ä
#define MC_MATCH_STAGE_JOIN						1303	///< ½ºÅ×ÀÌÁö Âü°¡
#define MC_MATCH_REQUEST_STAGE_JOIN				1304	///< ½ºÅ×ÀÌÁö Âü°¡ ¿äÃ»
#define MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN		1305	///< ºñ¹Ð ½ºÅ×ÀÌÁö Âü°¡ ¿äÃ»
#define MC_MATCH_RESPONSE_STAGE_JOIN			1306	///< ½ºÅ×ÀÌÁö¿¡ Á¶ÀÎ¿©ºÎ¸¦ ¾Ë¸²
#define MC_MATCH_STAGE_LEAVE					1307	///< ½ºÅ×ÀÌÁö Å»Åð
#define MC_MATCH_STAGE_REQUEST_PLAYERLIST		1308	///< ½ºÅ×ÀÌÁöÀÇ À¯Àú¸ñ·Ï ¿äÃ»
#define MC_MATCH_STAGE_FOLLOW					1309	///< ÁöÁ¤ À¯Àú¸¦ µû¶ó¼­ ½ºÅ×ÀÌÁö Âü°¡
#define MC_MATCH_RESPONSE_STAGE_FOLLOW			1310	///< ÁöÁ¤ À¯Àú¸¦ µû¶ó¼­ ½ºÅ×ÀÌÁö Âü°¡ ÀÀ´ä
#define MC_MATCH_REQUEST_STAGE_LIST				1311	///< Å¬¶óÀÌ¾ðÆ®°¡ ½ºÅ×ÀÌÁö ¸®½ºÆ® ¿äÃ»
#define MC_MATCH_STAGE_LIST_START				1312	///< ½ºÅ×ÀÌÁö ¸ñ·Ï Àü¼Û½ÃÀÛ ¿äÃ»
#define MC_MATCH_STAGE_LIST_STOP				1313	///< ½ºÅ×ÀÌÁö ¸ñ·Ï Àü¼ÛÁßÁö ¿äÃ»
#define MC_MATCH_STAGE_LIST						1314	///< ½ºÅ×ÀÌÁö ¸ñ·Ï
#define MC_MATCH_STAGE_CHAT						1321	///< ½ºÅ×ÀÌÁö ´ëÈ­
#define MC_MATCH_STAGE_REQUEST_QUICKJOIN		1322	///< ÄüÁ¶ÀÎ ¿äÃ»
#define MC_MATCH_STAGE_RESPONSE_QUICKJOIN		1323	///< ÄüÁ¶ÀÎ ÀÀ´ä
#define MC_MATCH_STAGE_GO						1331	///< ½ºÅ×ÀÌÁö ¹øÈ£·Î Âü°¡
#define MC_MATCH_STAGE_REQUIRE_PASSWORD			1332	///< ºñ¹Ð¹æÀÌ¶ó ÆÐ½º¿öµå°¡ ÇÊ¿äÇÏ´Ù.(S -> C)


#define MC_MATCH_STAGE_REQUEST_ENTERBATTLE		1401	///< ½ºÅ×ÀÌÁö ÀüÅõ Âü°¡ÇÑ´Ù°í ¿äÃ»
#define MC_MATCH_STAGE_ENTERBATTLE				1402	///< ½ºÅ×ÀÌÁö ÀüÅõÂü°¡
#define MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER	1403	///< ½ºÅ×ÀÌÁö ÀüÅõÅ»Åð(C -> S)
#define MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT	1404	///< ½ºÅ×ÀÌÁö ÀüÅõÅ»Åð(S -> C)

#define MC_MATCH_REQUEST_STAGESETTING			1411	///< ½ºÅ×ÀÌÁöÀÇ ¼³Á¤À» ¿äÃ»
#define MC_MATCH_RESPONSE_STAGESETTING			1412	///< ½ºÅ×ÀÌÁöÀÇ ¼³Á¤À» ¾Ë¸²
#define MC_MATCH_STAGESETTING					1413	///< ½ºÅ×ÀÌÁö ¼³Á¤
#define MC_MATCH_STAGE_MAP						1414	///< ½ºÅ×ÀÌÁö ¸Ê ¼³Á¤
#define MC_MATCH_STAGE_REQUEST_FORCED_ENTRY		1415	///< ½ºÅ×ÀÌÁö¿¡¼­ °ÔÀÓ¿¡ ³­ÀÔ ¿äÃ»
#define MC_MATCH_STAGE_RESPONSE_FORCED_ENTRY	1416	///< ½ºÅ×ÀÌÁö °ÔÀÓ ³­ÀÔ ÀÀ´ä
#define MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE	1417	///< ½ºÅ×ÀÌÁö ¸±·¹ÀÌ¸Ê ¿ä¼Ò(Å¸ÀÔ, È¸Â÷) ¾÷µ¥ÀÌÆ®
#define MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE	1418	///< ½ºÅ×ÀÌÁö ¸±·¹ÀÌ¸Ê ¸®½ºÆ® ¾÷µ¥ÀÌÆ®

#define MC_MATCH_STAGE_MASTER					1421	///< ½ºÅ×ÀÌÁö ¹æÀå ¾Ë¸²
#define MC_MATCH_STAGE_PLAYER_STATE				1422	///< ½ºÅ×ÀÌÁö ·¹µð µîÀÇ ÇÃ·¹ÀÌ¾î »óÅÂ Á¤º¸
#define MC_MATCH_STAGE_TEAM						1423	///< ½ºÅ×ÀÌÁö ÆÀ
#define MC_MATCH_STAGE_START					1431	///< ½ºÅ×ÀÌÁö ½ÃÀÛ(Ä«¿îÆ®´Ù¿î)
#define MC_MATCH_STAGE_LAUNCH					1432	///< ½ºÅ×ÀÌÁö ½ÇÇà
#define MC_MATCH_STAGE_RELAY_LAUNCH				1433	///< ¸±·¹ÀÌµÈ ½ºÅ×ÀÌÁö ½ÇÇà

#define MC_MATCH_LOADING_COMPLETE				1441	///< ·ÎµùÀÌ ³¡³µÀ½
#define MC_MATCH_STAGE_FINISH_GAME				1442	///< ½ºÅ×ÀÌÁö °ÔÀÓ Á¾·á

#define MC_MATCH_REQUEST_GAME_INFO				1451	///< °ÔÀÓ¾È Á¤º¸¸¦ ´Þ¶ó ¿äÃ»
#define MC_MATCH_RESPONSE_GAME_INFO				1452	///< °ÔÀÓ¾È Á¤º¸¸¦ ¾Ë·ÁÁØ´Ù

#define MC_MATCH_REQUEST_PEERLIST				1461	///< Âü¿©ÁßÀÎ À¯ÀúµéÀÇ PeerÁ¤º¸¸¦ ¿äÃ»
#define MC_MATCH_RESPONSE_PEERLIST				1462	///< Âü¿©ÁßÀÎ À¯ÀúµéÀÇ PeerÁ¤º¸¸¦ ¾Ë¸²

#define MC_MATCH_REQUEST_PEER_RELAY				1471	///< ¸ÞÄ¡¼­¹ö¿¡ ÇØ´çPeer¿¡ ´ëÇÑ Áß°è¿äÃ»
#define MC_MATCH_RESPONSE_PEER_RELAY			1472	///< ÇØ´çPeer¿¡ ´ëÇÑ Áß°èÇã°¡ Åëº¸

// °ÔÀÓ °ü·Ã
#define MC_MATCH_GAME_ROUNDSTATE				1501	///< ¶ó¿îµåÀÇ »óÅÂ¸¦ µ¿±âÈ­
#define MC_MATCH_ROUND_FINISHINFO				1502	///< ¶ó¿îµå Á¾·á½Ã Á¤º¸ ¾÷µ¥ÀÌÆ® (C <- S)
#define MC_MATCH_GAME_KILL						1511	///< Ä³¸¯ÅÍ »ç¸Á (C -> S)
#define MC_MATCH_GAME_DEAD						1512	///< Ä³¸¯ÅÍ Á×¾ú´Ù (S -> C)
#define MC_MATCH_GAME_LEVEL_UP					1513	///< °ÔÀÓÁß ·¹º§¾÷
#define MC_MATCH_GAME_LEVEL_DOWN				1514	///< °ÔÀÓÁß ·¹º§´Ù¿î
#define MC_MATCH_GAME_REQUEST_SPAWN				1515	///< Ä³¸¯ÅÍ ºÎÈ°¿äÃ» (C -> S)
#define MC_MATCH_GAME_RESPONSE_SPAWN			1516	///< Ä³¸¯ÅÍ ºÎÈ°¿äÃ» (C -> S)
#define MC_MATCH_GAME_TEAMBONUS					1517	///< ÆÀ º¸³Ê½º(S -> C)

#define MC_MATCH_GAME_REQUEST_TIMESYNC			1521	///< Match¼­¹ö¿¡ ½Ã°£µ¿±âÈ­ ¿äÃ»
#define MC_MATCH_GAME_RESPONSE_TIMESYNC			1522	///< ½Ã°£µ¿±âÈ­
#define MC_MATCH_GAME_REPORT_TIMESYNC			1523	///< ½Ã°£µ¿±â °Ë»ç

#define MC_MATCH_REQUEST_SUICIDE				1531	///< ÀÚ»ì ¿äÃ»
#define MC_MATCH_RESPONSE_SUICIDE				1532	///< ÀÚ»ì ÀÀ´ä
#define MC_MATCH_RESPONSE_SUICIDE_RESERVE		1533	///< ÀÚ»ì ¿¹¾à.
#define MC_MATCH_NOTIFY_THROW_TRAPITEM			1535	///< Æ®·¦¾ÆÀÌÅÛ ÅõÃ´ ¾Ë¸²(C -> S)
#define MC_MATCH_NOTIFY_ACTIVATED_TRAPITEM		1536	///< Æ®·¦¾ÆÀÌÅÛ ¹ßµ¿ ¾Ë¸²(C -> S)
#define MC_MATCH_NOTIFY_ACTIATED_TRAPITEM_LIST	1437	///< ³­ÀÔÇÑ À¯Àú¿¡°Ô ÇöÀç ¹ßµ¿ ÁßÀÎ Æ®·¦¾ÆÀÌÅÛ ¸ñ·Ï ¾Ë·ÁÁÖ±â(S -> C)
#define MC_MATCH_REQUEST_OBTAIN_WORLDITEM		1541	///< ¾ÆÀÌÅÛ ¸Ôµµ·Ï ¿äÃ»
#define MC_MATCH_OBTAIN_WORLDITEM				1542	///< ¾ÆÀÌÅÛ ¸ÔÀ½ ºê·ÎµåÄ³½ºÆ®(S -> C)
#define MC_MATCH_SPAWN_WORLDITEM				1543	///< ¾ÆÀÌÅÛÀÌ ½ºÆùµÊ
#define MC_MATCH_REQUEST_SPAWN_WORLDITEM		1544	///< ¾ÆÀÌÅÛÀ» ¶³±¸µµ·Ï ¿äÃ»(C -> S)
#define MC_MATCH_REMOVE_WORLDITEM				1545	///< ¾ÆÀÌÅÛ ¾ø¾îÁü(S -> C)
#define MC_MATCH_ASSIGN_COMMANDER				1551	///< ¾Ï»ìÀü ´ëÀåÀ¸·Î ÀÓ¸í
#define MC_MATCH_RESET_TEAM_MEMBERS				1552	///< ÆÀ¸â¹ö ´Ù½Ã ¼¼ÆÃ
#define MC_MATCH_SET_OBSERVER					1553	///< °­Á¦·Î ¿ÉÀú¹ö¸ðµå·Î ÀüÈ¯


// µ¿ÀÇ°ü·Ã
#define MC_MATCH_REQUEST_PROPOSAL				1561	///< ´Ù¸¥ »ç¶÷ÀÇ µ¿ÀÇ ¿äÃ»
#define MC_MATCH_RESPONSE_PROPOSAL				1562	///< µ¿ÀÇ ¿äÃ» ÀÀ´ä
#define MC_MATCH_ASK_AGREEMENT					1563	///< ´Ù¸¥ »ç¶÷¿¡°Ô µ¿ÀÇ ÁúÀÇ(S -> C)
#define MC_MATCH_REPLY_AGREEMENT				1564	///< µ¿ÀÇ ÀÀ´ä(C -> S)

// ·¡´õ °ü·Ã
#define MC_MATCH_LADDER_REQUEST_CHALLENGE		1571	///< ·¡´õ µµÀü ¿äÃ»(C -> S)
#define MC_MATCH_LADDER_RESPONSE_CHALLENGE		1572	///< ·¡´õ µµÀü ÀÀ´ä
#define MC_MATCH_LADDER_SEARCH_RIVAL			1574	///< ·¡´õ °Ë»öÁß		
#define MC_MATCH_LADDER_REQUEST_CANCEL_CHALLENGE	1575	///< ·¡´õ µµÀü Ãë¼Ò¿äÃ»
#define MC_MATCH_LADDER_CANCEL_CHALLENGE		1576	///< ·¡´õ µµÀü Ãë¼Ò
#define MC_MATCH_LADDER_PREPARE					1578	///< ·¡´õ ½ÃÀÛÁØºñ
#define MC_MATCH_LADDER_LAUNCH					1579	///< ·¡´õ ·±Ä¡ (S -> C)

// ÀÇ»ç¼ÒÅë °ü·Ã
#define MC_MATCH_USER_WHISPER					1601	///< ±Ó¼Ó¸»
#define MC_MATCH_USER_WHERE						1602	///< À§Ä¡È®ÀÎ
#define MC_MATCH_USER_OPTION					1605	///< °¢Á¾¿É¼Ç(±Ó¸»°ÅºÎ,ÃÊ´ë°ÅºÎ,Ä£±¸°ÅºÎ µîµî)
#define MC_MATCH_CHATROOM_CREATE				1651	///< Ã¤ÆÃ·ë °³¼³
#define MC_MATCH_CHATROOM_JOIN					1652	///< Ã¤ÆÃ·ë Âü°¡
#define MC_MATCH_CHATROOM_LEAVE					1653	///< Ã¤ÆÃ·ë Å»Åð
#define MC_MATCH_CHATROOM_INVITE				1661	///< Ã¤ÆÃ·ë ÃÊ´ë
#define MC_MATCH_CHATROOM_CHAT					1662	///< Ã¤ÆÃ
#define MC_MATCH_CHATROOM_SELECT_WRITE			1665	///< Ã¤ÆÃ·ë ¼±ÅÃ

// Ä³¸¯ÅÍ °ü·Ã
#define MC_MATCH_REQUEST_ACCOUNT_CHARLIST		1701	///< °èÁ¤ÀÇ ÀüÃ¼ Ä³¸¯ÅÍ ¸®½ºÆ®¸¦ ¿äÃ»
#define MC_MATCH_RESPONSE_ACCOUNT_CHARLIST		1702	///< °èÁ¤ÀÇ ÀüÃ¼ Ä³¸¯ÅÍ ¸®½ºÆ®¸¦ ÀÀ´ä
#define MC_MATCH_REQUEST_SELECT_CHAR			1703	///< Ä³¸¯ÅÍ ¼±ÅÃ ¿äÃ»
#define MC_MATCH_RESPONSE_SELECT_CHAR			1704	///< Ä³¸¯ÅÍ ¼±ÅÃ ÀÀ´ä
#define MC_MATCH_REQUEST_MYCHARINFO				1705	///< ³» Ä³¸¯ÅÍ Á¤º¸ ¿äÃ»
#define MC_MATCH_RESPONSE_MYCHARINFO			1706	///< ³» Ä³¸¯ÅÍ Á¤º¸ ÀÀ´ä
#define MC_MATCH_REQUEST_CREATE_CHAR			1711	///< ³» Ä³¸¯ÅÍ »ý¼º ¿äÃ»
#define MC_MATCH_RESPONSE_CREATE_CHAR			1712	///< ³» Ä³¸¯ÅÍ »ý¼º ÀÀ´ä
#define MC_MATCH_REQUEST_DELETE_CHAR			1713	///< ³» Ä³¸¯ÅÍ »èÁ¦ ¿äÃ»
#define MC_MATCH_RESPONSE_DELETE_CHAR			1714	///< ³» Ä³¸¯ÅÍ »èÁ¦ ÀÀ´ä
#define MC_MATCH_REQUEST_COPY_TO_TESTSERVER		1715	///< Ä³¸¯ÅÍ Å×½ºÆ®¼­¹ö·Î Àü¼Û ¿äÃ» - ¹Ì±¸Çö
#define MC_MATCH_RESPONSE_COPY_TO_TESTSERVER	1716	///< Ä³¸¯ÅÍ Å×½ºÆ®¼­¹ö·Î Àü¼Û ÀÀ´ä - ¹Ì±¸Çö
#define MC_MATCH_REQUEST_CHARINFO_DETAIL		1717	///< ´Ù¸¥ ÇÃ·¹ÀÌ¾î Á¤º¸º¸±â µîÀÇ ÀÚ¼¼ÇÑ ÇÃ·¹ÀÌ¾î Á¤º¸ ¿äÃ»
#define MC_MATCH_RESPONSE_CHARINFO_DETAIL		1718	///< ´Ù¸¥ ÇÃ·¹ÀÌ¾î Á¤º¸º¸±â µîÀÇ ÀÚ¼¼ÇÑ ÇÃ·¹ÀÌ¾î Á¤º¸ ÀÀ´ä
#define MC_MATCH_REQUEST_ACCOUNT_CHARINFO		1719	///< °èÁ¤ÀÇ ÇÑ Ä³¸¯ÅÍ Á¤º¸ ¿äÃ»
#define MC_MATCH_RESPONSE_ACCOUNT_CHARINFO		1720	///< °èÁ¤ÀÇ ÇÑ Ä³¸¯ÅÍ Á¤º¸ ÀÀ´ä

// ¾ÆÀÌÅÛ °ü·Ã
#define MC_MATCH_REQUEST_SIMPLE_CHARINFO		1801	///< Ä³¸¯ÅÍÀÇ °£´ÜÇÑ Á¤º¸ ¿äÃ»	
#define MC_MATCH_RESPONSE_SIMPLE_CHARINFO		1802	///< Ä³¸¯ÅÍÀÇ °£´ÜÇÑ Á¤º¸ ÀÀ´ä
#define MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO		1803	///< ³» Ä³¸¯ÅÍ XP, BPµî Á¤º¸ ¿äÃ»
#define MC_MATCH_RESPONSE_MY_SIMPLE_CHARINFO	1804	///< ³» Ä³¸¯ÅÍ XP, BPµî Á¤º¸ ÀÀ´ä

#define MC_MATCH_REQUEST_BUY_ITEM				1811	///< ¾ÆÀÌÅÛ ±¸¸Å ¿äÃ»
#define MC_MATCH_RESPONSE_BUY_ITEM				1812	///< ¾ÆÀÌÅÛ ±¸¸Å ÀÀ´ä
#define MC_MATCH_REQUEST_SELL_ITEM				1813	///< ¾ÆÀÌÅÛ ÆÇ¸Å ¿äÃ»
#define MC_MATCH_RESPONSE_SELL_ITEM				1814	///< ¾ÆÀÌÅÛ ÆÇ¸Å ÀÀ´ä
#define MC_MATCH_REQUEST_SHOP_ITEMLIST			1815	///< ¼¥¿¡¼­ ÆÇ¸ÅÇÏ´Â ¾ÆÀÌÅÛ ¸®½ºÆ® ¿äÃ»
#define MC_MATCH_RESPONSE_SHOP_ITEMLIST			1816	///< ¼¥¿¡¼­ ÆÇ¸ÅÇÏ´Â ¾ÆÀÌÅÛ ¸®½ºÆ® ÀÀ´ä

#define MC_MATCH_REQUEST_CHARACTER_ITEMLIST		1821	///< ³» ¾ÆÀÌÅÛ ¸®½ºÆ® ÁÖ½Ã¿À
#define MC_MATCH_RESPONSE_CHARACTER_ITEMLIST	1822	///< ¾ÆÀÌÅÛ ¸®½ºÆ® ¿©ƒ…¿À
#define MC_MATCH_REQUEST_EQUIP_ITEM				1823	///< ¾ÆÀÌÅÛ Àåºñ ¿äÃ»
#define MC_MATCH_RESPONSE_EQUIP_ITEM			1824	///< ¾ÆÀÌÅÛ Àåºñ ÀÀ´ä
#define MC_MATCH_REQUEST_TAKEOFF_ITEM			1825	///< ¾ÆÀÌÅÛ ÇØÁ¦ ¿äÃ»
#define MC_MATCH_RESPONSE_TAKEOFF_ITEM			1826	///< ¾ÆÀÌÅÛ ÇØÁ¦ ÀÀ´ä
#define MC_RESPONSE_GAMBLEITEMLIST				1827	///< DBÀÇ °×ºí ¾ÆÀÌÅÛ Á¤º¸ Àü¼Û.
#define MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK	1828	///< Ä³¸¯ÅÍÀÇ Look¸¸µé ¾÷µ¥ÀÌÆ®ÇÒ¶§ »ç¿ëÇÑ´Ù.
#define MC_MATCH_REQUEST_CHARACTER_ITEMLIST_FORCE	1829	///< ³» ¾ÆÀÌÅÛ ¸®½ºÆ® ÁÖ½Ã¿À : Áßº¹ ¿äÃ»µµ ¹«½ÃÇÏÁö ¾Ê´Â ¹öÀü(ÁÖÀÇÇØ¼­ »ç¿ëÇÒ°Í)

#define MC_MATCH_REQUEST_ACCOUNT_ITEMLIST			1831	///< ³» Ã¢°í ¾ÆÀÌÅÛ ¸®½ºÆ® ÁÖ½Ã¿À
#define MC_MATCH_RESPONSE_ACCOUNT_ITEMLIST			1832	///< Ã¢°í ¾ÆÀÌÅÛ ¸®½ºÆ® ¿©ƒ…¿À
#define MC_MATCH_REQUEST_BRING_ACCOUNTITEM			1833	///< Ã¢°í ¾ÆÀÌÅÛÀ» ³» Ä³¸¯ÅÍ·Î °¡Á®¿À±â
#define MC_MATCH_RESPONSE_BRING_ACCOUNTITEM			1834	///< Ã¢°í ¾ÆÀÌÅÛ °¡Á®¿À±â ÀÀ´ä
#define MC_MATCH_REQUEST_BRING_BACK_ACCOUNTITEM		1835	///< ³» Ä³½¬¾ÆÀÌÅÛ Ã¢°í·Î ¿Å±â±â ¿äÃ»
#define MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM	1836	///< ³» Ä³½¬¾ÆÀÌÅÛ Ã¢°í·Î ¿Å±â±â ÀÀ´ä
#define MC_MATCH_EXPIRED_RENT_ITEM					1837	///< ¾ÆÀÌÅÛÀÌ ±â°£ ¸¸·áµÇ¾ú½À´Ï´Ù.(S -> C)

#define MC_MATCH_REQUEST_GAMBLE					1841	///< »Ì±â (C -> S)
#define MC_MATCH_RESPONSE_GAMBLE				1842	///< »Ì±â °á°ú (S -> C)

#define MC_MATCH_REWARD_BATTLE_TIME				1843	///< ¹èÆ² ½Ã°£ º¸»ó °øÁö(S->C)

// ¾ÆÀÌÅÛ °ü·Ã(¼Ò¸ð¼º)
#define MC_MATCH_REQUEST_USE_SPENDABLE_NORMAL_ITEM		1850	///< ÀÏ¹Ý ¼Ò¸ð¼º ¾ÆÀÌÅÛ »ç¿ë ¿äÃ»(C -> S)
#define MC_MATCH_REQUEST_USE_SPENDABLE_BUFF_ITEM		1860	///< ¹öÇÁ ¼Ò¸ð¼º ¾ÆÀÌÅÛ »ç¿ë ¿äÃ»(C -> S)
#define MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM		1861	///< ¹öÇÁ ¼Ò¸ð¼º ¾ÆÀÌÅÛ »ç¿ë °á°ú(S -> C)
#define MC_MATCH_SPENDABLE_BUFF_ITEM_STATUS				1865	///< ¹öÇÁ ¼Ò¸ð¼º ¾ÆÀÌÅÛ »óÅÂ(S -> C)
#define MC_MATCH_REQUEST_USE_SPENDABLE_COMMUNITY_ITEM	1870	///< Ä¿¹Â´ÏÆ¼ ¼Ò¸ð¼º ¾ÆÀÌÅÛ »ç¿ë ¿äÃ»(C -> S)

// Ä£±¸ °ü·Ã
#define MC_MATCH_FRIEND_ADD						1901	///< Ä£±¸ Ãß°¡
#define MC_MATCH_FRIEND_REMOVE					1902	///< Ä£±¸ »èÁ¦
#define MC_MATCH_FRIEND_LIST					1903	///< Ä£±¸ ¸ñ·Ï ¿äÃ»
#define MC_MATCH_RESPONSE_FRIENDLIST			1904	///< Ä£±¸ ¸ñ·Ï Àü¼Û
#define MC_MATCH_FRIEND_MSG						1905	///< Ä£±¸ Ã¤ÆÃ


// Å¬·£°ü·Ã
#define MC_MATCH_CLAN_REQUEST_CREATE_CLAN			2000	///< Å¬·£ »ý¼º ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_CREATE_CLAN			2001	///< Å¬·£ »ý¼º ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_ASK_SPONSOR_AGREEMENT			2002	///< ¹ß±âÀÎ¿¡°Ô Å¬·£ »ý¼º µ¿ÀÇ ÁúÀÇ(S -> C)
#define MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT		2003	///< ¹ß±âÀÎÀÌ µ¿ÀÇ¸¦ ÀÀ´ä(C -> S)
#define MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN	2004	///< ¹ß±âÀÎÀÌ µ¿ÀÇÇÑ Å¬·£ »ý¼º ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN	2005	///< ¹ß±âÀÎÀÌ µ¿ÀÇÇÑ Å¬·£ »ý¼º ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_REQUEST_CLOSE_CLAN			2006	///< ¸¶½ºÅÍ°¡ Å¬·£ Æó¼â ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN			2007	///< ¸¶½ºÅÍÀÇ Å¬·£ Æó¼â ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_REQUEST_JOIN_CLAN				2008	///< ¿î¿µÀÚ°¡ °¡ÀÔÀÚÀÇ °¡ÀÔÀ» ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_JOIN_CLAN			2009	///< ¿î¿µÀÚÀÇ °¡ÀÔÀÚ °¡ÀÔÀÇ ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_ASK_JOIN_AGREEMENT			2010	///< °¡ÀÔÀÚ¿¡°Ô °¡ÀÔ µ¿ÀÇ ÁúÀÇ(S -> C)
#define MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT			2011	///< °¡ÀÔÀÚÀÇ °¡ÀÔ µ¿ÀÇ ÀÀ´ä(C -> S)
#define MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN		2012	///< °¡ÀÔÀÚ°¡ µ¿ÀÇÇÑ Å¬·£ °¡ÀÔ ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN		2013	///< °¡ÀÔÀÚ°¡ µ¿ÀÇÇÑ Å¬·£ °¡ÀÔ ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_REQUEST_LEAVE_CLAN			2014	///< Å¬·£¿øÀÌ Å»Åð¸¦ ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN			2015	///< Å¬·£¿øÀÇ Å»ÅðÀÇ ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_UPDATE_CHAR_CLANINFO			2016	///< Ä³¸¯ÅÍÀÇ Å¬·£Á¤º¸°¡ ¾÷µ¥ÀÌÆ®µÊ(S -> C)
#define MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE	2017	///< ¸â¹öÀÇ ±ÇÇÑ º¯°æ ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE	2018	///< ¸â¹öÀÇ ±ÇÇÑ º¯°æ ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER	2019	///< ¸â¹ö¸¦ Å»Åð ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER	2020	///< ¸â¹ö¸¦ Å»Åð ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_REQUEST_MSG					2021	///< Å¬·£ Ã¤ÆÃ ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_MSG							2022	///< Å¬·£ Ã¤ÆÃ(S -> C)
#define MC_MATCH_CLAN_REQUEST_MEMBER_LIST			2023	///< Å¬·£ ¸â¹ö ¸®½ºÆ® ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_MEMBER_LIST			2024	///< Å¬·£ ¸â¹ö ¸®½ºÆ® ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_REQUEST_CLAN_INFO				2025	///< Å¬·£ Á¤º¸ ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_CLAN_INFO			2026	///< Å¬·£ Á¤º¸ ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_STANDBY_CLAN_LIST				2027	///< Å¬·£Àü ´ë±âÁßÀÎ Å¬·£ ¸®½ºÆ® ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_MEMBER_CONNECTED				2028	///< Å¬·£¿øÀÌ Á¢¼ÓÇÔ(S -> C)
#define MC_MATCH_CLAN_REQUEST_EMBLEMURL				2051	///< Å¬·£¸¶Å© URL ¿äÃ»(C -> S)
#define MC_MATCH_CLAN_RESPONSE_EMBLEMURL			2052	///< Å¬·£¸¶Å© URL ÀÀ´ä(S -> C)
#define MC_MATCH_CLAN_LOCAL_EMBLEMREADY				2055	///< Å¬·£¸¶Å© ´Ù¿î·Îµå ¿Ï·á(C -> C)
#define MC_MATCH_CLAN_ACCOUNCE_DELETE				2056	///< Å¬·£ Æó¼â ¿äÃ» Á¢¼ö Á¤º¸ °øÁö.(S -> C)


// ÅõÇ¥°ü·Ã
#define MC_MATCH_CALLVOTE							2100	///< ÅõÇ¥¸¦ Á¦¾ÈÇÑ´Ù. (C -> S)
#define MC_MATCH_NOTIFY_CALLVOTE					2101	///< ÅõÇ¥Á¦¾ÈÀ» ¾Ë¸°´Ù. (S -> C)
#define MC_MATCH_NOTIFY_VOTERESULT					2102	///< ÅõÇ¥°á°ú¸¦ ¾Ë¸°´Ù. (S -> C)
#define MC_MATCH_VOTE_YES							2105	///< ÅõÇ¥ (C -> S)
#define MC_MATCH_VOTE_NO							2106	///< ÅõÇ¥ (C -> S)
#define MC_MATCH_VOTE_RESPONSE						2107	///< ÅõÇ¥ ¿¹¿Ü.(S -> C)
#define MC_MATCH_VOTE_STOP							2108	///< ÅõÇ¥ Ãë¼Ò. (S -> C)

// ¹æ¼Û°ü·Ã
#define MC_MATCH_BROADCAST_CLAN_RENEW_VICTORIES			2200	///< Å¬·£ÀÌ °è¼Ó ¿¬½ÂÁßÀÔ´Ï´Ù.(S -> C)
#define MC_MATCH_BROADCAST_CLAN_INTERRUPT_VICTORIES		2201	///< Å¬·£ÀÇ ¿¬½ÂÀ» ÀúÁöÇÏ¿´½À´Ï´Ù.(S -> C)
#define MC_MATCH_BROADCAST_DUEL_RENEW_VICTORIES			2202	///< xx´ÔÀÌ xxÃ¤³ÎÀÇ xx¹æ¿¡¼­ x¿¬½ÂÁßÀÔ´Ï´Ù.(S -> C)
#define MC_MATCH_BROADCAST_DUEL_INTERRUPT_VICTORIES		2203	///< xx´ÔÀÌ xx´ÔÀÇ xx¿¬½ÂÀ» ÀúÁöÇß½À´Ï´Ù. (S -> C)


// Duel Tournament °ü·Ã.. Added by È«±âÁÖ
#define MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME				2300	///< µà¾ó Åä³Ê¸ÕÆ®ÀÇ Âü°¡ ½ÅÃ»(C -> S)
#define MC_MATCH_DUELTOURNAMENT_RESPONSE_JOINGAME				2301	///< µà¾ó Åä³Ê¸ÕÆ®ÀÇ Âü°¡ ½ÅÃ»¿¡ ´ëÇÑ ÀÀ´ä(S -> C)
#define MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME				2302	///< µà¾ó Åä³Ê¸ÕÆ®ÀÇ Âü°¡ ½ÅÃ» Ãë¼Ò(C -> S)
#define MC_MATCH_DUELTOURNAMENT_CHAR_INFO						2303	///< µà¾ó Åä³Ê¸ÕÆ®ÀÇ ÄÉ¸¯ÅÍ Á¤º¸(S -> C)
#define MC_MATCH_DUELTOURNAMENT_PREPARE_MATCH					2304	///< µà¾ó Åä³Ê¸ÕÆ® ½ÃÀÛ Àü
#define MC_MATCH_DUELTOURNAMENT_LAUNCH_MATCH					2305	///< µà¾ó Åä³Ê¸ÕÆ® ½ÃÀÛ! (S - > C)
#define MC_MATCH_DUELTOURNAMENT_CANCEL_MATCH					2306	///< µà¾ó Åä³Ê¸ÕÆ® Ãë¼Ò! (S - > C)
#define MC_MATCH_DUELTOURNAMENT_CHAR_INFO_PREVIOUS				2307	///< µà¾ó Åä³Ê¸ÕÆ® Ä³¸¯ÅÍ Áö³­ÁÖ Á¤º¸(S -> C)
#define MC_MATCH_DUELTOURNAMENT_NOT_SERVICE_TIME				2308	///< ÇöÀç µà¾óÅä³Ê¸ÕÆ® ¼­ºñ½º ½Ã°£ÀÌ ¾Æ´Õ´Ï´Ù.(¿ÀÇÂ½Ã°£ %s%d½Ã00ºÐ ~ %s%d½Ã59ºÐ) (S - > C)

#define MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO		2400	///< ÄÉ¸¯ÅÍ ·©Å·ÀÇ ±ÙÃ³ ·©Å· Á¤º¸ ¿äÃ»(C -> S)
#define MC_MATCH_DUELTOURNAMENT_RESPONSE_SIDERANKING_INFO		2401	///< ÄÉ¸¯ÅÍ ·©Å·ÀÇ ±ÙÃ³ ·©Å· Á¤º¸ ÀÀ´ä(S -> C)
#define MC_MATCH_DUELTOURNAMENT_GROUPRANKING_INFO				2402	///< µà¾ó Åä³Ê¸ÕÆ®ÀÇ ±×·ì ·©Å· Á¤º¸(S -> C)

#define MC_MATCH_DUELTOURNAMENT_GAME_INFO						2500	///< µà¾ó Åä³Ê¸ÕÆ® °ÔÀÓ ³»¿¡¼­ÀÇ Á¤º¸
#define MC_MATCH_DUELTOURNAMENT_GAME_ROUND_RESULT_INFO			2501	///< µà¾ó Åä³Ê¸ÕÆ® °ÔÀÓ ³»¿¡¼­ÀÇ ¶ó¿îµå ½ÂÆÐ °á°ú Á¤º¸
#define MC_MATCH_DUELTOURNAMENT_GAME_MATCH_RESULT_INFO			2502	///< µà¾ó Åä³Ê¸ÕÆ® °ÔÀÓ ³»¿¡¼­ÀÇ ¸ÅÄ¡ ½ÂÆÐ °á°ú Á¤º¸
#define MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS				2503	///< µà¾ó Åä³Ê¸ÕÆ® °ÔÀÓ ³»¿¡¼­ÀÇ ¶ó¿îµå°¡ Á¾·áµÉ ¶§, ÇØ´ç °ÔÀÌ¸ÓÀÇ »óÅÂ¸¦ ¼­¹ö·Î º¸³¿(C -> S)
#define MC_MATCH_DUELTOURNAMENT_GAME_NEXT_MATCH_PLYAERINFO		2504	///< µà¾ó Åä³Ê¸ÕÆ® °ÔÀÓ ³»¿¡¼­ÀÇ ´ÙÀ½ ¸ÅÄ¡¾÷ÀÇ ÇÃ·¹ÀÌ¾î Á¤º¸

// °ÔÀÓ ·ê °ü·Ã
// ¹ö¼­Ä¿ ¸ðµå
#define MC_MATCH_ASSIGN_BERSERKER					3001	///< ¹ö¼­Ä¿ ÀÓ¸í(S -> C)


#define MC_MATCH_DUEL_QUEUEINFO						3100	///< ´ë±â¿­ ¼ø¹ø ¾Ë¸²(S -> C)


// ¸ÅÄ¡ ¿¡ÀÌÁ¯Æ® °ü·Ã ¸í·É¾î
#define MC_MATCH_REGISTERAGENT				5001	///< ¿¡ÀÌÁ¯Æ® µî·Ï
#define MC_MATCH_UNREGISTERAGENT			5002	///< ¿¡ÀÌÁ¯Æ® ÇØÁö
#define MC_MATCH_AGENT_REQUEST_LIVECHECK	5011	///< ¿¡ÀÌÁ¯Æ® È®ÀÎ¿äÃ»
#define MC_MATCH_AGENT_RESPONSE_LIVECHECK	5012	///< ¿¡ÀÌÁ¯Æ® È®ÀÎÀÀ´ä
#define MC_AGENT_ERROR						5013	///< ¿¡ÀÌÀüÆ® °ü·Ã ¿¡·¯

#define MC_AGENT_CONNECT					5021	///< ¸ÅÄ¡¼­¹ö¿¡ ¿¡ÀÌÁ¯Æ® ¼­¹ö µî·Ï
#define MC_AGENT_DISCONNECT					5022	///< ¸ÅÄ¡¼­¹ö¿¡ ¿¡ÀÌÁ¯Æ® ¼­¹ö µî·ÏÇØÁö
#define MC_AGENT_LOCAL_LOGIN				5023	///< Å¬¶óÀÌ¾ðÆ® Á¢¼ÓÃ³¸®
#define MC_AGENT_RESPONSE_LOGIN				5024	///< Å¬¶óÀÌ¾ðÆ®¿¡ ·Î±×ÀÎ ÅëÁö
//#define MC_AGENT_MATCH_VALIDATE				5031	///< ¸ÅÄ¡¼­¹ö¿ÍÀÇ Á¢¼ÓÈ®ÀÎ
#define MC_AGENT_STAGE_RESERVE				5051	///< ¿¡ÀÌÀüÆ®¿¡ ½ºÅ×ÀÌÁö ¹èÁ¤
#define MC_AGENT_STAGE_RELEASE				5052	///< ¿¡ÀÌÀüÆ®¿¡ ½ºÅ×ÀÌÁö ÇØÁö
#define MC_AGENT_STAGE_READY				5053	///< ¿¡ÀÌÀüÆ®¿¡ ½ºÅ×ÀÌÁö ÁØºñ¿Ï·á
#define MC_AGENT_LOCATETO_CLIENT			5061	///< Å¬¶óÀÌ¾ðÆ®¿¡°Ô Agent ¾Ë¸²
#define MC_AGENT_RELAY_PEER					5062	///< ¿¡ÀÌÀüÆ®¿¡ Áß°èÁö½Ã
#define MC_AGENT_PEER_READY					5063	///< ¸ÞÄ¡¼­¹ö¿¡ ÇÇ¾îÁØºñ ¾Ë¸²
#define MC_AGENT_PEER_BINDTCP				5071	///< Å¬¶óÀÌ¾ðÆ® Bind TCP
#define MC_AGENT_PEER_BINDUDP				5072	///< Å¬¶óÀÌ¾ðÆ® Bind UDP
#define MC_AGENT_PEER_UNBIND				5073	///< Å¬¶óÀÌ¾ðÆ® Unbind

#define MC_AGENT_TUNNELING_TCP				5081	///< TCP ÅÍ³Î¸µ
#define MC_AGENT_TUNNELING_UDP				5082	///< UDP ÅÍ³Î¸µ
#define MC_AGENT_ALLOW_TUNNELING_TCP		5083	///< TCP ÅÍ³Î¸µ Çã¿ë
#define MC_AGENT_ALLOW_TUNNELING_UDP		5084	///< UDP ÅÍ³Î¸µ Çã¿ë
#define MC_AGENT_DEBUGPING					5101	///< µð¹ö±ë¿ë
#define MC_AGENT_DEBUGTEST					5102	///< µð¹ö±ë¿ë

#define MC_TEST_BIRDTEST1					60001	///< ¹öµå Å×½ºÆ®1
#define MC_TEST_PEERTEST_PING				60002	///< Target Peer Test
#define MC_TEST_PEERTEST_PONG				60003	///< Target Peer Test



/// Ä¿¸Çµå ¼¼ºÎ ¿É¼Ç

// MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LISTÀÇ ¼¼ºÎ¿É¼Ç
#define MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_NORMAL		0		
#define MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_NONCLAN		1
#define MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_MYCLAN		2


// Äù½ºÆ® °ü·Ã ¸í·É¾î(¼­¹ÙÀÌ¹ú ¸ðµå, Äù½ºÆ® ¸ðµå °øÅë)
#define MC_QUEST_NPC_SPAWN					6000	///< NPC ½ºÆù(S -> C)
#define MC_QUEST_ENTRUST_NPC_CONTROL		6001	///< NPCÀÇ Á¶Á¾À» ÇÇ¾î¿¡°Ô À§ÀÓ(S -> C)
#define MC_QUEST_CHECKSUM_NPCINFO			6002	///< NPCÁ¤º¸µéÀÇ Ã¼Å©¼¶À» ¼­¹ö¿¡°Ô º¸³½´Ù(C -> S)
#define MC_QUEST_REQUEST_NPC_DEAD			6003	///< NPC Á×À½ ¿äÃ»(C -> S)
#define MC_QUEST_NPC_DEAD					6004	///< NPC Á×À½(S -> C)
#define MC_QUEST_REFRESH_PLAYER_STATUS		6005	///< Á×¾úÀ¸¸é ´Ù½Ã ÅÂ¾î³ª°í, ¸ðµç »óÅÂ¸¦ ÃÖ»óÀ¸·Î º¹±Í(S -> C)
#define MC_QUEST_NPC_ALL_CLEAR				6006	///< NPC ¸ðµÎ »èÁ¦(S -> C)
#define MC_MATCH_QUEST_REQUEST_DEAD			6007	///< ÇÃ·¹ÀÌ¾î Á×¾ú´Ù°í ¿äÃ»(C -> S) Äù½ºÆ®¸ðµå¿¡¼­´Â MC_MATCH_GAME_KILL´ë½Å º¸³½´Ù.
#define MC_MATCH_QUEST_PLAYER_DEAD			6008	///< ÇÃ·¹ÀÌ¾î Á×À½(S -> C)
#define MC_QUEST_OBTAIN_QUESTITEM			6009	///< Äù½ºÆ® ¾ÆÀÌÅÛ È¹µæ(S -> C)
#define MC_QUEST_STAGE_MAPSET				6010	///< ½ºÅ×ÀÌÁö ¸Ê¼Â º¯°æ(S <-> C)
#define MC_QUEST_OBTAIN_ZITEM				6011	///< ÀÏ¹Ý ¾ÆÀÌÅÛ È¹µæ(S -> C)
#define MC_QUEST_PING						6012	///< ¼­¹ö->Å¬¶ó ÇÎ (S -> C)
#define MC_QUEST_PONG						6013	///< Å¬¶ó->¼­¹ö ÇÎ (C -> S)
#define MC_QUEST_NPCLIST					6014	///< ÇöÁ¦ Äù½ºÆ®¿¡¼­ »ç¿ëÇÒ NPCÀÇ Á¤º¸.(S->C) 
///<  Å¬¶ó´Â ¼­¹öÀÇ Á¤º¸¸¦ °¡Áö°í NPC¸¦ ÃÊ±âÈ­ ÇÑ´Ù.


#define MC_QUEST_PEER_NPC_BASICINFO			6040	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â NPC ±âº»Á¤º¸
#define MC_QUEST_PEER_NPC_HPINFO			6041	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â NPC Ã¼·ÂÁ¤º¸
#define MC_QUEST_PEER_NPC_ATTACK_MELEE		6042	///< ±ÙÁ¢ °ø°Ý
#define MC_QUEST_PEER_NPC_ATTACK_RANGE		6043	///< ¿ø°Å¸® °ø°Ý
#define MC_QUEST_PEER_NPC_SKILL_START		6044	///< ½ºÅ³ Ä³½ºÆÃ ½ÃÀÛ
#define MC_QUEST_PEER_NPC_SKILL_EXECUTE		6045	///< ½ºÅ³ Ä³½ºÆÃ¼º°ø È¿°ú ½ÃÀÛ
#define MC_QUEST_PEER_NPC_DEAD				6046	///< Á×Àº ±× ½ÃÁ¡¿¡ ÇÇ¾îµé¿¡°Ô º¸³½´Ù. 
#define MC_QUEST_PEER_NPC_BOSS_HPAP			6047	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â NPC º¸½º Ã¼·Â ¾Æ¸Ó

// Äù½ºÆ® °ÔÀÓ ÁøÇà °ü·Ã(Äù½ºÆ® ¸ðµå)
#define MC_QUEST_GAME_INFO					6051	///< °ÔÀÓ Á¤º¸. ·ÎµùÇÒ ¸®¼Ò½º µîÀ» ¾Ë¸²(S -> C)
#define MC_QUEST_COMBAT_STATE				6052	///< Äù½ºÆ® °ÔÀÓ³» »óÅÂ Á¤º¸(S -> C)
#define MC_QUEST_SECTOR_START				6053	///< ¸Ê¼½ÅÍ °ÔÀÓ ½ÃÀÛ(S -> C)
#define MC_QUEST_COMPLETED					6054	///< ÇØ´ç Äù½ºÆ® Å¬¸®¾î(S -> C)
#define MC_QUEST_FAILED						6055	///< Äù½ºÆ® ½ÇÆÐ(S -> C)

#define MC_QUEST_REQUEST_MOVETO_PORTAL		6057	///< Æ÷Å»·Î ÀÌµ¿ÇÔ(C -> S)
#define MC_QUEST_MOVETO_PORTAL				6058	///< Æ÷Å»·Î ÀÌµ¿(S -> C)
#define MC_QUEST_READYTO_NEWSECTOR			6059	///< »õ·Î¿î ¼½ÅÍ ÁØºñ ¿Ï·á(S <-> C)

#define MC_GAME_START_FAIL					6060	///< Äù½ºÆ® ½ÃÀÛÀ» ½ÇÆÐ.(S -> C)
#define MC_QUEST_STAGE_GAME_INFO			6061	///< ´ë±âÁß ½ºÅ×ÀÌÁö Á¤º¸ ¾Ë¸²(QL, ½Ã³ª¸®¿ÀID). (S -> C)
#define MC_QUEST_SECTOR_BONUS				6062	///< ¼½ÅÍ º¸³Ê½º(S -> C)

// Äù½ºÆ® °ÔÀÓ ÁøÇà °ü·Ã(¼­¹ÙÀÌ¹ú ¸ðµå)
#define MC_QUEST_ROUND_START				6100	///< »õ·Î¿î ¶ó¿îµå ½ÃÀÛ(S -> C)
#define MC_QUEST_REQUEST_QL					6101	///< ÇöÀç Äù½ºÆ® ½ºÅ×ÀÌÁöÀÇ QLÁ¤º¸¸¦ ¿äÃ».(C -> S)
#define MC_QUEST_RESPONSE_QL				6102	///< ÇöÀç Äù½ºÆ® ½ºÅ×ÀÌÁöÀÇ QLÁ¤º¸¸¦ ¾Ë¸².(S -> C)
#define MC_QUEST_SURVIVAL_RESULT			6103	///< ¼­¹ÙÀÌ¹ú °á°ú Åëº¸(S -> C)
#define MC_SURVIVAL_RANKINGLIST				6104	///< ¼­¹ÙÀÌ¹ú »óÀ§ ·©Å· ¸ñ·Ï Àü¼Û(S -> C)
#define MC_SURVIVAL_PRIVATERANKING			6105	///< À¯ÀúÀÇ ¼­¹ÙÀÌ¹ú ·©Å· Á¤º¸ Àü¼Û(S -> C)


// Äù½ºÆ® Å×½ºÆ®¿ë
#define MC_QUEST_NPC_LOCAL_SPAWN			53000
#define MC_QUEST_TEST_REQUEST_NPC_SPAWN		6901	///< NPC ½ºÆù ºÎÅ¹(C -> S)
#define MC_QUEST_TEST_REQUEST_CLEAR_NPC		6902	///< NPC Clear ºÎÅ¹(C -> S)
#define MC_QUEST_TEST_REQUEST_SECTOR_CLEAR	6903	///< ¼½ÅÍ Clear(C -> S)	
#define MC_QUEST_TEST_REQUEST_FINISH		6904	///< Äù½ºÆ® Complete(C -> S)

/// ½Å±ÔÄù½ºÆ®¿ë (Ã§¸°Áö Äù½ºÆ®)
#define MC_NEWQUEST_NPC_SPAWN				6301	///< NPC ½ºÆù (S -> C)
#define MC_NEWQUEST_REQUEST_NPC_SPAWN		6304	///< NPC ½ºÆE(C -> S)
#define MC_NEWQUEST_REQUEST_NPC_DEAD		6302	///< NPC Á×À½ ¿äÃ»(C -> S)
#define MC_NEWQUEST_NPC_DEAD				6303	///< NPC Á×À½(S -> C)
#define MC_NEWQUEST_MOVE_TO_NEXT_SECTOR		6341	///< ´ÙÀ½ ¼½ÅÍ·Î ÀÌµ¿ (S -> C)
#define MC_NEWQUEST_PEER_NPC_BASICINFO		6351	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â NPC ±âº»Á¤º¸
#define MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE	6404	///< NPC ¾×¼Ç ½ÃÀÛ
#define MC_NEWQUEST_PEER_NPC_ATTACK_MELEE	6405	///< NPC ±ÙÁ¢°ø°Ý	//todok del ÀÌ°Ç ¾×¼Ç ½ÇÇàÅëÁö¸¦ ¹ÞÀ¸¸é peerµéÀÌ °¢ÀÚ Ã³¸®ÇÏ°Ô µÆ´Ù
#define MC_NEWQUEST_NEW_CONTROLLER			6406
#define MC_NEWQUEST_LATE_NPC_SPAWN			6407
#define MC_NEWQUEST_STAGE_GAME_INFO			6408
// MatchServer Schedule°ü·Ã.
#define MC_MATCH_SCHEDULE_ANNOUNCE_MAKE				7001	///< ½ºÄÉÁì °øÁö »ý¼º.
#define MC_MATCH_SCHEDULE_ANNOUNCE_SEND				7002	///< ½ºÄÉÁì °øÁö Àü¼Û.
#define MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_DOWN	7003	///< Å¬·£¼­¹ö Å¬·£Àü ºñÈ°¼ºÈ­.
#define MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_ON		7004	///< Å¬·£¼­¹ö Å¬·£Àü È°¼ºÈ­.
#define MC_MATCH_SCHEDULE_STOP_SERVER				7005	///< ¼­¹ö Á¾·á.

// ÀÚ¼ö.
#define MC_REQUEST_GIVE_ONESELF_UP					9101	///< ÀÚ¼öÇÏ´Ù.


//// ÇÇ¾îÅõÇÇ¾î °ü·Ã ¸í·É¾î /////////////////////////////////////////////////////////////
#define MC_PEER_PING				10001	///< peerµé »çÀÌÀÇ network latency(ping)À» ÃøÁ¤ÇÏ±âÀ§ÇÑ command
#define MC_PEER_PONG				10002	///< ping¿¡ ´ëÇÑ ÀÀ´ä
#define MC_PEER_UDPTEST				10005	///< Peer to Peer UDP Àü¼ÛÀÌ °¡´ÉÇÑÁö °Ë»çÇÑ´Ù.
#define MC_PEER_UDPTEST_REPLY		10006	///< Peer to Peer UDP Àü¼ÛÀÌ °¡´ÉÇÑÁö °Ë»çÇÑ´Ù.

#define MC_UDP_PING					10007	///< Client¿Í Agentµé »çÀÌÀÇ pingÀ» ÃøÁ¤ÇÏ±âÀ§ÇÑ command
#define MC_UDP_PONG					10008	///< ping¿¡ ´ëÇÑ ÀÀ´ä

#define MC_PEER_OPENED				10011	///< °³º° Peer Åë½Å °³Åë¾Ë¸²
#define MC_PEER_BASICINFO			10012	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â Ä³¸¯ÅÍ ±âº»Á¤º¸ ÅõÇ¥ÆÇÁ¤ÀÇ ±Ù°Å°¡ µÈ´Ù
#define MC_PEER_UPDATECHAR			10017	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â Ä³¸¯ÅÍ ±âº»Á¤º¸ ÅõÇ¥ÆÇÁ¤ÀÇ ±Ù°Å°¡ µÈ´Ù
#define MC_PEER_HPINFO				10013	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â Ä³¸¯ÅÍ Ã¼·ÂÁ¤º¸ ÅõÇ¥ ÆÇÁ¤ÀÇ ±Ù°Å°¡ µÈ´Ù
#define MC_PEER_HPAPINFO			10014	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â Ä³¸¯ÅÍ Ã¼·Â/¾Æ¸Ó Á¤º¸
#define MC_PEER_DUELTOURNAMENT_HPAPINFO		10016	///< Á¤±âÀûÀ¸·Î ¾÷µ¥ÀÌÆ®µÇ´Â µà¾óÅä³Ê¸ÕÆ® Àü¿ë Ä³¸¯ÅÍ Ã¼·Â/¾Æ¸Ó Á¤º¸

#define MC_PEER_CHANGECHARACTER		10015
#define MC_PEER_MOVE				10021   ///< ¿¡ ³ªÁß¿¡ MC_OBJECT_MOVE·Î ¹Ù²î¾î¾ßÇÒµí..
#define MC_PEER_CHANGE_WEAPON		10022   
#define MC_PEER_CHANGE_PARTS		10023
#define MC_PEER_ATTACK				10031
#define MC_PEER_DAMAGE				10032
#define MC_PEER_RELOAD				10033
#define MC_PEER_SHOT				10034
#define MC_PEER_SHOT_SP				10035
#define MC_PEER_SKILL				10036
#define MC_PEER_SHOT_MELEE			10037	///< ±ÙÁ¢°ø°Ý
#define MC_PEER_DIE					10041
#define MC_PEER_SPAWN				10042
#define MC_PEER_DASH				10045
#define MC_PEER_SPMOTION			10046
#define MC_PEER_CHAT				10052
#define MC_PEER_CHAT_ICON			10053	///< Ã¤ÆÃ ½ÃÀÛ/³¡. ¸Ó¸®¿¡ ¾ÆÀÌÄÜÀ» º¸¿©ÁÖ°Å³ª °¨Ãá´Ù.
#define MC_PEER_REACTION			10054	///< ÀÌÆåÆ®¸¦ À§ÇØ ¾Ë·ÁÁØ´Ù 
#define MC_PEER_ENCHANT_DAMAGE		10055	///< ÀÎÃ¦Æ® µ¥¹ÌÁö¸¦ ¸Â¾Ò´Ù
#define MC_PEER_BUFF_INFO			10056	///< ¹öÇÁ ÀÌÆåÆ®¸¦ Àü´Þ(¼ô¹öÇÁÀÇ °æ¿ì, ¹ßµ¿½Ã »ó´ëµµ ÅëÁö¸¦ ¹ÞÀ¸¹Ç·Î µû·Î º¸³»ÁÖÁö ¾Ê¾Æµµ µÈ´Ù)
											///< ±×·¯³ª ³­ÀÔÀÚ¿¡°Ô´Â ÀÚ½ÅÀÇ ¹ßµ¿ ÁßÀÎ ÀÌÆåÆ®¸¦ ¾Ë·ÁÁÖ¾î¾ß ÇÏ¹Ç·Î ±×¶§ »ç¿ëÇÑ´Ù)

#define MC_REQUEST_RESOURCE_CRC32				11001
#define MC_RESPONSE_RESOURCE_CRC32				12002


// Äù½ºÆ® ¾ÆÀÌÅÛ °ü·Ã ºÎºÐ.
#define MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST		21000	///< ³ªÀÇ Äù½ºÆ® ¾ÆÀÌÅÛ ¸ñ·ÏÀ» ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_CHAR_QUEST_ITEM_LIST		21001	///< ³ªÀÇ Äù½ºÆ® ¾ÆÀÌÅÛ ¸ñ·ÏÀ» Àü¼Û.(S -> C)
#define MC_MATCH_REQUEST_BUY_QUEST_ITEM				21002	///< Äù½ºÆ® ¾ÆÀÌÅÛ ±¸ÀÔ ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_BUY_QUEST_ITEM			21003	///< Äù½ºÆ® ¾ÆÀÌÅÛ ±¸ÀÔ ¿Ï·á.(S -> C)
#define MC_MATCH_REQUEST_SELL_QUEST_ITEM			21004	///< ³ªÀÇ Äù½ºÆ® ¾ÆÀÌÅÛ ÆÇ¸Å ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_SELL_QUEST_ITEM			21005	///< ³ªÀÇ Äù½ºÆ® ¾ÆÀÌÅÛ ÆÇ¸Å ¿Ï·á.(S -> C)
#define MC_MATCH_USER_REWARD_QUEST					21006	///< Äù½ºÆ®°¡ ¿Ï·áµÈÈÄ º¸»óµÈ Äù½ºÆ® ¾ÆÀÌÅÛ Á¤º¸.
#define MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM		21007	///< Èñ»ý ¾ÆÀÌÅÛÀ» ½½·Ô¿¡ ³õ±â ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM		21008	///< Èñ»ý ¾ÆÀÌÅÛÀ» ½½·Ô¿¡ ³õ±â ÀÀ´ã.(S -> C)
#define MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM	21009	///< ½½·Ô¿¡ ¿Ã·Á³õÀº Èñ»ý ¾ÆÀÌÅÛÀ» È¸¼ö ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_CALLBACK_SACRIFICE_ITEM	21010	///< ½º·Ô¿¡ ¿Ã·Á³õÀº Èñ»ý ¾ÆÀÌÅÛÀ» ÈÖ¼ö ÀÀ´ä.(S -> C)
#define MC_MATCH_REQUEST_SLOT_INFO					21011	///< Èñ»ý ¾ÆÀÌÅÛ ½½·Ô Á¤º¸¸¦ ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_SLOT_INFO					21012	///< Èñ»ý ¾ÆÀÌÅÛ ½½·Ô Á¤º¸¸¦ ¾Ë¸².(S -> C)
#define MC_MATCH_NEW_MONSTER_INFO					21013	///< ¸ó½ºÅÍ µµ°¨¿¡ Ãß°¡µÉ »õ·Î¿î ¸ó½ºÅÍ Á¤º¸.
#define MC_MATCH_REQUEST_MONSTER_BIBLE_INFO			21014	///< ¸ó½ºÅÍ µµ°¨ÀÇ ³»¿ë ¿äÃ».(C -> S)
#define MC_MATCH_RESPONSE_MONSTER_BIBLE_INFO		21015	///< ¸ó½ºÅÍ µµ°¨ÀÇ ³»¿ë ¾Ë¸².(S -> C)


/// Keeper¿Í Keeper Manager¿Í Åë½ÅÇÏ´Âµ¥ ÇÊ¿äÇÑ ¸í·É. 2005.06.03 Ä¿¸Çµå Á¤¸® ÇÊ¿ä.
#define MC_KEEPER_MANAGER_CONNECT				31001	///< keeper Manager¿¡¼­ keeper·Î Á¢¼Ó ¿äÃ».
#define MC_CHECK_KEEPER_MANAGER_PING			31002	///< Keeper¿Í Keeper Manager°¡ Á¤»óÀûÀ¸·Î ¿¬°áµÇ¾î ÀÖ´ÂÁö °Ë»ç.
#define MC_REQUEST_MATCHSERVER_STATUS			31004	///< MatchServerÀÇ Á¤º¸¸¦ ¿äÃ».(Keeper -> MatchServer)
#define MC_RESPONSE_MATCHSERVER_STATUS			31005	///< Keeper·Î MatchServerÀÇ Á¤º¸¸¦ Àü¼ÛÇØ ÁÜ.(MatchServer -> Keeper)
// 2005.06.01 ~
#define MC_RESPONSE_KEEPER_MANAGER_CONNECT		31006	///< Keeper managerÀÇ Á¢¼ÓÀÌ Á¤»óÀûÀ¸·Î ÀÌ·ç¾î ÁüÀ» ¾Ë¸².( Keeper -> Keeper Manager )
#define MC_REQUEST_CONNECTION_STATE				31007	///< ÇöÁ¦ ¼­¹öÀÇ »óÅÂ.(Keeper Manager -> Keeper)
#define MC_RESPONSE_CONNECTION_STATE			31008	///< ¼­¹ö»óÅÂ ÀÀ´ä.(Keeper -> Keeper Manager)
#define MC_REQUEST_SERVER_HEARBEAT				31009	///< ¼­¹ö°¡ µ¿ÀÛÇÏ°í ÀÖ´ÂÁö ÀÀ´ä ¿äÃ».(Keeper -> MatchServer)
#define MC_RESPONSE_SERVER_HEARHEAT				31010	///< ¼­¹ö°¡ µ¿ÀÛÇÏ°í ÀÖÀ»À» ÀÀ´ä.(MatchServer -> Keeper)
#define MC_REQUEST_KEEPER_CONNECT_MATCHSERVER	31011	///< Keeper°¡ MatchServer·Î Á¢¼ÓÇÏµµ·Ï ¿äÃ»ÇÔ(Keeper Manager -> Keeper -> MatchServer )
#define MC_RESPONSE_KEEPER_CONNECT_MATCHSERVER	31012	///< Keeper°¡ MatchServer¿¡ Á¢¼ÓÇß´Ù´Â ÀÀ´ä.(MatchServer -> Keeper -> Keeper Manager )
#define MC_REQUEST_REFRESH_SERVER				31013	///< Keeper ManagerÀÇ ¼­¹ö ¸®½ºÆ® ¼öµ¿ °»½ÅÀ» À§ÇØ¼­ Keeper·Î MatchServerÀÇ HeartbaetÃ¤Å© ¿äÃ».(Keeper Manager -> Keeepr)
#define MC_REQUEST_LAST_JOB_STATE				31014	///< ¸¶Áö¸· ½ÇÇàµÈ ÀÏÀÇ °á°ú¸¦ ¿äÃ».(Keeper -> Keeper Manager )
#define MC_RESPONSE_LAST_JOB_STATE				31015	///< ÀÛ¾÷ °á°ú ¿äÃ» ÀÀ´ä.(Keeper -> Keeper Manager)
#define MC_REQUEST_CONFIG_STATE					31016	///< Keeper¿¡ ÀúÀåµÈ ¼³Á¤ Á¤º¸ ¿äÃ».(Keeper Manager -> Keeper)
#define MC_RESPONSE_CONFIG_STATE				31017	///< Keeper¿¡ ÀúÀåµÈ ¼³Á¤ Á¤º¸ ¿äÃ» ÀÀ´ä.(Keeper -> Keeper Manager)
#define MC_REQUEST_SET_ONE_CONFIG				31018	///< ConfigÇÏ³ª ¼³Á¤ ¿äÃ».(Keeper Manager -> Keeper)
#define MC_RESPONSE_SET_ONE_CONFIG				31019	///< ConfigÇÏ³ª ¼³Á¤ ¿äÃ» ÀÀ´ä.(Keeper -> Keeper Manager)
#define MC_REQUEST_KEEPERMGR_ANNOUNCE			31020	///< Keeper Manager¿¡¼­ °¢ ¼­¹ö·Î °øÁö¸¦ º¸³»µµ·Ï Keeper¿¡ ¿äÃ»ÇÔ.(Keeper Manager -> Keeper)
#define MC_REQUEST_KEEPER_ANNOUNCE				31021	///< Keeper¿¡¼­ MatchServer°¡ °øÁö»çÇ×À» ¾Ë¸®µµ·Ï ¿äÃ»ÇÔ.(Keeper -> MatchServer)
#define MC_REQUEST_RESET_PATCH					31022	///< ÇöÁ¦+±îÁö ÁøÇàÇÑ ÆÐÄ¡ ÀÛ¾÷»óÅÂ¸¦ ÃÊ±âÈ­ÇÔ.
#define MC_REQUEST_DISCONNECT_SERVER			31023	///< Keeper¿Í MatchServer¿ÍÀÇ ¿¬°á Á¾·á.
#define MC_REQUEST_REBOOT_WINDOWS				31024	///< À©µµ¿ì ÀçºÎÆÃ.(Keeper Manager -> Keeper)
#define MC_REQUEST_ANNOUNCE_STOP_SERVER			31025	///< °øÁö»çÇ×°ú ÇÔ°Ô ¼­¹ö¸¦ Á¾·á ½ÃÅ´ ¿äÃ».
#define MC_RESPONSE_ANNOUNCE_STOP_SERVER		31026	///< °øÁö»çÇ×°ú ÇÔ°Ô ¼­¹ö¸¦ Á¾·á ½ÃÅ´ ¿äÃ» ÀÀ´ä.
#define MC_REQUEST_SERVER_AGENT_STATE			31027	///< ÇöÁ¦ ¼­¹ö¿Í ¿¡ÀÌÀüÆ®ÀÇ ½ÇÇà »óÅÂ ¿äÃ».
#define MC_RESPONSE_SERVER_AGENT_STATE			31028	///< ÇöÁ¦ ¼­¹ö¿Í ¿¡ÀÌÀüÆ®ÀÇ ½ÇÇà »óÅÂ ¿äÃ» ÀÀ´ä.
#define MC_REQUEST_WRITE_CLIENT_CRC				31029	///< Å¬¶óÀÌ¾ðÆ®ÀÇ CRC CheckSumÀ» ¼­¹ö iniÆÄÀÏ¿¡ ÀúÀå ¿äÃ».(KeeperManager -> Keeper)
#define MC_RESPONSE_WRITE_CLIENT_CRC			31030	///< Å¬¶óÀÌ¾ðÆ®ÀÇ CRC CheckSumÀ» ¼­¹ö iniÆÄÀÏ¿¡ ÀúÀå ¿äÃ» ÀÀ´ä.(Keeper -> Keeper Manager)
#define MC_REQUEST_KEEPER_RELOAD_SERVER_CONFIG	31031	///< Keeper¿Í ¿¬°áµÇ¾îÀÖ´Â ¼­¹ö°¡ ¼³Á¤ ÆÄÀÏÀ» ´Ù½Ã ·ÎµåÇÏµµ·Ï ¿äÃ»ÇÔ.(KeeperManager -> Keeper)
#define MC_REQUEST_RELOAD_CONFIG				31032	///< ¼­¹öÀÇ ¼³Á¤ ÆÄÀÏÀ» ´Ù½Ã ·Î´ÙÇÏµµ·Ï ¿äÃ».(Keeper -> server)
#define MC_REQUEST_KEEPER_ADD_HASHMAP			31033	
#define MC_RESPONSE_KEEPER_ADD_HASHMAP			31034
#define MC_REQUEST_ADD_HASHMAP					31035
#define MC_RESPONSE_ADD_HASHMAP					31036

// MatchServer ÆÐÄ¡ °ü·Ã.
#define MC_REQUEST_DOWNLOAD_SERVER_PATCH_FILE	32024	///< Keeper°¡ FTP¼­¹ö¿¡¼­ PatchÆÄÀÏÀ» ´Ù¿î·Îµå ½ÃÅ´.(Keeper Manager -> Keeper )
#define MC_REQUEST_START_SERVER					32026	///< MatchServer½ÇÇà ¿äÃ».(Keeper Manager -> Keeper)
#define MC_REQUEST_STOP_SERVER					32028	///< ¼­¹ö Á¤Áö ¿äÃ».(Keeper -> Keeper Manager)
#define MC_REQUEST_PREPARE_SERVER_PATCH			32030	///< ÆÐÄ¡ ÁØºñ ÀÛ¾÷.(¾ÐÃà Ç®±â, ÀÓ½Ã ÆÄÀÏ Á¦°Å.)
#define MC_REQUEST_SERVER_PATCH					32032	///< ÆÐÄ¡ ÆÄÀÏÀ» º¹»çÇÔ.(Keeper Manager -> Keeper )
// MatchAgent ÆÐÄ¡ °ü·Ã.
#define MC_REQUEST_STOP_AGENT_SERVER			33034	///< Agent serverÁ¤Áö ¿äÃ».(Keeper Manager -> Keeper )
#define MC_REQUEST_START_AGENT_SERVER			33036	///< Agent server½ÃÀÛ ¿äÃ».(Keeper Manager -> Keeper)
#define MC_REQUEST_DOWNLOAD_AGENT_PATCH_FILE	33038	///< request agent patch file download.(Keeper Manager -> Keeper)
#define MC_REQUEST_PREPARE_AGENT_PATCH			33040	///< reqeust prepare agent patch.(Keeper Manager -> Keeper)
#define MC_REQUEST_AGENT_PATCH					33042	///< request agent patch.(Keeper Manager -> Keeper)
#define MC_REQUEST_SERVER_STATUS				33043	///< server¿Í agentÀÇ Á¤º¸¸¦ Keeper·Î ¿äÃ»ÇÔ.(Keeper Manager -> keeper)
#define MC_RESPONSE_SERVER_STATUS				33044	///< server¿Í agetnÀÇ Á¤º¸ ¿äÃ» ÀÀ´ä.(Keeper -> Keeper Manager)
// Keeper manager schedule.
#define MC_REQUEST_KEEPER_MANAGER_SCHEDULE		34001	///< request keeper manager schedule.(Keeper Manager -> Keeper -> MatchServer)
#define MC_RESPONSE_KEEPER_MANAGER_SCHEDULE		34002	///< Keeper ManagerÀÇ ½ºÄÉÁìµî·Ï ¿äÃ» ÀÀ´ä.
#define MC_REQUEST_START_SERVER_SCHEDULE		34003	///< ´Ù½Ã½ÃÀÛ ½ºÄÉÁÙ¿¡¼­ ¼­¹ö¸¦ ½ÃÀÛÇÏ´Â ½ÃÄÉÁÙ. ½ÃÀÛÇÒ¶§±îÁö ½ºÄÉÁÙÀÌ Á¦°ÅµÇÁö ¾ÊÀ½.
///


/// Locator°ü·Ã.
#define MC_REQUEST_SERVER_LIST_INFO				40001	/// Á¢¼Ó°¡´ÉÇÑ ¼­¹öÀÇ ¸®½ºÆ® Á¤º¸ ¿äÃ».(Client -> Locator)
#define MC_RESPONSE_SERVER_LIST_INFO			40002	/// Á¢¼Ó°¡´ÉÇÑ ¼­¹ö ¸®½ºÆ® Á¤º¸ ÀÀ´ä.(Locator -> Client)
#define MC_RESPONSE_BLOCK_COUNTRY_CODE_IP		40003	/// Á¢¼ÓºÒ°¡ ±¹°¡ ÄÚµåÀÇ IPÀÀ´ä.(Locator -> Client)


// filter.
#define MC_RESPONSE_BLOCK_COUNTRYCODE			50001	/// ºí·°µÈ ±¹°¡ÄÚµåÀÇ IP°¡ Á¢¼Ó½Ã Åëº¸¿ë.
#define MC_LOCAL_UPDATE_USE_COUNTRY_FILTER		50002
#define MC_LOCAL_GET_DB_IP_TO_COUNTRY			50003
#define MC_LOCAL_GET_DB_BLOCK_COUNTRY_CODE		50004
#define MC_LOCAL_GET_DB_CUSTOM_IP				50005
#define MC_LOCAL_UPDATE_IP_TO_COUNTRY			50006
#define MC_LOCAL_UPDATE_BLOCK_COUTRYCODE		50007
#define MC_LOCAL_UPDATE_CUSTOM_IP				50008
#define MC_LOCAL_UPDATE_ACCEPT_INVALID_IP		50009

#define MC_MATCH_FLAG_EFFECT	50010	///< ¹öÇÁ ¼Ò¸ð¼º ¾ÆÀÌÅÛ »ç¿ë ¿äÃ»(C -> S)
#define MC_MATCH_FLAG_CAP	50011	///< ¹öÇÁ ¼Ò¸ð¼º ¾ÆÀÌÅÛ »ç¿ë ¿äÃ»(C -> S)
#define MC_MATCH_REQUEST_FLAG_CAP	50012	///( S -> C)
#define MC_MATCH_FLAG_STATE	50013	///( S -> C)
#define MC_MATCH_REPORT                         50019
//Custom: Packets
#define MC_MATCH_GUNGAME_SEND_NEW_WEAPON	50020
#define MC_MATCH_GUNGAME_SET				50021
#define MC_MATCH_INFECT_PLAYER				50022

#define MC_MATCH_INFECTION_LATEJOIN			50023
//Spymode
#define MC_MATCH_SPY_INFO					50024
#define MC_MATCH_SPY_TEAMS					50025
#define MC_MATCH_STAGE_START_SPY			50026
#define MC_MATCH_SPY_BONUS					50027
#define MC_MATCH_SPY_ROUNDREWARD			50028
//todo: spymode custom reward for killing player
#define MC_BLITZ_START_HONOR				50029
#define MC_BLITZ_PLAYER_KILL				50030
#define MC_BLITZ_REQUEST_UPGRADE			50031
#define MC_BLITZ_RESPONSE_UPGRADE			50032
#define MC_BLITZ_REQUEST_CLASS				50033
#define MC_BLITZ_RESPONSE_CLASS				50034
//gap for blitz???
#define MC_MATCH_LATEJOIN_CQ				50040
#define MC_MATCH_LATEJOIN_QUEST				50041

//Gap for quest stuff
#define MC_SKILLMAP_OBTAIN_ITEM				50042

//Antilead
#define MC_GAME_ANTILEAD					50043

//Demolition
#define MC_MATCH_REQUEST_PLANT_BOMB			50044
#define MC_MATCH_RESPONSE_PLANT_BOMB		50045
#define MC_MATCH_BOMB_EXPLODE				50046

//Skillmap
#define MC_MATCH_UPDATE_SKILLMAP_BESTTIME	50048
#define MC_MATCH_REQUEST_SKILLMAP_BESTTIME	50049
#define MC_MATCH_RESPONSE_SKILLMAP_BESTTIME	50050

//Achievements
#define MC_MATCH_ACHIEVEMENT_COMPLETED		50051

//Custom: Create account
#define MC_MATCH_REQUEST_CREATE_ACCOUNT		50052
#define MC_MATCH_RESPONSE_CREATE_ACCOUNT	50053
#endif
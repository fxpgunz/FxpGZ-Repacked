
#pragma once
#if defined(_DEBUG) || defined(_RELEASE) || defined(LOCALE_KOREA) || defined(LOCALE_NHNUSA) || defined(LOCALE_JAPAN) || defined(LOCALE_BRAZIL)
#endif

#define _AVATAR_ENABLE
#define _MIPING		1
#define _SRVRPNG	1
#define _SYSTEMUPDATETIME (3 * 60 * 1000)
#define _SUICIDE_QUICK 1
#define _GUNGAME 1

//Auto Event Room
//#define _ROOMEVENT
//#define TIMEAUTOEVENT1	(118 * 60 * 1000)
//#define TIMEAUTOEVENT2	(140 * 60 * 1000)


//RandomNotice
#define _RANDOMNOTICE 1
#define _RANDOMNOTICETIME (15 * 60 * 1000) // = Minutos * segundos * milisegundos
#define _RANDOMNOTICEMSG1 "Now you can receive items while playing Clan War."
#define _RANDOMNOTICEMSG2 "Ahora puedes recibir items mientras juegas al clan war."
#define _RANDOMNOTICEMSG3 "Enjoy the best types of games here at GUNZ!"
#define _RANDOMNOTICEMSG4 "Use the ^5/commands^2 command to see the list."
#define _RANDOMNOTICEMSG5 "Disfruta de los mejores tipos de juegos aquí en GUNZ!"
#define _RANDOMNOTICEMSG6 "Merry XMAS and New Year!"
#define _RANDOMNOTICEMSG7 "Batalha de torneios muito em breve, prepare-se agora!"
#define _RANDOMNOTICEMSG8 "Repack By Jorklenis2"
#define _RANDOMNOTICEMSG9 "¿Eres nuevo? No te preocupes compra una Blue Box y canjealo ya!"
#define _RANDOMNOTICEMSG10 "Daily events performed by the staff."
#define _RANDOMNOTICEMSG11 "Feliz navidad y año nuevo por parte de GunZ the duel."

#define _2PCLAN		
#define _CW_VOTE 
#define _LADDERGAME

//#define _MULTILANGUAGE 1
#define _QUEST 1
#define _QUEST_ITEM 1
#define _MONSTER_BIBLE 1
#define _DUELTOURNAMENT 1
#define _SELL_CASHITEM
#define _QUESTCLAN 1
#define SUPPORT_EXCEPTIONHANDLING

//////////////Test////////////
#define _FIXERRORDIRECTX
#define _AGENT_IP "127.0.0.1"
#define _STAFFCHAT
#define _FONTSIZE
#define _MAX_SPENDABLE
#define _IGNORECOMMD
#define _MSGDIDCLANWAR
#define _CLOSECHANNEL 1 
#define _EVENTCHANNEL 1
#define _FORCE_EVENT_TEAM 1
//#define _DESHACKER
//#define _RELOADCONFIG
//#define _RELOADCONFIGTIME (2 * 60 * 1000)
#define _PAUSE
#define _QUESTRESPAWNJOIN
#define _KILLSTREAK
//#define _AFKSYSTEM (9 * 60 * 1000) // = 10 min
//#define _NOCLAN
#define _LIMITMAP
#define _CLOSEWARSERVER
#define _COUNTRYFLAG
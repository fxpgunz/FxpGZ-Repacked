@echo off
rem publish.bat [���] [�������]

set TARGETLANG=KOR
if "%1" == "" goto :no_target_lang
set TARGETLANG=%1
:no_target_lang

del vssver.scc /s /q /f

mkdir \\patch\gunzteam\Build\Server\lastest
set TARGET=\\patch\gunzteam\Build\Server\lastest\MatchAgent\


del %TARGET%*.* /s /q /f
mkdir %TARGET%

copy MatchAgent.exe %TARGET%
copy MatchAgent.pdb %TARGET%
copy dbghelp.dll %TARGET%
copy AgentConfig.xml %TARGET%_AgentConfig.xml

rem �ͽ��÷η� â�� ����.
explorer \\patch\gunzteam\Build\Server\lastest\

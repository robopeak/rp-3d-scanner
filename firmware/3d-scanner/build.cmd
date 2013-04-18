@ECHO OFF
set REDIRECT_PATH=%cd%
set SH_CMD=make
cd ..\..
launchsh.cmd

cd %REDIRECT_PATH%

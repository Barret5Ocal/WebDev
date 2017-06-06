@echo off 

mkdir ..\..\..\builds\backend

pushd ..\..\..\builds\backend

cl -nologo -Z7 ..\..\project\backend\code2\showip.c wsock32.lib Ws2_32.lib

popd 

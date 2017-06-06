@echo off 

mkdir ..\..\..\builds\backend

pushd ..\..\..\builds\backend

cl -nologo -Z7 ..\..\project\backend\code3\listener.c wsock32.lib Ws2_32.lib
cl -nologo -Z7 ..\..\project\backend\code3\talker.c wsock32.lib Ws2_32.lib

popd 

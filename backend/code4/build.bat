@echo off

mkdir ..\..\..\builds\backend

pushd ..\..\..\builds\backend

cl -nologo -Z7 ..\..\project\backend\code4\talker.cpp wsock32.lib
cl -nologo -Z7 ..\..\project\backend\code4\listener.cpp wsock32.lib

popd
@echo off

mkdir ..\..\..\builds\backend

pushd ..\..\..\builds\backend

cl -nologo -Z7 ..\..\project\backend\code\main.cpp wsock32.lib

popd
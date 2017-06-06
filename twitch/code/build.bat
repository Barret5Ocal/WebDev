@echo off

mkdir ..\..\..\builds\twitch

pushd ..\..\..\builds\twitch

cl -nologo -Z7 /EHsc ..\..\project\twitch\code\twitch.cpp wsock32.lib Ws2_32.lib Kernel32.lib

popd
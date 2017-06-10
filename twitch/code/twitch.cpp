/*
WHAT DO I DO NEXT?
- learn how to use the rest of the API
- figure out how the twitch bot can have a name from yours 
*/

/*
   alt+t = TODO
   alt+y = NOTE
   alt+r = block 
   crt+a = replace in range
   alt+E = shutdown 4coder
*/

#include <stdio.h>

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#endif

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

char *Host = "irc.twitch.tv";
int Port = 6667;
char *PortStr = "6667";
char *Nick = "barret5ocal_tog_dev";
char *BotName = "b50Bot";
char *Pass = "oauth:0qo03hcn20kfo2wkjlrgw0f6hxzdtx";
char *Chan = "barret5ocal_tog_dev";
float Rate = 20.0f/30.0f;
char *BannedWords[] = {"asswipe", "fuckernutter"};

int SocketHandle; 

// TODO(Barret5Ocal): start recording state
// What do I need to record??
struct bot_state
{
    
};

#include "b50_string.h"

#define STB_SPRINTF_IMPLEMENTATION 
#include "stb_sprintf.h"

bool InitializeSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2),
                      &WsaData ) == NO_ERROR;
#else
    return true;
#endif
}

void ShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}


bool CreateSocketTCP()
{
    addrinfo Hints = {};
    
    Hints.ai_family = AF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;
    
    addrinfo *Result; 
    int Res = getaddrinfo(Host, PortStr, &Hints, &Result);
    if(Res != 0)
    {
        printf("getaddrinfo failed with error: %d\n", Res);
        return 1; 
    }
    
    addrinfo *Index;
    for(Index = Result; Index != 0; Index = Index->ai_next)
    {
        SocketHandle = socket(Index->ai_family, Index->ai_socktype, Index->ai_protocol);
        if(SocketHandle == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
        }
        
        
        Res = connect(SocketHandle, Index->ai_addr, (int)Index->ai_addrlen);
        if(Res == SOCKET_ERROR)
        {
            closesocket(SocketHandle);
            SocketHandle = INVALID_SOCKET;
            continue; 
        }
        break; 
    }
    
    freeaddrinfo(Result);
    
    return 0;
}

bool SendPacketTCP(char *Data, int Len)
{
    int Res = send(SocketHandle, Data, Len, 0);
    if(Res == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        return 1; 
    }
    
    printf("Bytes Sent: %ld\n", Res);
    
    return 0; 
}

bool TwitchMessage(char *MSG, char *Channel)
{
    char MsgBuffer[50] = {};
    int Yes = stbsp_sprintf((char *)MsgBuffer, "PRIVMSG %s :%s\r\n", MSG, Channel);
    return SendPacketTCP(MsgBuffer, sizeof(MsgBuffer));
}

bool TCPSendShutDown()
{
    int Res = shutdown(SocketHandle, SD_SEND);
    if(Res == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    return 0; 
}

void ReFormatResponce(char *Data)
{
    // NOTE(Barret5Ocal): I am receiving the packets correctly
    // its just that each line of the response is null terminated
    while(*Data)
    {
        ++Data; 
        if(*Data == 0)
        {
            *Data = '\n';
            ++Data; 
            if(*Data == 0)
                break; 
        }
    }
}

void Wait()
{
    float MessagesPerSecond = 20.0f / 30.0f; 
    float ConvertToOneSecond = 1.0f / MessagesPerSecond; 
    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    sleep(ConvertToOneSecond); 
#elif PLATFORM == PLATFORM_WINDOWS
    Sleep(ConvertToOneSecond * 1000);
#endif 
}

#include "regex.cpp"

void ReceivePacketsTCP(bot_state *Bot)
{
    unsigned char Buffer[1024] = {};
    int BufferSize = sizeof(Buffer);
    int Res;
    do 
    {
        
        memset(Buffer, 0, sizeof(Buffer));
        Res = recv(SocketHandle, (char*)Buffer, BufferSize, 0);
        
        if ( Res > 0 )
        {
            printf("Bytes received: %d\n", Res);
            ReFormatResponce((char*)Buffer);
            
            response Response = ParseResponse((char*)Buffer);
            
            printf((char *)Buffer);
            
            if(!Q_strcmp((unsigned char *)"PING :tmi.twitch.tv\r\n\n", (unsigned char *)Buffer))
            {
                char *Pong = "PONG :tmi.twitch.tv\r\n";
                SendPacketTCP(Pong, Q_strlen(Pong));
            }
            else
            {
                
                if(Response.Type == MSG_REC)
                {
                    if(!Q_strcmp((unsigned char *)Nick, (unsigned char *)Response.User.String))
                    {
                        if(!Q_strcmp((unsigned char *)"!quit", (unsigned char *)Response.Message.String))
                        {
                            // NOTE(Barret5Ocal): Always make sure the buffer is big enough. the message won't send correctly if it is not
                            char QuitMsg[50] = {};
                            int Yes = stbsp_sprintf((char *)QuitMsg, "PRIVMSG #%s :I am quiting\r\n", Chan);
                            SendPacketTCP(QuitMsg, sizeof(QuitMsg));
                            break; 
                        }
                        if(!Q_strcmp((unsigned char *)"!membership", (unsigned char *)Response.Message.String))
                        {
                            SendPacketTCP("CAP REQ :twitch.tv/membership", Q_strlen("CAP REQ :twitch.tv/membership"));
                        }
                        if(!Q_strcmp((unsigned char *)"!mod", (unsigned char *)Response.Message.String))
                        {
                            char ModeMsg[100] = {};
                            char *NewMode = "barret5ocal_TOG";
                            int Yes = stbsp_sprintf(ModeMsg, "MODE %s +o %s", NewMode, Chan);
                            SendPacketTCP(ModeMsg, sizeof(ModeMsg));
                        }
                        if(!Q_strcmp((unsigned char *)"!message", (unsigned char *)Response.Message.String))
                        {
                            char Msg[50] = {};
                            int Yes = stbsp_sprintf((char *)Msg, "PRIVMSG #%s :quick message\r\n", Chan);
                            SendPacketTCP(Msg, sizeof(Msg));
                            
                        }
                    }
                    else 
                    {
                        // NOTE(Barret5Ocal): Search for banned words
                        for(int Index = 0;
                            Index < ArrayCount(BannedWords);
                            ++Index)
                        {
                            if(strstr((char *const)Response.Message.String, (const char *const)BannedWords[Index]))
                            {
                                // TODO(Barret5Ocal): Ban here 
                            }
                            
                        }
                    }
                }
                
                Wait();
                
            }
            DestroyResponse(&Response);
        }
        else if ( Res == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
        
    } while( Res > 0); 
    
    // NOTE(Barret5Ocal): I don't think TCP connections can be nonBlocking. Need to look into this more 
}

void SocketClose()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close( SocketHandle );
#elif PLATFORM == PLATFORM_WINDOWS
    closesocket( SocketHandle );
#endif
    
}

// NOTE(Barret5Ocal): TCP is needed to interact with twitch
int main(int Argc, char *Argv[])
{
    
    bot_state Bot = {};
    
    InitializeSockets();
    CreateSocketTCP();
    
    char DataPass[50] = {};
    char DataUser[50] = {};
    char DataNick[50] = {};
    char DataJoin[50] = {};
    
    stbsp_sprintf(DataPass, "PASS %s\r\n", Pass);
    stbsp_sprintf(DataUser, "USER %s\r\n", Nick);
    stbsp_sprintf(DataNick, "NICK %s\r\n", Nick);
    stbsp_sprintf(DataJoin, "JOIN #%s\r\n", Chan);
    
    SendPacketTCP(DataPass, sizeof(DataPass));
    SendPacketTCP(DataUser, sizeof(DataUser));
    SendPacketTCP(DataNick, sizeof(DataNick));
    SendPacketTCP(DataJoin, sizeof(DataJoin));
    
    Wait();
    
    
    ReceivePacketsTCP(&Bot);
    
    SocketClose();
    ShutdownSockets();
    
    return 1; 
}

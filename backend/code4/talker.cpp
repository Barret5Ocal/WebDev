/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include <stdio.h>

#include "Socket.cpp"

int Port = 30001;
int SendToPort = 30000;

char ProtocalID[] = "B50C";
char Data[] = "hello world!";

win_socket Sock;

address Server = {};

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

void CreateSocket()
{
    if ( !SocketOpen( &Sock, Port ) )
    {
        printf( "failed to open socket!\n" );
    }
}

void SendPacket(const char data[], const int len)
{
    SocketSend( &Sock, &Server, (void *)data, len );
}

void ReceivePackets()
{
    while ( true )
    {
        address sender = {};
        unsigned char buffer[256];
        int bytes_read =
            SocketReceive( &Sock, &sender,
                            buffer,
                            sizeof( buffer ) );

       
/*
  NOTE(barret): this code causes the program to break
        char Check = (char)getchar();
        if(Check == '0')
            break;
*/
        if ( bytes_read <= 0)
            continue;

        // process packet
        char* packet_data = (char*)buffer;        
        printf("%d: %s\n", bytes_read, packet_data);
        break; 
    }
}

int main(int argc, char** argv)
{
    
    InitializeSockets();
    CreateSocket();

    Server = InitAddress(127,0,0,1,SendToPort);
    while (true)
    {
        SendPacket(Data, sizeof(Data));
        ReceivePackets();
    }

    ShutdownSockets();
    return 0;
}

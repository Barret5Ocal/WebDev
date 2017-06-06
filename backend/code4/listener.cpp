/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include <stdio.h>

#include "Socket.cpp"

int Port = 30000;

char ProtocalID[] = "B50C";
char Data[] = "B50C hello world!";

win_socket Sock;

address Connection = {};

bool CheckForProtocalID(char *Data)
{
    char ID[5] = {};
    memcpy(ID, Data, 4);
    if(!strcmp(ProtocalID, ID))
    {
        return true; 
    }
    return false; 
}

bool AddressNonNull(address S1)
{
    if(S1.ip != 0)
    {
        if(S1.port != 0)
            return true;
    }
    return false; 
}

bool AddressCompare(address S1, address S2)
{
    if(S1.ip == S2.ip)
    {
        if(S1.port == S2.port)
            return true;
    }
    return false; 
}
void PrintMessage(char *MSG)
{
    char Output[sizeof(MSG) - 5];

    MSG += 5;
    
    int Index = 0;
    while (MSG)
        Output[Index++] = *MSG++;

    printf(Output);
}


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
    SocketSend( &Sock, &Connection, (void *)data, len );
}


void ReceivePackets()
{
    printf("Now recieving packets\n");
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
        if(AddressCompare(Connection, sender))
        {
            PrintMessage(packet_data);
            //NOTE(barret) send reponse back
            char *Response = "Holding Connection"; 
            SendPacket(Response, sizeof(Response));
        }
        else if(CheckForProtocalID(packet_data))
        {
            Connection = sender;
        }
    }
}
    
int main(int argc, char** argv)
{

    InitializeSockets();
    CreateSocket();

    //printf("sendToPort: %d\n", sendToPort);    
    ReceivePackets();
    
    ShutdownSockets();
    return 0;
}

/*
  BACKEND PRACTICE
  - next up
        - virtual connection
                - make one program instance a server and the other a client
                - server waits for a responces
                - client sends message to server
                - server records clients address
                - server sents responces to client
                - client will already have the servers address
                - the server and the client will filter out all messages that aren't from the connection
 */
#include <stdio.h>

#include "Socket.cpp"

int port = 30000;
int sendToPort = 30001;
bool sending = false;
const char data[] = "hello world!";

win_socket sock;


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
    if ( !SocketOpen( &sock, port ) )
    {
        printf( "failed to open socket!\n" );
    }
}

void SendPacket(const char data[], const int len)
{
    SocketSend( &sock, &InitAddress(127,0,0,1,sendToPort), (void *)data, len );
}

void ReceivePackets()
{
    while ( true )
    {
        address sender = {};
        unsigned char buffer[256];
        int bytes_read =
            SocketReceive( &sock, &sender,
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

bool ParseCmdLine(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        char *argStr = argv[i];
        if (!strcmp(argStr,"--port") || !strcmp(argStr,"-p"))
        {
            char *portStr = argv[i + 1];
            port = atoi(portStr);
        }
        else if (!strcmp(argStr, "--sendto") || !strcmp(argStr, "-st"))
        {
            char *portStr = argv[i + 1];
            sendToPort = atoi(portStr);
        }
        else if (!strcmp(argStr, "--send") || !strcmp(argStr, "-s"))
        {
            sending = true;
        }
        
    }
    return true;
}

int main(int argc, char** argv)
{
    if (!ParseCmdLine(argc, argv))
    {
        return 1;
    }

    InitializeSockets();
    CreateSocket();

    //printf("sendToPort: %d\n", sendToPort);    
    
    if (sending)
    {
        SendPacket(data, sizeof(data));
        printf( "Sent data: %s\n", data );
    }
    else
    {
        ReceivePackets();
    }
    
    ShutdownSockets();
    return 0;
}

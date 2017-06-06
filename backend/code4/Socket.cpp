
#include "Socket.h"

bool SocketOpen(win_socket *Socket, unsigned short port)
{
      Socket->handle = socket( AF_INET,
                     SOCK_DGRAM,
                     IPPROTO_UDP );

    if ( Socket->handle <= 0 )
    {
        printf( "failed to create socket\n" );
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( (unsigned short) port );

    if ( bind( Socket->handle,
               (const sockaddr*) &address,
               sizeof(sockaddr_in) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        return false;
    }
    printf( "Listening on port %d\n", port );


    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

    int nonBlocking = 1;
    if ( fcntl( Socket->handle,
                F_SETFL,
                O_NONBLOCK,
                nonBlocking ) == -1 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }

    #elif PLATFORM == PLATFORM_WINDOWS

    DWORD nonBlocking = 1;
    if ( ioctlsocket( Socket->handle,
                      FIONBIO,
                      &nonBlocking ) != 0 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }

    #endif

    return true;
}


void SocketClose(win_socket *Socket)
{
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close( Socket->handle );
    #elif PLATFORM == PLATFORM_WINDOWS
    closesocket( Socket->handle );
    #endif
    
}

bool SocketSend(win_socket *Socket,
                address *Destination,
                void *PacketData,
                int PacketSize)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( Destination->ip );
    addr.sin_port = htons( Destination->port );

    int sent_bytes =
        sendto( Socket->handle,
                (const char*)PacketData,
                PacketSize,
                0,
                (sockaddr*)&addr,
                sizeof(sockaddr_in) );

    if ( sent_bytes != PacketSize )
    {
        printf( "failed to send packet\n" );
        return false;
    }

    return true;    
}

int SocketReceive(win_socket *Socket, 
                  address *Sender,
                  void *Data,
                  int Size)
{
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif

    sockaddr_in from;
    socklen_t fromLength = sizeof( from );

    int bytes = recvfrom( Socket->handle,
                          (char*)Data,
                          Size,
                          0,
                          (sockaddr*)&from,
                          &fromLength );

    unsigned int from_address =
        ntohl( from.sin_addr.s_addr );

    unsigned int from_port =
        ntohs( from.sin_port );

    Sender->ip = from_address;
    Sender->port = ((unsigned short)from_port);

    return bytes;
    
}

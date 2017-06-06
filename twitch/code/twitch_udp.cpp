
bool CreateSocket()
{
    SocketHandle = socket(AF_INET, 
                          SOCK_DGRAM,
                          IPPROTO_UDP);
    
    if(SocketHandle <= 0)
    {
        printf("failed to create socket\n");
        return false;
    }
    
    sockaddr_in address; 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short) Port);
    
    if(bind(SocketHandle, (const sockaddr *)&address,
            sizeof(sockaddr_in)) < 0)
    {
        printf("failed to bind socket\n");
        return false;
    }
    printf("Listening on port %d\n", Port);
    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    int nonBlocking = 1; 
    if (fcntl(SocketHandle, F_SETFL,
              O_NONBLOCK, nonBlocking) == 1)
    {
        printf("failed to set non-blocking\n");
        return false;
    }
    
#elif PLATFORM == PLATFORM_WINDOWS
    
    DWORD nonBlocking = 1; 
    if(ioctlsocket(SocketHandle, FIONBIO, 
                   &nonBlocking) != 0)
    {
        printf("failed to set non-blocking\n");
        return false;
    }
    
#endif 
    
    return true; 
}

bool SendPacket(int address, char *Data, int Len)
{
    sockaddr_in addr; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = htonl(address);
    addr.sin_port = htons((unsigned short) Port);
    
    int SentBytes = sendto(SocketHandle,
                           Data, 
                           Len, 
                           0, 
                           (sockaddr *) &addr,
                           sizeof(sockaddr_in));
    
    if(SentBytes != Len)
    {
        printf("failed to send packet\n");
        return false;
    }
    
    return true; 
}

int  Receive(unsigned int *ip, unsigned short *port, unsigned char Buffer[], int BufferSize)
{
    
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif
    sockaddr_in From;
    socklen_t FromLength = sizeof(From);
    
    int BytesRead = recvfrom(SocketHandle, 
                             (char *)Buffer, 
                             BufferSize, 0,
                             (sockaddr *)&From, 
                             &FromLength);
    
    unsigned int FromAddress = 
        ntohl(From.sin_addr.s_addr);
    
    unsigned int FromPort = 
        ntohs(From.sin_port);
    
    *ip = FromAddress; 
    *port = ((unsigned short)FromPort);
    
    return BytesRead; 
}


void SendMessage()
{
    // TODO(Barret5Ocal): Why do messages send correctly but i never get a response from twitch. Figure this out! Possible reasons: sending messages with own user name, maybe I don't get a response if the bot post a message(i just tested about what happens when i post a message in the dashboard, the bot gets a reponds in the correct format),
    
    char Data[100] = {};
    char *Message = "This is a sample message";
    stbsp_sprintf(Data, "PRIVMSG %s :%s\r\n", Chan, Message);
    SendPacketTCP(Data, sizeof(Data));
    
}

void ReceivePackets(int address)
{
    while (true)
    {
        unsigned int ip;
        unsigned short port; 
        
        unsigned char Buffer[256];
        int BufferSize = sizeof(Buffer);
        
        int BytesRead = Receive(&ip, &port, Buffer, BufferSize);
        
        if(BytesRead <= 0)
            continue;
        
        char *PacketData = (char *)Buffer;
        printf("%d: %s\n", BytesRead, PacketData);
        if(!strcmp(PacketData, "PING :tmi.twitch.tv\r\n"))
        {
            char *Pong = "PONG :tmi.twitch.tv\r\n"; 
            SendPacket(address, Pong, sizeof(Pong));
        }
        break; 
    }
}



int ConvertStringToIP(char *ip)
{
    char *S[4] = {};
    
    unsigned char a = 127;
    unsigned char b = 0;
    unsigned char c = 0;
    unsigned char d = 1;
    
    int SIndex = 0;
    S[SIndex] = ip;
    
    while (*ip)
    {
        if(*ip == 46)
        {
            *ip = 0; 
            ++SIndex;
            ++ip;
            S[SIndex] = ip; 
        }
        ++ip;
    }
    
    a = atoi(S[0]);
    b = atoi(S[1]);
    c = atoi(S[2]);
    d = atoi(S[3]);
    
    unsigned int address =  ( a << 24 ) |
        ( b << 16 ) |
        ( c << 8  ) |
        d;
    
    return address; 
}

int GetHostIP(char *ip)
{
    hostent *HostIP = gethostbyname(Host);
    
    in_addr **addr_list = (in_addr **)HostIP->h_addr_list; 
    
    for(int i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
    
    return 1; 
}

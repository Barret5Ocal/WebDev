#include "Address.h"

address InitAddress( unsigned char a,
                     unsigned char b,
                     unsigned char c,
                     unsigned char d,
                     unsigned short port)
{
    unsigned int ip = ( a << 24 ) |
        ( b << 16 ) |
        ( c << 8  ) |
        d;

    address Address = {ip, port};
    return(Address);
}

address InitAddress( unsigned int ip,
                     unsigned short port)
{
    
    address Address = {ip, port};
    return(Address);
}

// unsigned char GetA() const;
// unsigned char GetB() const;
// unsigned char GetC() const;
// unsigned char GetD() const;

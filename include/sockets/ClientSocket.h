// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "Socket.h"

namespace sockets
{

class ClientSocket : private Socket
{
public:

    ClientSocket(std::string host, int port);
    virtual ~ClientSocket(){};

    const ClientSocket& operator << (const std::string&) const;
    const ClientSocket& operator >> (std::string&) const;
};

}
#endif

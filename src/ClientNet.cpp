#include "ClientNet.h"

ClientNet::ClientNet()
{
    //this->ws_ = ws_;
}

void ClientNet::initialise(int isLittleEndian)
{
    this->isLittleEndian = isLittleEndian;
    framesWithoutContact = 0;
}


#include "ClientNet.h"

ClientNet::ClientNet()
{
}

void ClientNet::initialise(int isLittleEndian)
{
    this->isLittleEndian = isLittleEndian;
    framesWithoutContact = 0;
}


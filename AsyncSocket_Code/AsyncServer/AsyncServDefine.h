#pragma once
#include <WinSock2.h>

struct StClientSock
{
    SOCKET sClient;
    CString strIPAddr;
};

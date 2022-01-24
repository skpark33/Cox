/*! \file ciUtil.C
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief string 을 delimiters 로 Tokenning 한다.
 *  (Environment: OSF1 5.1A)
 *
 *  \author     jhchoi
 *  \version
 *  \date       2003년 4월 22일 18:00
 */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#include "ciUtil.h"

#ifndef _COP_MSC_
 //   #include <arpa/inet.h>
 //   #include <unistd.h>
 //   #include <netdb.h>
 //   #include <stdlib.h>
#endif //_COP_MSC_

ciUtil::ciUtil() {
}

ciUtil::~ciUtil() {
}

ciUInt
ciUtil::getThreadId(void) {

#ifdef _COP_MSC_
    return ::GetCurrentThreadId();
#else
#if _COP_TRU64_
    return pthread_getsequence_np(pthread_self());
#else
    return 0;
#endif
#endif
}


ciString
ciUtil::getIpAddress() {
    // get host ip address

    ciString strAddr = "";
    char ac[80];

    if (gethostname(ac, sizeof(ac)) == -1) {
        //ciERROR(("gethostname error"));
        return strAddr;
    }

    struct hostent* phe = gethostbyname(ac);
    if ( phe == 0 ) {
        //ciERROR(("gethostbyname error"));
        return strAddr;
    }

    if ( phe->h_addr_list[0] != 0 ) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
        strAddr = inet_ntoa(addr);
    }
    //ciDEBUG(9, ("IPADDRESS[%s]", strAddr.c_str()));

    return strAddr;
}

ciInt
ciUtil::system(const char* cmd)
{
#ifdef _COP_MSC_
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    if (CreateProcessA(NULL, (char*)cmd, NULL, NULL, FALSE,
                NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    } else {
        return -1;
    }
#else // UNIX
    return system(cmd);
#endif //_COP_MSC_
}


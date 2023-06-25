/*****************************************************************//**
 * \file   network.cpp
 * \brief  Network Utility Source
 * \details only support for windows10
 * \author sdk
 * \date   June 2023
 *********************************************************************/

#if _WIN32

// https://learn.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmpsendecho
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <icmpapi.h>
#include <wininet.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wininet.lib")
#endif


#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <ctime>
#include "network.hpp"
#include "time.hpp"


namespace network {

bool Init() {
    static WSADATA  g_wsa_data;
    static bool     init_done = false;
    if (init_done) // Already Initialized
        return true;

    auto result = WSAStartup(MAKEWORD(2, 2), &g_wsa_data);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return false;
    }
    return true;
}

std::vector<std::string> GetIpAddrByHostname(const std::string& hostname, bool& on_error) {
    std::cout << "Get IP Address by Hostname: " << hostname << std::endl;

    struct hostent* p_remote_host;
    struct in_addr addr;
    char** p_alias;
    std::vector<std::string> vec_ip;

    p_remote_host = gethostbyname(hostname.c_str());
    if (p_remote_host == nullptr) {
        auto err = WSAGetLastError();
        if (err != 0) {
            std::cerr << "[ERROR] Failed to get host by hostname: " << hostname << std::endl;
            if (err == WSAHOST_NOT_FOUND) {
                std::cerr << "Host not found\n";
            }
            else if (err == WSANO_DATA) {
                std::cerr << "No data record found\n";
            }
            else {
                printf("Function failed with error: %ld\n", err);
            }
            on_error = true;
        }
    }
    else {
        int i = 0;
        std::cout << "Official name: " << p_remote_host->h_name;
        for (p_alias = p_remote_host->h_aliases; *p_alias!= 0; p_alias++) {
            std::cout << "\tAlternate name #" << ++i << ": " << *p_alias << std::endl;
        }
        std::cout << "\tAddress type: ";
        switch (p_remote_host->h_addrtype) {
        case AF_INET:
            std::cout << "AF_INET\n";
            break;
        case AF_NETBIOS:
            std::cout << "AF_NETBIOS\n";
            break;
        default:
            std::cout << p_remote_host->h_addrtype << std::endl;
            break;
        }
        std::cout << "\tAddress length: " << p_remote_host->h_length << std::endl;
        
        i = 0;
        if (p_remote_host->h_addrtype == AF_INET)
        {
            while (p_remote_host->h_addr_list[i] != 0) {
                addr.s_addr = *(u_long*)p_remote_host->h_addr_list[i++];
                std::string ip_addr(inet_ntoa(addr));
                std::cout << "\tIP Address # " << i << ": " << ip_addr << std::endl;
                vec_ip.push_back(ip_addr);
            }
        }
        else if (p_remote_host->h_addrtype == AF_NETBIOS)
        {
            std::cout << "NETBIOS address was returned\n";
        }
    }
    return vec_ip;
}

int GetIcmpEchoRTT(const std::string& ip) {
    // Declare and initialize variables

    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    std::vector<uint8_t> ReplyBuffer;
    DWORD ReplySize = 0;
    int result_rtt = -1;

    // Validate the parameters
    ipaddr = inet_addr(ip.c_str());
    if (ipaddr == INADDR_NONE) {
        std::cerr << "Invalid IP Address: " << ip << std::endl;
        return -1;
    }

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        std::cerr << "\tUnable to open handle.\n" ;
        std::cerr << "IcmpCreatefile returned error: " << GetLastError() << std::endl;;
        return -1;
    }

    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer.resize(size_t(ReplySize));


    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
        NULL, ReplyBuffer.data(), ReplySize, 1000);
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer.data();
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        //std::cout << "\tSent icmp message to " << ip << '\n';
        //std::cout << "\t  Received from " << inet_ntoa(ReplyAddr) << std::endl;
        //std::cout << "\t  Status = \n" << pEchoReply->Status << std::endl;
        //std::cout << "\t  Roundtrip time = " << pEchoReply->RoundTripTime << "ms\n";
        result_rtt = pEchoReply->RoundTripTime;
    }
    else {
        std::cerr << "\tCall to IcmpSendEcho failed.\n" << std::endl;
        std::cerr << "\tIcmpSendEcho returned error: " << GetLastError() << '\n' << std::endl;
        return -1;
    }
    return result_rtt;
}

int SendIcmpEchoTest(const std::string& ip) {

    // Declare and initialize variables

    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    std::vector<uint8_t> ReplyBuffer;
    DWORD ReplySize = 0;

    // Validate the parameters
    ipaddr = inet_addr(ip.c_str());
    if (ipaddr == INADDR_NONE) {
        std::cerr << "Invalid IP Address: " << ip << std::endl;
        return 1;
    }

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("\tUnable to open handle.\n");
        printf("IcmpCreatefile returned error: %ld\n", GetLastError());
        return 1;
    }

    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer.resize(size_t(ReplySize));


    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
        NULL, ReplyBuffer.data(), ReplySize, 1000);
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer.data();
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        std::cout << "\tSent icmp message to " << ip << '\n';
        if (dwRetVal > 1) {
            printf("\tReceived %ld icmp message responses\n", dwRetVal);
            printf("\tInformation from the first response:\n");
        }
        else {
            printf("\tReceived %ld icmp message response\n", dwRetVal);
            printf("\tInformation from this response:\n");
        }
        printf("\t  Received from %s\n", inet_ntoa(ReplyAddr));
        printf("\t  Status = %ld\n",
            pEchoReply->Status);
        printf("\t  Roundtrip time = %ld milliseconds\n",
            pEchoReply->RoundTripTime);
    }
    else {
        printf("\tCall to IcmpSendEcho failed.\n");
        printf("\tIcmpSendEcho returned error: %ld\n", GetLastError());
        return 1;
    }
    return 0;
}

uint64_t GetHttpServerTime(const std::string& host)
{

    // Clear output buffer
    SYSTEMTIME sysTime;
    SecureZeroMemory(&sysTime, sizeof(SYSTEMTIME));

    // Open connection
    HINTERNET hInternetSession = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternetSession)
        return timeutil::GetUnixTimestampFromSysTime(sysTime);
    std::string uri = std::string("http://") + host;
    std::wstring uri_w;
    uri_w.assign(uri.begin(), uri.end());
    //std::wcout << uri_w << std::endl;

    HINTERNET hInternetFile = InternetOpenUrl(hInternetSession,
        uri_w.c_str(), 0, 0,
        INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);


    if (!hInternetFile)
    {
        InternetCloseHandle(hInternetSession);
        return timeutil::GetUnixTimestampFromSysTime(sysTime);
    }

    // Query date time in format systemtime
    DWORD dwSize = sizeof(SYSTEMTIME);
    if (!HttpQueryInfo(hInternetFile, HTTP_QUERY_DATE |
        HTTP_QUERY_FLAG_SYSTEMTIME, &sysTime, &dwSize, NULL))
    {
        InternetCloseHandle(hInternetSession);
        InternetCloseHandle(hInternetFile);
        return timeutil::GetUnixTimestampFromSysTime(sysTime);
    }

    // Clean up ...
    InternetCloseHandle(hInternetFile);
    InternetCloseHandle(hInternetSession);
    return timeutil::GetUnixTimestampFromSysTime(sysTime);
}



}
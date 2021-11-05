#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

int main(int argc, char* argv[])
{
    //Initialize Winsock
    WSADATA wsaData;
    int iresult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(iresult != NO_ERROR)
    {
        std::cout << "Error in WSAStartup: " << WSAGetLastError() << std::endl;
        return -1;
    }

    //Create UDP socket
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == INVALID_SOCKET)
    {
        std::cout << "Error in Creating socket" << std::endl;
        return -1;
    }

    //Create remote address struct from IP and port
    std::string host = "35.85.114.0", service = "8888";
    addrinfo hint{};
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;

    addrinfo* result;
    int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
    if(error != 0 && result != nullptr)
    {
        std::cout << "Error in creating socket address" << std::endl;
        return -1;
    }
    while (!result->ai_addr && result->ai_next)
    {
        result = result->ai_next;
    }
    if (!result->ai_addr)
    {
        return -1;
    }

    //Send data over socket
    sendto(s, static_cast<const char*>(argv[0]), (int)strlen(argv[0]),
        0, (result->ai_addr), (int)result->ai_addrlen);

    //Listen for response
    char recvBuffer[10000];
    socklen_t fromlen = (socklen_t)result->ai_addrlen;
    const int byteReceivedCount = recvfrom(s, recvBuffer, sizeof(recvBuffer), 0, result->ai_addr, &fromlen);

    if(byteReceivedCount < 0)
    {
        std::cout << "Error in receiving packet" << WSAGetLastError() << std::endl;
        return -1;
    }
    else //print received buffer if received data.
    {
        std::string recvToString(recvBuffer);
        auto pos = recvToString.find_last_of('.');
        if(pos != std::string::npos)
        {
            std::cout << recvToString.substr(0, pos) << '.' << std::endl;
        }
        else
            std::cout << recvToString << std::endl;
    }

    //Free address info returned by getaddrinfo()
    freeaddrinfo(result);
    //Close socket
    closesocket(s);
    //Shutdown Winsock
    WSACleanup();

    return 0;
}


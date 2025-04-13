#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    char *shutdownServerMessage = "Shutdown";
    char *calculateMessage = "Calculate";
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    int num;
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    printf("%s \n", recvbuf);

    char option;

    printf("0) Shutdown the server \n1) Calculate something\n");
    option = getchar();
    getchar();
    switch (option)
    {
    case '0':
        iResult = send(ConnectSocket, shutdownServerMessage, (int)strlen(shutdownServerMessage) + 1, 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        printf("Send request to shutdown server\n");
        break;
    case '1':
        iResult = send(ConnectSocket, calculateMessage, (int)strlen(calculateMessage) + 1, 0);
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        printf("%s \n", recvbuf);
        scanf("%d", &num);
        iResult = send(ConnectSocket, (char *)&num, sizeof(int), 0);
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        printf("%s \n", recvbuf);
        scanf("%d", &num);
        iResult = send(ConnectSocket, (char *)&num, sizeof(int), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        printf("Send request to calculate\n");
        do
        {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (strcmp(recvbuf, "Done") == 0) // serwer zakończył obliczenia
            {
                printf("%s \n", recvbuf);          
                iResult = shutdown(ConnectSocket, SD_SEND);
                if (iResult == SOCKET_ERROR)
                {
                    printf("shutdown failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }
                break;
            }
            else if (iResult > 1)
                printf("%s \n", recvbuf);
            else if (iResult == 0)
                printf("Connection closed\n");
            else
                printf("recv failed with error: %d\n", WSAGetLastError()); // 10054 oznacza że serwer się wyłączył

        } while (iResult > 0);
        break;
    default:
        break;
    }
    // cleanup
    printf("Disconnecting \n");
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
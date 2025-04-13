#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

CRITICAL_SECTION CriticalSection;

DWORD WINAPI fun(LPVOID arg);

struct dataForThread
{
    int id;
    SOCKET ClientSocket;
    SOCKET ListenSocket;
};

DWORD WINAPI fun(LPVOID arg)
{
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    char *finishedMessage = {"Done"};
    char *hiMessage = {"Greetings user"};
    char *lowerBoundMessage = {"Please enter the lower bound of the interval"};
    char *upperBoundMessage = {"Now enter the upper bound of the interval"};
    int recvbuflen = DEFAULT_BUFLEN;

    struct dataForThread *data = (struct dataForThread *)arg;
    SOCKET ClientSocket = data->ClientSocket;
    SOCKET ListenSocket = data->ListenSocket;
    int id = data->id;

    int lower;
    int upper;
    int iResult;

    iSendResult = send(ClientSocket, hiMessage, strlen(hiMessage) + 1, 0);

    printf("Waiting for requests from user %d\n", id);
    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (strcmp(recvbuf, "Shutdown") == 0)
        {
            printf("Shutting down\n");
            iResult = shutdown(ClientSocket, SD_SEND);
            if (iResult == SOCKET_ERROR)
            {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            closesocket(ClientSocket);
            closesocket(ListenSocket);
            WSACleanup();
            DeleteCriticalSection(&CriticalSection);
            ExitProcess(0);
        }
        if (strcmp(recvbuf, "Calculate") == 0)
        {
            iSendResult = send(ClientSocket, lowerBoundMessage, strlen(lowerBoundMessage) + 1, 0);
            iResult = recv(ClientSocket, (char *)&lower, sizeof(int), 0);

            iSendResult = send(ClientSocket, upperBoundMessage, strlen(upperBoundMessage) + 1, 0);
            iResult = recv(ClientSocket, (char *)&upper, sizeof(int), 0);

            printf("Calculating for user %d \n", id);
            // szukamy wszystkich liczb z ilością dzielników równą 10
            for (int i = lower; i <= upper; i++)
            {
                int counter = 0;
                for (int j = 1; j <= i; j++)
                {
                    if (i % j == 0)
                        counter++;
                }
                if (counter == 10)
                {
                    EnterCriticalSection(&CriticalSection);
                    FILE *fptr;
                    fptr = fopen("results.txt", "a");
                    fprintf(fptr, "User %d number: %d has 10 divisors \n", id, i);
                    fclose(fptr);
                    LeaveCriticalSection(&CriticalSection);
                }
            }
            iSendResult = send(ClientSocket, finishedMessage, strlen(finishedMessage) + 1, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Calculations for user %d completed \n", id);
            break;
        }
    } while (1);
    printf("Connection closing with user %d \n", id);
    closesocket(ClientSocket);
    return 0;
}

void createNewThread(struct dataForThread data)
{
    DWORD id; // identyfikator watku
    HANDLE newThreadHandle = CreateThread(NULL, 0, fun, (LPVOID)&data, 0, &id);

    if (newThreadHandle != NULL)
    {
        SetThreadPriority(newThreadHandle, THREAD_PRIORITY_NORMAL);
    }
    else
        printf("Error new Thred coulnd not be create\n");
}

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;
    InitializeCriticalSection(&CriticalSection);

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET NewClientSocket = INVALID_SOCKET;
    int id = 0;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    struct dataForThread data;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server ready \n");
    do
    {
        NewClientSocket = accept(ListenSocket, NULL, NULL);
        if (NewClientSocket != INVALID_SOCKET)
        {
            printf("Client Accepted \n");
            data.id = id;
            data.ClientSocket = NewClientSocket;
            data.ListenSocket = ListenSocket;
            createNewThread(data);
            id++;
        }
    } while (1);
    return 0;
}
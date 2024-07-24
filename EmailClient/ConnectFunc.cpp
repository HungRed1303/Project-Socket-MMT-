#include "ConnectFunc.h"

int CreateWinsock() {

    // Initialize Winsock
    WSADATA wsaData;
    WORD wVersionRequest = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequest, &wsaData) != 0) {
        cout << "WSAStartup failed.\n";
        return -1;
    }
    return 0;
}

SOCKET CreateSocket() {

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    return sock;
}

int ConnectServer(const SOCKET& clientSocket, const int PORT, string server) {

    sockaddr_in service;
    service.sin_family = AF_INET;

    wstring Lserver = StrToWstr(server);
    InetPton(AF_INET, Lserver.c_str(), &service.sin_addr.s_addr);

    service.sin_port = htons(PORT);

    if (connect(clientSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "Client connect() - Failed to connect!" << endl;
        WSACleanup();
        return -1;
    }
    return 0;
}

void CheckSend(int byteCount) {

    if (byteCount < 0) {
        cout << "Client: error " << WSAGetLastError() << endl;
        WSACleanup();
    }
}

void CheckReceive(int byteCount, char* receiveBuffer) {

    if (byteCount < 0) {
        cout << "Client: error " << WSAGetLastError() << endl;
        WSACleanup();
    }
}

#include "ReceiveMailFunc.h"

string GetData(const SOCKET& clientSocket, int iMess, size_t size) {

    Mail ret;

    char* receiveBuffer = new char[size];
    memset(receiveBuffer, '\0', size);
    string emailData = "";
    string data = "";

    emailData = "RETR " + to_string(iMess) + "\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));

    int byteRead = 0;
    // Read every small packet
    do {
        byteRead = recv(clientSocket, receiveBuffer, static_cast<int>(size), 0);
        if (byteRead > 0) {
            data.append(receiveBuffer, byteRead);
        }
    } while (data[data.length() - 3] != '.');

    return data;
}

void ReceiveMail(string username, string password) {

    Config config = ReadConfig("XMLFile.xml");
    // Create Winsock
    if (CreateWinsock() == -1) {
        return;
    }
    // Create socket
    SOCKET clientSocket = CreateSocket();
    if (clientSocket == INVALID_SOCKET) {
        return;
    }
    // Connect to POP3 server
    if (ConnectServer(clientSocket, config.pop3Port, config.mailServer) == -1) {
        return;
    }
    string emailData = "";
    char receiveBuffer[MAX_SIZE_BUFFER] = "";

    // Receive connect signal from POP3 server
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    // Communicate with POP3 server
    emailData = "CAPA\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    memset(receiveBuffer, '\0', strlen(receiveBuffer));
    emailData = "USER " + config.address + "\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    memset(receiveBuffer, '\0', strlen(receiveBuffer));
    emailData = "PASS " + password + "\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    // Get number of emails and total size
    memset(receiveBuffer, '\0', strlen(receiveBuffer));
    emailData = "STAT\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    pair <size_t, size_t> STATData = GetDataFromSTAT(receiveBuffer);
    size_t numberMails = STATData.first;
    size_t totalSize = STATData.second;

    memset(receiveBuffer, '\0', strlen(receiveBuffer));
    emailData = "LIST\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    vector <size_t> sizeMails = GetSizeOfMails(receiveBuffer, numberMails);

    for (int i = 0; i < numberMails; i++) {
        string data = GetData(clientSocket, i + 1, sizeMails[i]);
        SaveMail(data, "temp", GetPath());
        Mail mail = GetInfoFromMsgFile("temp.msg", GetPath());
        string messID = GetMessID(data);
        // Filter - Save mail to the corresponding folder
        if (ContainsKeyword(mail.subject, config.important)) {
            SaveMail(data, messID, GetPath() / "Important");
            // Update buffer.txt (Save info seen mail)
            UpdateStatusFile(messID, GetPath() / "Important");
        }
        else if (ContainsKeyword(mail.content, config.work)) {
            SaveMail(data, messID, GetPath() / "Work");
            // Update buffer.txt (Save info seen mail)
            UpdateStatusFile(messID, GetPath() / "Work");
        }
        else if (ContainsKeyword(mail.content, config.spam) || ContainsKeyword(mail.subject, config.spam)) {
            SaveMail(data, messID, GetPath() / "Spam");
            // Update buffer.txt (Save info seen mail)
            UpdateStatusFile(messID, GetPath() / "Spam");
        }
        else {
            SaveMail(data, messID, GetPath() / "Inbox");
            // Update buffer.txt (Save info seen mail)
            UpdateStatusFile(messID, GetPath() / "Inbox");
        }
        // Filter - Save mail to the corresponding folder
        SaveMail(data, messID, GetPath() / "Project");
        // Update buffer.txt (Save info seen mail)
        UpdateStatusFile(messID, GetPath() / "Project");
    }
    // Send message to finish communication
    memset(receiveBuffer, '\0', strlen(receiveBuffer));
    emailData = "QUIT\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    // Cleanup and close connect
    closesocket(clientSocket);
    WSACleanup();

    return;
}
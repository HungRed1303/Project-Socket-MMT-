#include "SendMailFunc.h"

string GetContentFormat(const string& content) {

    string emailData = "";
    // Format content
    emailData += "Content-Type: text/plain; charset=UTF-8; format=flowed\n";
    emailData += "Content-Transfer-Encoding: 7bit\n\n";
    emailData += content + "\n\n";

    return emailData;
}

string GetMimeFormat(const vector <string>& toList, const vector <string>& ccList, 
                     const vector <string>& bccList, const string& subject) {

    Config config = ReadConfig("XMLFile.xml");
    string emailData = "";
    size_t first = config.address.find("@", 0) + 1;
    size_t last = config.address.length();
    string tail = config.address.substr(first, last - first);
    // Format MIME
    emailData = "Message-ID: <" + GenerateUUID() + "@" + tail +  ">\n";
    emailData += "Date: " + GetTime();
    emailData += "MIME-Version: 1.0\n";
    emailData += "User-Agent: Mail Client nhom 11\n";
    emailData += "Content-Language: en-US\n";

    if (!toList.empty()) {
        emailData += "To:";
        for (int i = 0; i < toList.size(); i++) {
            emailData += " " + toList[i] + ((i < toList.size() - 1) ? "," : "\n");
        };
    }
    if (!ccList.empty()) {
        emailData += "Cc:";
        for (int i = 0; i < ccList.size(); i++) {
            emailData += " " + ccList[i] + ((i < ccList.size() - 1) ? "," : "\n");
        };
    }
    
    emailData += "From: " + config.username + " <" + config.address + ">\n";
    emailData += "Subject: " + subject + "\n";
    
    return emailData;
}

int SendMail(const vector <string>& toList, const vector <string>& ccList, const vector <string>& bccList, 
             const string& subject, const string& content, const vector <string>& filePath) {

    Config config = ReadConfig("XMLFile.xml");
    // Create Winsock
    if (CreateWinsock() == -1) {
        return -1;
    }
    // Create socket
    SOCKET clientSocket = CreateSocket();
    if (clientSocket == INVALID_SOCKET) {
        return -1;
    }
    // Connect to SMTP server
    if (ConnectServer(clientSocket, config.smtpPort, config.mailServer) == -1) {
        return -1;
    }
    // Communication between client and server
    string emailData = "";
    char receiveBuffer[MAX_SIZE_BUFFER] = "";
    // Receive connect signal from SMTP server
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    emailData = "EHLO [" + config.mailServer + "]\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    emailData = "MAIL FROM:<" + config.username + "<" + config.address + ">" + ">\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    // Send recipients' information 
    for (string to : toList) {
        emailData = "RCPT TO:<" + to + ">\n";
        CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
        CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);
    }

    for (string to : ccList) {
        emailData = "RCPT TO:<" + to + ">\n";
        CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
        CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);
    }

    for (string to : bccList) {
        emailData = "RCPT TO:<" + to + ">\n";
        CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
        CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);
    }

    emailData = "DATA\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    // Check attachment
    if (!filePath.empty()) {
        AttachFile(clientSocket, toList, ccList, bccList, subject, content, filePath);
    }
    else {
        emailData = GetMimeFormat(toList, ccList, bccList, subject);
        emailData += GetContentFormat(content) + ".\n";
        CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
        CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);
    }

    // Cleanup and close connect
    closesocket(clientSocket);
    WSACleanup();
    cout << "Gui email thanh cong.\n" << endl;

    return 0;
}

int AttachFile(const SOCKET& clientSocket, const vector <string>& toList, const vector <string>& ccList, const vector <string>& bccList,
               const string& subject, const string& content, const vector <string>& filePath) {

    // Communication between client and config.mailServer
    string emailData = "";
    char receiveBuffer[MAX_SIZE_BUFFER] = "";

    // Send MIME information
    string boundary = "simple-boundary";
    emailData = "Content-Type: multipart/mixed; boundary=" + boundary + "\n";

    emailData += GetMimeFormat(toList, ccList, bccList, subject);

    emailData += "\nThis is a multi-part message in MIME format.\n";
    emailData += "--" + boundary + "\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    // Send content information
    emailData = GetContentFormat(content);
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));

    // Attach files
    for (int i = 0; i < (int)filePath.size(); i++) {
        emailData = "--" + boundary + "\n";
        CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));

        string fileExtension = GetFileExtension(filePath[i]);
        wstring mimeType = GetMimeTypeFromString(StrToWstr(fileExtension));
        emailData = WstrToStr(L"Content-Type: " + mimeType + L"; name=\"" + StrToWstr(GetFilename(filePath[i])) + L"\"\n");
        emailData += "Content-Disposition: attachment; filename=" + GetFilename(filePath[i]) + "\n";
        emailData += "Content-Transfer-Encoding: base64\n\n";
        CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));

        // Open attachment
        ifstream attachment(filePath[i], ios::binary);
        if (attachment.is_open() == true) {

            attachment.seekg(0, ios::end);
            size_t fileSize = attachment.tellg();
            attachment.seekg(0, ios::beg);

            // Read data in attachment
            char* fileContent = new char[fileSize];
            attachment.read(fileContent, fileSize);
            string fileContentBase64 = Base64Encode(fileContent, fileSize);

            int start = 0;
            // Divide data to every small packet with size is 72
            while (start < fileContentBase64.length()) {
                string data = fileContentBase64.substr(start, min(PACKET_SIZE, fileContentBase64.length() - start)) + "\n";
                send(clientSocket, data.c_str(), static_cast<int>(data.length()), 0);
                start += PACKET_SIZE;
            }
            emailData = "\n";
            CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));

            attachment.close();
            delete[] fileContent;
        }
        else {
            cout << "File path is not correct.\n";
        }
    }

    // Send finish signal
    emailData = "--" + boundary + "--\n";
    emailData += "\n.\n";
    CheckSend(send(clientSocket, emailData.c_str(), static_cast<int>(emailData.size()), 0));
    CheckReceive(recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0), receiveBuffer);

    return 0;
}
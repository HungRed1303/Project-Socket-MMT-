#include "StringFunc.h"

string base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string Base64Encode(const char* data, size_t length) {

    string encoded;
    size_t i = 0;

    while (i < length) {
        // Character 1
        encoded += base64Table[(data[i] & 0xFC) >> 2];
        // Character 2
        encoded += base64Table[((data[i] & 0x03) << 4) | ((data[i + 1] & 0xF0) >> 4)];
        i++;
        if (i < length) {
            // Character 3
            encoded += base64Table[((data[i] & 0x0F) << 2) | ((data[i + 1] & 0xC0) >> 6)];
            i++;
        }
        else {
            // Padding
            encoded += '=';
        }
        if (i < length) {
            // Character 4
            encoded += base64Table[data[i] & 0x3F];
            i++;
        }
        else {
            // Padding
            encoded += '=';
        }
    }
    return encoded;
}

string Base64Decode(string encoded_string) {

    string result = "";
    int size = (int)encoded_string.size();
    int i = 0;

    while (size-- && (encoded_string[i] != '=') && isBase64(encoded_string[i])) {
        char c1 = encoded_string[i++];
        char c2 = encoded_string[i++];
        char c3 = encoded_string[i++];
        char c4 = encoded_string[i++];

        char decoded_c1 = char(base64Table.find(c1));
        char decoded_c2 = char(base64Table.find(c2));
        char decoded_c3 = char(base64Table.find(c3));
        char decoded_c4 = char(base64Table.find(c4));

        char byte1 = (decoded_c1 << 2) | (decoded_c2 >> 4);
        char byte2 = ((decoded_c2 & 0x0F) << 4) | (decoded_c3 >> 2);
        char byte3 = ((decoded_c3 & 0x03) << 6) | decoded_c4;

        result += byte1;
        if (c3 != '=') {
            result += byte2;
        }
        if (c4 != '=') {
            result += byte3;
        }
    }

    return result;
}

bool isBase64(unsigned char c) {

    return (isalnum(c) || (c == '+') || (c == '/'));
}

string GetFileExtension(const string& filePath) {

    string extension = "null";
    for (int i = 0; i < filePath.length(); i++) {
        if (filePath[i] == '.') {
            extension = filePath.substr(i, filePath.length() - i);
            break;
        }
    }
    return extension;
}

string GetFilename(const string& filePath) {

    string filename;
    size_t lastBackslash = filePath.find_last_of('/');
    filename = filePath.substr(lastBackslash + 1, filePath.size() - lastBackslash - 1);

    return filename;
}

wstring StrToWstr(const string& str) {

    // Source: Chat GPT
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wchar_t* buffer = new wchar_t[bufferSize];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, bufferSize);
    wstring wideStr(buffer);
    delete[] buffer;

    return wideStr;
}

string WstrToStr(wstring wstr) {

    // Source: Chat GPT
    string narrowStr;
    for (wchar_t wideChar : wstr) {
        narrowStr.push_back(static_cast<char>(wideChar));
    }
    return narrowStr;
}

pair <size_t, size_t> GetDataFromSTAT(const char* str) {

    string st = string(str);
    size_t firstSpace = st.find_first_of(' ', 0);
    size_t secondSpace = st.find_first_of(' ', firstSpace + 1);
    size_t numMails = stoi(st.substr(firstSpace + 1, secondSpace - firstSpace - 1));
    size_t size = stoi(st.substr(secondSpace + 1, st.length() - secondSpace - 1));
    return make_pair(numMails, size);
}

Mail GetInfoFromMsgFile(string filename, const filesystem::path& path) {

    Mail ret;
    // Open file to get data
    ifstream read(path / filename, ios::binary);
    if (!read.is_open()) {
        return ret;
    }
    string data = "";
    string line = "";
    while (getline(read, line)) {
        data += line;
    }
    string needle = "";
    // Get time
    needle = "Date: ";
    size_t pos = 0, endLine = 0;
    pos = data.find(needle, 0) + needle.length();
    endLine = data.find("\r", pos);
    ret.date = data.substr(pos, endLine - pos);

    // Check mail has attachment
    size_t pivot = data.find("boundary", 0);
    bool hasAttachments = (pivot != string::npos && pivot < pos);
    // Get sender
    needle = "From: ";
    pos = data.find(needle, endLine) + needle.length();
    endLine = data.find("\r", pos);
    ret.sender = data.substr(pos, endLine - pos);
    // Get subject
    needle = "Subject: ";
    pos = data.find(needle, endLine) + needle.length();
    endLine = data.find("\r", pos);
    if (endLine > pos) {
        ret.subject = data.substr(pos, endLine - pos);
    }
    else {
        ret.subject = "";
    }
    // Get content
    needle = "Content-Transfer-Encoding:";
    pos = data.find(needle, endLine);
    needle = "\r";
    pos = data.find(needle, pos + 1) + needle.length() * 2;

    if (hasAttachments == true) {
        endLine = data.find("--", pos) - needle.length() * 2;
    }
    else {
        endLine = data.find(".", pos) - needle.length() * 2;
    }
    if (endLine > pos) {
        ret.content = data.substr(pos, endLine - pos); \
    }
    else {
        ret.content = "";
    }

    // Get full data in mail
    // Check attachments
    if (hasAttachments == true) {
        // Mail has attachments
        vector <string> names;
        vector <string> content;
        do {
            // Get filename
            needle = "filename=\"";
            pos = data.find(needle, endLine) + needle.length();
            endLine = data.find("\"", pos);
            names.push_back(data.substr(pos, endLine - pos));
            // Get data
            needle = "Content-Transfer-Encoding";
            pos = data.find(needle, endLine) + needle.length();
            needle = "\r";
            pos = data.find(needle, pos + 1) + needle.length() * 2;
            endLine = data.find("--", pos) - needle.length() * 2;

            content.push_back(data.substr(pos, endLine - pos));
        } while (data.find("filename=", endLine) != string::npos);
        ret.names = names;
        ret.fileData = content;
    }
    return ret;
}

string GetMessID(const string& data) {

    string needle = "Message-ID: <";
    size_t pos = data.find(needle, 0) + needle.length();
    size_t endLine = data.find("@", pos);
    return data.substr(pos, endLine - pos);
}

bool ContainsKeyword(const std::string& text, const std::string& keyword) {
    // Tìm kiếm từ khóa trong chuỗi với std::string::find
    // Nếu std::string::find không tìm thấy từ khóa, nó sẽ trả về std::string::npos
    return text.find(keyword) != std::string::npos;
}